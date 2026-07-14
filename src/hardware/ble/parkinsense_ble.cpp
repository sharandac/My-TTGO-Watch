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

    #define PARKINSENSE_BLE_MIN_CHUNK         20
    #define PARKINSENSE_BLE_CHUNK_DELAY_FAST   25
    #define PARKINSENSE_BLE_CHUNK_DELAY_MED    50
    #define PARKINSENSE_BLE_CHUNK_DELAY_SLOW   80
    #define PARKINSENSE_BLE_PREFERRED_MTU      247
    #define PARKINSENSE_BLE_MTU_WAIT_MS        6000

    static uint32_t parkinsense_ble_connect_ms = 0;
    static uint16_t parkinsense_ble_cached_mtu = 23;
    static bool parkinsense_ble_mtu_exchanged = false;
    static bool parkinsense_ble_notify_subscribed = false;
    static uint32_t parkinsense_ble_last_wait_log_ms = 0;

    #define PARKINSENSE_BLE_POST_SUBSCRIBE_MS  400

    static uint32_t parkinsense_ble_now_ms(void) {
        return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
    }

    class ParkinsenseTXCallbacks : public NimBLECharacteristicCallbacks {
        void onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue) {
            (void)desc;
            parkinsense_ble_notify_subscribed = (subValue & 0x0003) != 0;
            log_i("[PWR_BLE] parkinsense TX onSubscribe subValue=0x%04x subs=%u",
                subValue, (unsigned)pCharacteristic->getSubscribedCount());
        }
        void onUnsubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue) {
            (void)pCharacteristic;
            (void)desc;
            (void)subValue;
            parkinsense_ble_notify_subscribed = false;
            log_i("[PWR_BLE] parkinsense TX onUnsubscribe");
        }
    };

    static bool parkinsense_ble_has_notify_subscriber(void) {
        if (parkinsense_ble_notify_subscribed)
            return true;
        return pParkinsenseTXCharacteristic &&
            pParkinsenseTXCharacteristic->getSubscribedCount() > 0;
    }

    static void parkinsense_ble_log_wait_state(void) {
        if (!parkinsense_ble_connect_ms)
            return;
        uint32_t now = parkinsense_ble_now_ms();
        if ((now - parkinsense_ble_last_wait_log_ms) < 5000)
            return;
        parkinsense_ble_last_wait_log_ms = now;
        NimBLEServer *pServer = blectl_get_ble_server();
        if (!pServer || pServer->getConnectedCount() == 0)
            return;
        uint16_t subs = pParkinsenseTXCharacteristic
            ? (uint16_t)pParkinsenseTXCharacteristic->getSubscribedCount()
            : 0;
        log_i("[PWR_BLE] parkinsense_ble: waiting to send subs=%u notify_cb=%d mtu=%u exchanged=%d elapsed=%ums",
            (unsigned)subs, (int)parkinsense_ble_notify_subscribed,
            (unsigned)parkinsense_ble_cached_mtu, (int)parkinsense_ble_mtu_exchanged,
            (unsigned)(now - parkinsense_ble_connect_ms));
    }

    static uint32_t parkinsense_ble_chunk_delay_ms(uint16_t mtu) {
        if (mtu >= 200)
            return PARKINSENSE_BLE_CHUNK_DELAY_FAST;
        if (mtu >= 64)
            return PARKINSENSE_BLE_CHUNK_DELAY_MED;
        return PARKINSENSE_BLE_CHUNK_DELAY_SLOW;
    }

    static uint16_t parkinsense_ble_resolve_peer_mtu(NimBLEServer *pServer) {
        if (!pServer || pServer->getConnectedCount() == 0)
            return parkinsense_ble_cached_mtu;
        std::vector<uint16_t> peers = pServer->getPeerDevices();
        if (peers.empty())
            return parkinsense_ble_cached_mtu;
        uint16_t mtu = pServer->getPeerMTU(peers[0]);
        if (mtu > 0)
            parkinsense_ble_cached_mtu = mtu;
        return parkinsense_ble_cached_mtu;
    }
#endif

bool parkinsense_ble_pairing_required(void) {
#ifdef PARKINSENSE_BLE_UNSECURED
    return false;
#else
    return true;
#endif
}

int parkinsense_ble_last_fail_reason(void) {
#ifndef NATIVE_64BIT
    return parkinsense_ble_fail_reason;
#else
    return 0;
#endif
}

