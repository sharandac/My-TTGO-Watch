#include "config.h"
#include "parkinsense_ble.h"
#include "hardware/blectl.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #include "NimBLEDevice.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"

    static NimBLECharacteristic *pParkinsenseTXCharacteristic = NULL;
    static int parkinsense_ble_fail_reason = 0;

    #define PARKINSENSE_BLE_MIN_CHUNK    20
    #define PARKINSENSE_BLE_CHUNK_DELAY  50
#endif

int parkinsense_ble_last_fail_reason(void) {
#ifndef NATIVE_64BIT
    return parkinsense_ble_fail_reason;
#else
    return 0;
#endif
}

void parkinsense_ble_setup(void) {
#ifndef NATIVE_64BIT
    NimBLEServer *pServer = blectl_get_ble_server();
    NimBLEAdvertising *pAdvertising = blectl_get_ble_advertising();

    NimBLEService *pService = pServer->createService(NimBLEUUID(PARKINSENSE_SERVICE_UUID));

    /* Encrypted read triggers GATT_INSUFFICIENT_* until the link is paired. */
    NimBLECharacteristic *pPairProbe = pService->createCharacteristic(
        NimBLEUUID(PARKINSENSE_CHARACTERISTIC_UUID_PAIR),
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC
    );
    pPairProbe->setValue((uint8_t)1);

    pParkinsenseTXCharacteristic = pService->createCharacteristic(
        NimBLEUUID(PARKINSENSE_CHARACTERISTIC_UUID_TX),
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::NOTIFY
    );
    pParkinsenseTXCharacteristic->setValue((uint8_t)0);

    pService->start();
    pAdvertising->addServiceUUID(pService->getUUID());
    log_i("[PWR_BLE] parkinsense_ble_setup: encrypted service registered");
#endif
}

bool parkinsense_ble_send(const char *data, size_t len) {
#ifndef NATIVE_64BIT
    if (!pParkinsenseTXCharacteristic) {
        parkinsense_ble_fail_reason = 1;
        return false;
    }

    NimBLEServer *pServer = blectl_get_ble_server();
    if (!pServer) {
        parkinsense_ble_fail_reason = 2;
        return false;
    }
    if (pServer->getConnectedCount() == 0) {
        parkinsense_ble_fail_reason = 3;
        return false;
    }

    size_t chunk_size = PARKINSENSE_BLE_MIN_CHUNK;
    std::vector<uint16_t> peers = pServer->getPeerDevices();
    if (!peers.empty()) {
        uint16_t mtu = pServer->getPeerMTU(peers[0]);
        if (mtu > 3)
            chunk_size = mtu - 3;
        if (chunk_size < PARKINSENSE_BLE_MIN_CHUNK)
            chunk_size = PARKINSENSE_BLE_MIN_CHUNK;
    }

    uint16_t subCount = pParkinsenseTXCharacteristic->getSubscribedCount();
    log_i("ble_send: len=%u mtu_chunk=%u subs=%u peers=%u", (unsigned)len, (unsigned)chunk_size, (unsigned)subCount, (unsigned)peers.size());

    size_t pos = 0;
    while (pos < len) {
        if (pServer->getConnectedCount() == 0) {
            parkinsense_ble_fail_reason = 4;
            return false;
        }

        size_t remaining = len - pos;
        size_t chunk_len = (remaining > chunk_size) ? chunk_size : remaining;
        size_t split = pos + chunk_len;

        if (split < len) {
            for (size_t i = split; i > pos; i--) {
                char c = data[i - 1];
                if (c == ']' || c == ',' || c == '"') {
                    split = i;
                    break;
                }
            }
            chunk_len = split - pos;
            /* Never send '}' as first char of a chunk. If prev chunk is lost we'd get
             * "s":[}+'}' which parses as "Unexpected character: }". Keep ]} together. */
            if (split < len && data[split] == '}') {
                split++;
                chunk_len = split - pos;
            }
        }

        pParkinsenseTXCharacteristic->setValue((const uint8_t*)(data + pos), chunk_len);
        pParkinsenseTXCharacteristic->notify();
        pos += chunk_len;

        if (pos < len)
            vTaskDelay(pdMS_TO_TICKS(PARKINSENSE_BLE_CHUNK_DELAY));
    }

    const uint8_t newline = '\n';
    pParkinsenseTXCharacteristic->setValue(&newline, 1);
    pParkinsenseTXCharacteristic->notify();
    vTaskDelay(pdMS_TO_TICKS(PARKINSENSE_BLE_CHUNK_DELAY));

    parkinsense_ble_fail_reason = 0;
    return true;
#else
    log_i("parkinsense_ble_send (native stub): %.*s", (int)len, data);
    return true;
#endif
}
