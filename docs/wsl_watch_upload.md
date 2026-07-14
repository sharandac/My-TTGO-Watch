# Upload firmware to the watch from WSL2

This guide covers connecting a LilyGO T-Watch (ESP32) over USB from **Windows → WSL2**, then **building** and **uploading** this project with PlatformIO.

Default PlatformIO environment for this repo: **`t-watch2020-v3`** (see `platformio.ini`).

---

## What you need

- Windows 10/11 with **WSL2** installed
- Project cloned inside WSL (e.g. `~/projects/parkinsense_lilygo`)
- **PlatformIO** available in WSL (`pio` on your PATH)
- USB data cable (not charge-only)
- Watch powered on

---

## Part 1 — One-time setup

### 1.1 Install USB forwarding on Windows

Open **PowerShell as Administrator** and run:

```powershell
winget install dorssel.usbipd-win
```

Close and reopen PowerShell after installation.

### 1.2 Allow serial access in WSL

In a **WSL** terminal:

```bash
sudo apt update
sudo apt install -y linux-tools-generic hwdata
```

Add your user to the `dialout` group (needed to open `/dev/ttyACM*` without root):

```bash
sudo usermod -aG dialout $USER
```

**Log out of WSL and open a new terminal** (or run `wsl --shutdown` from Windows PowerShell, then reopen WSL) so the group change applies.

### 1.3 Confirm PlatformIO works in WSL

```bash
cd ~/projects/parkinsense_lilygo
pio --version
```

If `pio` is not found, install PlatformIO in WSL or use the PlatformIO Core path from your setup.

---

## Part 2 — Connect the watch (every upload session)

### 2.1 Plug in the watch

Connect the watch to the PC with USB. Turn it on (or use bootloader mode if upload fails — see [Troubleshooting](#troubleshooting)).

### 2.2 List USB devices on Windows

**PowerShell as Administrator**:

```powershell
usbipd list
```

Find your watch or serial adapter. Typical descriptions:

- `USB Serial`, `CP210`, `CH340`, `Silicon Labs`
- `USB JTAG/serial` (ESP32 USB-CDC)

Note the **BUSID** (example: `2-3`).

If the device is **Not shared**, bind it once (replace `2-3` with your BUSID):

```powershell
usbipd bind --busid 2-3
```

### 2.3 Attach the device to WSL

Still in **admin PowerShell**:

```powershell
usbipd attach --wsl --busid 2-3
```

Use your actual BUSID. You should see a success message.

### 2.4 Find the serial port in WSL

In **WSL**:

```bash
ls -l /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
```

| Port | Typical hardware |
|------|------------------|
| `/dev/ttyACM0` | ESP32 USB-CDC (common on T-Watch) |
| `/dev/ttyUSB0` | Separate USB‑serial chip (CP210/CH340) |

Remember the port you will use below (examples use `/dev/ttyACM0`).

Optional — confirm the kernel saw the device:

```bash
dmesg | tail -20
```

---

## Part 3 — Build and upload

All commands run from the **project root** in WSL:

```bash
cd ~/projects/parkinsense_lilygo
```

Adjust the path if your clone lives elsewhere.

### 3.1 Build only

Uses the default environment `t-watch2020-v3`:

```bash
pio run
```

Explicit environment:

```bash
pio run -e t-watch2020-v3
```

### 3.2 Upload only

Upload assumes the firmware is already built. Replace the port if yours differs:

```bash
pio run -t upload --upload-port /dev/ttyACM0
```

With explicit environment:

```bash
pio run -e t-watch2020-v3 -t upload --upload-port /dev/ttyACM0
```

### 3.3 Build and upload in one step

```bash
pio run -t upload --upload-port /dev/ttyACM0
```

Or:

```bash
pio run -e t-watch2020-v3 -t upload --upload-port /dev/ttyACM0
```

### 3.4 Serial monitor (optional)

After upload, view logs:

```bash
pio device monitor --port /dev/ttyACM0 -b 115200
```

Press `Ctrl+C` to exit.

---

## Part 4 — Release USB back to Windows

When you are done, detach so Windows can use the port again.

**PowerShell as Administrator**:

```powershell
usbipd detach --busid 2-3
```

Use the same BUSID as in step 2.3.

---

## Quick reference

| Step | Where | Command |
|------|--------|---------|
| List USB devices | Windows (Admin) | `usbipd list` |
| Bind (once if needed) | Windows (Admin) | `usbipd bind --busid 2-3` |
| Attach to WSL | Windows (Admin) | `usbipd attach --wsl --busid 2-3` |
| Find port | WSL | `ls -l /dev/ttyACM* /dev/ttyUSB*` |
| Build | WSL | `pio run -e t-watch2020-v3` |
| Upload | WSL | `pio run -e t-watch2020-v3 -t upload --upload-port /dev/ttyACM0` |
| Monitor | WSL | `pio device monitor --port /dev/ttyACM0 -b 115200` |
| Detach | Windows (Admin) | `usbipd detach --busid 2-3` |

---

## Other environments

If you use a different watch variant, set the environment explicitly:

```bash
pio run -e t-watch2020-v1 -t upload --upload-port /dev/ttyACM0
pio run -e t-watch2020-v2 -t upload --upload-port /dev/ttyACM0
```

See `platformio.ini` for all `env:` names.

---

## Troubleshooting

### Permission denied on `/dev/ttyACM0`

```bash
sudo usermod -aG dialout $USER
```

Log out of WSL and open a new terminal, then retry.

### No `/dev/ttyACM*` or `/dev/ttyUSB*` in WSL

1. Confirm attach succeeded: `usbipd list` on Windows (device should show as attached to WSL).
2. Unplug/replug the cable.
3. Run attach again: `usbipd attach --wsl --busid 2-3`
4. Check `dmesg | tail -20` in WSL.

### Upload fails / “Failed to connect”

1. Put the ESP32 in download mode: hold **BOOT**, tap **RESET**, release **BOOT**, then run upload again immediately.
2. Try a slower upload speed in `platformio.ini` under your env, e.g. `upload_speed = 115200`, then rebuild and upload.
3. Try the other port name (`ttyUSB0` vs `ttyACM0`).

### `usbipd attach` says already attached

```powershell
usbipd detach --busid 2-3
usbipd attach --wsl --busid 2-3
```

### Easier alternative: upload from Windows

If USB in WSL is unreliable:

1. Open this repo in VS Code on **Windows** (not “WSL Remote”).
2. Install PlatformIO on Windows.
3. Connect the watch — it appears as `COM3`, `COM4`, etc.
4. Run **Upload** in PlatformIO or:

   ```bash
   pio run -e t-watch2020-v3 -t upload
   ```

---

## Related docs

- [Parkinsense BLE pairing (phone app)](parkinsense_ble_pairing.md)
- Project build script: `build_check.sh` (build + native tests)