void parkinsense_ble_on_client_connected(void) {
#ifndef NATIVE_64BIT
    parkinsense_ble_connect_ms = parkinsense_ble_now_ms();
    parkinsense_ble_cached_mtu = 23;
    parkinsense_ble_mtu_exchanged = false;
    parkinsense_ble_notify_subscribed = false;
    parkinsense_ble_last_wait_log_ms = 0;
    log_i("[PWR_BLE] parkinsense_ble: link up, waiting for notify subscribe + MTU");
#endif
}

void parkinsense_ble_on_client_disconnected(void) {
#ifndef NATIVE_64BIT
    parkinsense_ble_connect_ms = 0;
    parkinsense_ble_cached_mtu = 23;
    parkinsense_ble_mtu_exchanged = false;
    parkinsense_ble_notify_subscribed = false;
    parkinsense_ble_last_wait_log_ms = 0;
#endif
}

void parkinsense_ble_log_wait_state_if_stuck(void) {
#ifndef NATIVE_64BIT
    if (parkinsense_ble_ready_to_send())
        return;
    parkinsense_ble_log_wait_state();
#endif
}

void parkinsense_ble_on_mtu_changed(uint16_t mtu) {
#ifndef NATIVE_64BIT
    if (mtu > 0) {
        parkinsense_ble_cached_mtu = mtu;
        parkinsense_ble_mtu_exchanged = true;
        log_i("[PWR_BLE] parkinsense_ble: MTU=%u chunk_delay=%ums send_interval=%ums",
            (unsigned)mtu,
            (unsigned)parkinsense_ble_chunk_delay_ms(mtu),
            (unsigned)parkinsense_ble_send_interval_ms());
    }
#endif
}

bool parkinsense_ble_ready_to_send(void) {
#ifndef NATIVE_64BIT
    NimBLEServer *pServer = blectl_get_ble_server();
    if (!pServer || pServer->getConnectedCount() == 0)
        return false;
    if (!parkinsense_ble_has_notify_subscriber())
        return false;
    if (parkinsense_ble_mtu_exchanged)
        return true;
    if (parkinsense_ble_connect_ms == 0)
        return false;
    return (parkinsense_ble_now_ms() - parkinsense_ble_connect_ms) >= PARKINSENSE_BLE_POST_SUBSCRIBE_MS;
#else
    return false;
#endif
}

uint16_t parkinsense_ble_peer_mtu(void) {
#ifndef NATIVE_64BIT
    NimBLEServer *pServer = blectl_get_ble_server();
    return parkinsense_ble_resolve_peer_mtu(pServer);
#else
    return 23;
#endif
}

uint32_t parkinsense_ble_send_interval_ms(void) {
#ifndef NATIVE_64BIT
    const uint16_t mtu = parkinsense_ble_cached_mtu;
    if (mtu >= 200)
        return 1000;
    if (mtu >= 64)
        return 2000;
    return 3000;
#else
    return 1000;
#endif
}

