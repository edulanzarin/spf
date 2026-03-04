#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

EFI_FILE="$PROJECT_ROOT/uefi/shadowbyte_spoofer.efi"
ESP_DIR="$PROJECT_ROOT/esp"
OVMF_CODE="/usr/share/OVMF/OVMF_CODE_4M.fd"
OVMF_VARS="/usr/share/OVMF/OVMF_VARS_4M.fd"
OVMF_VARS_COPY="$PROJECT_ROOT/OVMF_VARS.fd"

if [ ! -f "$EFI_FILE" ]; then
    echo "❌ Error: $EFI_FILE not found. Run 'make' first."
    exit 1
fi

if [ ! -f "$OVMF_CODE" ]; then
    echo "❌ Error: OVMF not found. Install with:"
    echo "   sudo apt install ovmf qemu-system-x86"
    exit 1
fi

echo "🔧 Creating ESP (EFI System Partition)..."
rm -rf "$ESP_DIR"
mkdir -p "$ESP_DIR/EFI/BOOT"

# Copia o executável UEFI
cp "$EFI_FILE" "$ESP_DIR/EFI/BOOT/BOOTX64.EFI"

# Cria o script de inicialização automática (startup.nsh) para o UEFI Shell
echo "@echo -off" > "$ESP_DIR/startup.nsh"
echo "FS0:" >> "$ESP_DIR/startup.nsh"
echo "\EFI\BOOT\BOOTX64.EFI" >> "$ESP_DIR/startup.nsh"

# Copia VARS pra ser gravável
cp "$OVMF_VARS" "$OVMF_VARS_COPY"

echo "🚀 Launching QEMU with OVMF..."
echo ""
echo "Expected output:"
echo "  [ShadowByte] UEFI Spoofer v1.0"
echo "  [ShadowByte] HWID generator initialized"
echo "  [ShadowByte] SMBIOS table found at 0x..."
echo ""

qemu-system-x86_64 \
    -machine q35 \
    -drive if=pflash,format=raw,readonly=on,file="$OVMF_CODE" \
    -drive if=pflash,format=raw,file="$OVMF_VARS_COPY" \
    -drive format=raw,file=fat:rw:"$ESP_DIR" \
    -net none \
    -nographic \
    -m 512M

echo ""
echo "✅ QEMU test complete"
