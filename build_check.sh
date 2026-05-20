#!/usr/bin/env bash
export PATH="$HOME/.platformio/penv/bin:$HOME/.local/bin:/usr/local/bin:/usr/bin:/bin"
cd /home/capit/projects/parkinsense_lilygo
pio run 2>&1 | tail -n 40
pio test -e test_native