void parkinsense_ble_setup(void) {
#ifndef NATIVE_64BIT
    NimBLEServer *pServer = blectl_get_ble_server();
    NimBLEAdvertising *pAdvertising = blectl_get_ble_advertising();

    NimBLEService *pService = pServer->createService(NimBLEUUID(PARKINSENSE_SERVICE_UUID));

#ifdef PARKINSENSE_BLE_UNSECURED
    pParkinsenseTXCharacteristic = pService->createCharacteristic(
        NimBLEUUID(PARKINSENSE_CHARACTERISTIC_UUID_TX),
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    pParkinsenseTXCharacteristic->setCallbacks(new ParkinsenseTXCallbacks());
    pParkinsenseTXCharacteristic->setValue((uint8_t)0);
    pService->start();
    pAdvertising->addServiceUUID(pService->getUUID());
    log_i("[PWR_BLE] parkinsense_ble_setup: unsecured service (no pairing required)");
#else
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
    pParkinsenseTXCharacteristic->setCallbacks(new ParkinsenseTXCallbacks());
    pParkinsenseTXCharacteristic->setValue((uint8_t)0);

    pService->start();
    pAdvertising->addServiceUUID(pService->getUUID());
    log_i("[PWR_BLE] parkinsense_ble_setup: encrypted service registered");
#endif
#endif
}


bool parkinsense_ble_send_binary(const uint8_t *data, size_t len) {
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
    if (pParkinsenseTXCharacteristic->getSubscribedCount() == 0) {
        parkinsense_ble_fail_reason = 5;
        return false;
    }
    if (!parkinsense_ble_ready_to_send()) {
        parkinsense_ble_fail_reason = 6;
        return false;
    }

    const uint16_t peer_mtu = parkinsense_ble_resolve_peer_mtu(pServer);
    const uint32_t chunk_delay = parkinsense_ble_chunk_delay_ms(peer_mtu);
    size_t chunk_size = PARKINSENSE_BLE_MIN_CHUNK;
    std::vector<uint16_t> peers = pServer->getPeerDevices();
    if (!peers.empty()) {
        uint16_t mtu = pServer->getPeerMTU(peers[0]);
        if (mtu > 3)
            chunk_size = mtu - 3;
        if (chunk_size < PARKINSENSE_BLE_MIN_CHUNK)
            chunk_size = PARKINSENSE_BLE_MIN_CHUNK;
    }

    log_i("ble_send_binary: len=%u mtu=%u mtu_chunk=%u delay=%ums",
        (unsigned)len, (unsigned)peer_mtu, (unsigned)chunk_size, (unsigned)chunk_delay);

    size_t pos = 0;
    while (pos < len) {
        if (pServer->getConnectedCount() == 0) {
            parkinsense_ble_fail_reason = 4;
            return false;
        }
        size_t chunk_len = len - pos;
        if (chunk_len > chunk_size)
            chunk_len = chunk_size;
        pParkinsenseTXCharacteristic->setValue(data + pos, chunk_len);
        pParkinsenseTXCharacteristic->notify();
        pos += chunk_len;
        if (pos < len)
            vTaskDelay(pdMS_TO_TICKS(chunk_delay));
    }

    const uint8_t newline = '\n';
    pParkinsenseTXCharacteristic->setValue(&newline, 1);
    pParkinsenseTXCharacteristic->notify();
    vTaskDelay(pdMS_TO_TICKS(chunk_delay));

    parkinsense_ble_fail_reason = 0;
    return true;
#else
    (void)data;
    (void)len;
    return true;
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

    uint16_t subCount = pParkinsenseTXCharacteristic->getSubscribedCount();
    if (!parkinsense_ble_has_notify_subscriber()) {
        parkinsense_ble_fail_reason = 5;
        log_i("ble_send: len=%u subs=%u notify_cb=%d (waiting for notify subscribe)",
            (unsigned)len, (unsigned)subCount, (int)parkinsense_ble_notify_subscribed);
        return false;
    }

    if (!parkinsense_ble_ready_to_send()) {
        parkinsense_ble_fail_reason = 6;
        return false;
    }

    const uint16_t peer_mtu = parkinsense_ble_resolve_peer_mtu(pServer);
    const uint32_t chunk_delay = parkinsense_ble_chunk_delay_ms(peer_mtu);
    size_t chunk_size = PARKINSENSE_BLE_MIN_CHUNK;
    std::vector<uint16_t> peers = pServer->getPeerDevices();
    if (!peers.empty()) {
        uint16_t mtu = pServer->getPeerMTU(peers[0]);
        if (mtu > 3)
            chunk_size = mtu - 3;
        if (chunk_size < PARKINSENSE_BLE_MIN_CHUNK)
            chunk_size = PARKINSENSE_BLE_MIN_CHUNK;
    }

#ifdef PARKINSENSE_BLE_UNSECURED
    log_i("ble_send: len=%u mtu=%u mtu_chunk=%u delay=%ums subs=%u peers=%u",
        (unsigned)len, (unsigned)peer_mtu, (unsigned)chunk_size, (unsigned)chunk_delay,
        (unsigned)subCount, (unsigned)peers.size());
#else
    log_i("ble_send: len=%u mtu=%u mtu_chunk=%u delay=%ums subs=%u peers=%u",
        (unsigned)len, (unsigned)peer_mtu, (unsigned)chunk_size, (unsigned)chunk_delay, (unsigned)subCount, (unsigned)peers.size());
#endif

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
            vTaskDelay(pdMS_TO_TICKS(chunk_delay));
    }

    const uint8_t newline = '\n';
    pParkinsenseTXCharacteristic->setValue(&newline, 1);
    pParkinsenseTXCharacteristic->notify();
    vTaskDelay(pdMS_TO_TICKS(chunk_delay));

    parkinsense_ble_fail_reason = 0;
    return true;
#else
    log_i("parkinsense_ble_send (native stub): %.*s", (int)len, data);
    return true;
#endif
}
