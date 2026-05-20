# Parkinsense BLE pairing (react-native-ble-plx)

The watch exposes an encrypted GATT service. The phone must pair before reads,
subscribes, or data notifies succeed.

## UUIDs

| Role | UUID |
|------|------|
| Service | `4a6c5e01-7b8f-4a9d-b2c3-d4e5f6a7b8c9` |
| Pair probe (read first) | `4a6c5e03-7b8f-4a9d-b2c3-d4e5f6a7b8c9` |
| Data TX (notify) | `4a6c5e02-7b8f-4a9d-b2c3-d4e5f6a7b8c9` |

Both characteristics require an **encrypted** link (`READ_ENC`).

## Recommended app flow

1. Scan for the service UUID or a known bonded device name.
2. `manager.connectToDevice(deviceId)` — link starts unencrypted.
3. **Pairing probe:** `readCharacteristicForDevice(deviceId, serviceUuid, pairProbeUuid)`.
   - First attempt fails with insufficient authentication/encryption.
   - Android/iOS shows the system pairing UI; the watch may log/display a 6-digit PIN.
4. Retry the same read after pairing completes (value is `0x01`).
5. Subscribe to the data characteristic:
   `monitorCharacteristicForDevice(deviceId, serviceUuid, txUuid)`.
6. Receive hourly JSON blocks as notifications (messages end with `\n`).

## Notes

- Pair once; bond keys are stored on both sides. Hourly reconnects should not
  prompt again unless bond is cleared.
- Do not implement custom pairing UI in JS — let the OS handle ECDH and
  AES-CCM link encryption after the protected GATT access fails.
- Background scan/connect behavior is OS-dependent; hourly sync still requires
  the app to reconnect when the watch advertises.

## Example (sketch)

```javascript
const SERVICE = '4a6c5e01-7b8f-4a9d-b2c3-d4e5f6a7b8c9';
const PAIR_PROBE = '4a6c5e03-7b8f-4a9d-b2c3-d4e5f6a7b8c9';
const DATA_TX = '4a6c5e02-7b8f-4a9d-b2c3-d4e5f6a7b8c9';

async function connectAndPair(manager, deviceId) {
  const device = await manager.connectToDevice(deviceId, { autoConnect: false });
  await device.discoverAllServicesAndCharacteristics();

  const readProbe = () =>
    device.readCharacteristicForService(SERVICE, PAIR_PROBE);

  try {
    await readProbe();
  } catch (e) {
    // OS pairing in progress — retry after user confirms
    await new Promise((r) => setTimeout(r, 2000));
    await readProbe();
  }

  device.monitorCharacteristicForService(SERVICE, DATA_TX, (err, char) => {
    if (char?.value) {
      // base64 payload — reassemble chunks until '\n'
    }
  });

  return device;
}
```
