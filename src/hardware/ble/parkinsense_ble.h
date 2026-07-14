#ifndef _PARKINSENSE_BLE_H
    #define _PARKINSENSE_BLE_H

    #include <stddef.h>
    #include <stdbool.h>

    #define PARKINSENSE_SERVICE_UUID              "4a6c5e01-7b8f-4a9d-b2c3-d4e5f6a7b8c9"
    #define PARKINSENSE_CHARACTERISTIC_UUID_TX    "4a6c5e02-7b8f-4a9d-b2c3-d4e5f6a7b8c9"
    /** Read-only probe (secured builds only); READ_ENC triggers OS pairing. */
    #define PARKINSENSE_CHARACTERISTIC_UUID_PAIR  "4a6c5e03-7b8f-4a9d-b2c3-d4e5f6a7b8c9"

    /** True unless firmware built with PARKINSENSE_BLE_UNSECURED. */
    bool parkinsense_ble_pairing_required(void);

    void parkinsense_ble_setup(void);
    /**
     * @brief Send a buffer over the dedicated parkinsense BLE characteristic.
     *        Chunks the data to fit the negotiated MTU and appends '\n' as a
     *        message delimiter so any client can reassemble.
     *
     * @param data  null-terminated string to send
     * @param len   length of data (excluding null terminator)
     * @return true if at least one notify was sent
     */
    bool parkinsense_ble_send(const char *data, size_t len);
    /** Raw bytes (PKSD binary); MTU-only chunking, no JSON split rules. */
    bool parkinsense_ble_send_binary(const uint8_t *data, size_t len);
    /** Last BLE send failure reason: 0=ok, 1=no char, 2=no server, 3=no peers, 4=disconnected mid-send, 5=no notify subscribers, 6=link not ready (MTU/subscribe wait) */
    int parkinsense_ble_last_fail_reason(void);

    /** Link lifecycle hooks (from blectl server callbacks). */
    void parkinsense_ble_on_client_connected(void);
    void parkinsense_ble_on_client_disconnected(void);
    void parkinsense_ble_on_mtu_changed(uint16_t mtu);

    /** True when connected and MTU exchange finished or grace period elapsed. */
    bool parkinsense_ble_ready_to_send(void);
    /** Negotiated ATT MTU for the active peer (23 until exchange). */
    uint16_t parkinsense_ble_peer_mtu(void);
    /** Recommended ms between block sends for the current MTU. */
    uint32_t parkinsense_ble_send_interval_ms(void);
    /** Log why sends are blocked (rate-limited). Call from pipeline when connected. */
    void parkinsense_ble_log_wait_state_if_stuck(void);

#endif // _PARKINSENSE_BLE_H
