# ShadowByte UEFI Spoofer Makefile

ARCH            = x86_64
TARGET          = shadowbyte_spoofer.efi
TARGET_SO       = shadowbyte_spoofer.so

# Paths
GNU_EFI_DIR     = /usr
EFI_INCLUDE     = $(GNU_EFI_DIR)/include/efi
EFI_INCLUDE_ARCH = $(EFI_INCLUDE)/$(ARCH)
EFI_LIB         = $(GNU_EFI_DIR)/lib

# Compiler
CC              = gcc
OBJCOPY         = objcopy

# Source files
MAIN_SRC        = main.c
GEN_SRC         = gen/hwid_gen.c
SPOOF_SRC       = spoof/smbios.c
UTILS_SRC       = ../utils/smbios_helpers.c

SOURCES         = $(MAIN_SRC) $(GEN_SRC) $(SPOOF_SRC) $(UTILS_SRC)

# Flags
CFLAGS          = -I$(EFI_INCLUDE) \
                  -I$(EFI_INCLUDE_ARCH) \
                  -I../include \
                  -fpic \
                  -fshort-wchar \
                  -mno-red-zone \
                  -fno-stack-protector \
                  -shared \
                  -Wl,-T,$(EFI_LIB)/elf_$(ARCH)_efi.lds \
                  -Wl,-Bsymbolic \
                  -Wl,--no-undefined

# Build targets
.PHONY: all clean test

all: $(TARGET)

$(TARGET_SO): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) \
		$(EFI_LIB)/crt0-efi-$(ARCH).o \
		-lefi -lgnuefi

$(TARGET): $(TARGET_SO)
	$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-$(ARCH) $< $@
	@echo ""
	@echo "✅ Build complete: $(TARGET)"
	@echo ""

clean:
	rm -f $(TARGET_SO) $(TARGET)
	@echo "🧹 Cleaned build artifacts"

test: $(TARGET)
	@echo "🚀 Testing in QEMU..."
	@../scripts/test_qemu.sh
