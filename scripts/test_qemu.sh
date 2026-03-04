#!/bin/bash

# Test script for UEFI spoofer in QEMU with OVMF

set -e

EFI_FILE="uefi/Shadowbyte_spoofer.efi"
ESP_DIR="esp"
OVMF_CODE="/usr/share/OVMF/OVMF_CODE.fd"
OVMF_VARS="/usr/share/OVMF/OVMF_VARS.fd"

# check if compiled
if [ ! -f "$EFI_FILE" ]; then
    echo "❌ Error: $EFI_FILE not found. Run 'make' first."
    exit 1
fi

# check if OVMF is installed
if [ ! -f "$OVMF_CODE" ]; then
    echo "❌ Error: OVMF not found. Install with:"
    echo "   sudo apt install ovmf qemu-system-x86"
    exit 1
fi

echo "🔧 Creating ESP (EFI System Partition)..."
rm -rf "$ESP_DIR"
mkdir -p "$ESP_DIR/EFI/BOOT"

# copy our EFI binary
cp "$EFI_FILE" "$ESP_DIR/EFI/BOOT/BOOTX64.EFI"

echo "🚀 Launching QEMU with OVMF..."
echo ""
echo "Expected output:"
echo "  [Shadowbyte] UEFI Spoofer v1.0"
echo "  [Shadowbyte] HWID generator initialized"
echo "  [Shadowbyte] SMBIOS table found at 0x..."
echo ""

# run QEMU with OVMF
qemu-system-x86_64 \
    -drive if=pflash,format=raw,readonly=on,file="$OVMF_CODE" \
    -drive if=pflash,format=raw,file="$OVMF_VARS" \
    -drive format=raw,file=fat:rw:"$ESP_DIR" \
    -net none \
    -nographic \
    -m 512M

echo ""
echo "✅ QEMU test complete"
