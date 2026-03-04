# shadowbyte UEFI Spoofer Makefile

ARCH            = x86_64
TARGET          = shadowbyte_spoofer.efi

# Paths
GNU_EFI_DIR     = /usr
EFI_INCLUDE     = $(GNU_EFI_DIR)/include/efi
EFI_INCLUDE_ARCH = $(EFI_INCLUDE)/$(ARCH)
EFI_LIB         = $(GNU_EFI_DIR)/lib

# Compiler & Linker
CC              = gcc
LD              = ld
OBJCOPY         = objcopy

# Flags
CFLAGS          = -I$(EFI_INCLUDE) \
                  -I$(EFI_INCLUDE_ARCH) \
                  -I../include \
                  -fno-stack-protector \
                  -fpic \
                  -fshort-wchar \
                  -mno-red-zone \
                  -Wall \
                  -DEFI_FUNCTION_WRAPPER \
                  -DGNU_EFI_USE_MS_ABI

LDFLAGS         = -nostdlib \
                  -znocombreloc \
                  -T $(EFI_LIB)/elf_$(ARCH)_efi.lds \
                  -shared \
                  -Bsymbolic \
                  -L $(EFI_LIB) \
                  $(EFI_LIB)/crt0-efi-$(ARCH).o

LIBS            = -lefi -lgnuefi

# Source files
MAIN_SRC        = main.c
GEN_SRC         = gen/hwid_gen.c
SPOOF_SRC       = spoof/smbios.c
UTILS_SRC       = ../utils/smbios_helpers.c

SOURCES         = $(MAIN_SRC) $(GEN_SRC) $(SPOOF_SRC) $(UTILS_SRC)
OBJECTS         = $(SOURCES:.c=.o)

# Build targets
.PHONY: all clean test

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

shadowbyte_spoofer.so: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

$(TARGET): shadowbyte_spoofer.so
	$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic \
	           -j .dynsym -j .rel -j .rela -j .reloc \
	           --target=efi-app-$(ARCH) $< $@
	@echo ""
	@echo "✅ Build complete: $(TARGET)"
	@echo ""

clean:
	rm -f $(OBJECTS) shadowbyte_spoofer.so $(TARGET)
	@echo "🧹 Cleaned build artifacts"

test: $(TARGET)
	@echo "🚀 Testing in QEMU..."
	@./test_qemu.sh
