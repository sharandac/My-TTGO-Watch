# Force console subsystem on Windows so the linker expects main() not WinMain.
# Skip on Linux/WSL where g++ does not accept -mconsole.
Import("env")
import sys
if sys.platform.startswith("win"):
    env.Prepend(LINKFLAGS=["-mconsole", "-Wl,-subsystem,console"])
