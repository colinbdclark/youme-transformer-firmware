#!/bin/sh
set -e

# Find the tty device for our board by USB VID (0xCAFE = 51966).
# FIXME: This currently only works on macOS.
find_board_device() {
    ioreg -r -c IOUSBHostDevice -l 2>/dev/null | python3 -c "
import sys, re
text = sys.stdin.read()
m = re.search(r'idVendor.*?51966.*?\"IODialinDevice\"\s*=\s*\"(/dev/[^\"]+)\"', text, re.DOTALL)
if m:
    print(m.group(1))
" 2>/dev/null
}

echo "==> Compiling..."
./compile.sh

echo ""
echo "Flashing..."
./flash-firmware.sh build/youme-transformer.elf

echo ""
echo "Waiting for device to enumerate..."
DEVICE=""
for i in $(seq 1 30); do
    sleep 1
    DEVICE=$(find_board_device)
    if [ -n "$DEVICE" ]; then
        break
    fi
done

if [ -z "$DEVICE" ]; then
    echo "Error: No serial device found. Is the board connected via USB?"
    exit 1
fi

echo "Connected to $DEVICE (Ctrl-T Q to exit)"
echo ""
tio "$DEVICE"
