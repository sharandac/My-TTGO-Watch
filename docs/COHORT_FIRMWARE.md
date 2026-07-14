# Parkinsense cohort firmware (T-Watch 2020 V3)

Blessed build for field data collection: PKSD binary BLE, PSRAM buffers, auto-start recording.

## Flash

From WSL (see `docs/wsl_watch_upload.md` for USB):

```bash
pio run -e t-watch2020-v3_cohort -t upload -j 1
```

## Boot confirmation

Serial should include:

```
Parkinsense COHORT build label=cohort-v1 binary=1 auto_start=1 psram=1 bma_fifo=1
Parkinsense data pipeline initialized
Parkinsense data pipeline started
```



## Continuous sampling (BMA423 FIFO)

Cohort builds define PARKINSENSE_BMA_FIFO=1:

- Accel runs at **50 Hz** in the BMA423 hardware FIFO while the ESP32 light-sleeps.
- FIFO **watermark ~1 s** (300 bytes) fires INT1 → ESP32 wakes briefly, drains samples into PSRAM.
- Step/tilt/wakeup gestures still share INT1; FIFO drain runs before gesture handling.
- Expect **~1875 PSRAM blocks per hour** of wear time (vs ~300 with legacy CPU polling).

Boot log when pipeline starts:



Pipeline runs without tapping Play on the Parkinsense tile.

## Environments

| Env | Use |
|-----|-----|
| `t-watch2020-v3_cohort` | **Cohort / production data collection** |
| `t-watch2020-v3_test_hourly` | Same cadence + binary; legacy test banner |
| `t-watch2020-v3_dev` | Real-time stream while connected (battery drain) |
| `t-watch2020-v3` | Upstream JSON hourly (not matched to native PKSD app) |

## Tests

```bash
pio test -e test_native
pio test -e test_native_cohort
pio test -e test_t-watch2020-v3_cohort   # on-device, optional
```
