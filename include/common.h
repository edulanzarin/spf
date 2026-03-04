#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdint.h>

// spoof flags (bitmask)
#define SPOOF_SMBIOS (1 << 0)
#define SPOOF_ACPI   (1 << 1)
#define SPOOF_TPM    (1 << 2)
#define SPOOF_MAC    (1 << 3)
#define SPOOF_DISK   (1 << 4)
#define SPOOF_ALL    0xFF

// SMBIOS type numbers
#define SMBIOS_TYPE_BIOS                0
#define SMBIOS_TYPE_SYSTEM              1
#define SMBIOS_TYPE_BASEBOARD           2
#define SMBIOS_TYPE_CHASSIS             3
#define SMBIOS_TYPE_PROCESSOR           4
#define SMBIOS_TYPE_CACHE               7
#define SMBIOS_TYPE_PORT_CONNECTOR      8
#define SMBIOS_TYPE_SYSTEM_SLOTS        9
#define SMBIOS_TYPE_OEM_STRINGS         11
#define SMBIOS_TYPE_BIOS_LANG           13
#define SMBIOS_TYPE_PHYSICAL_MEM_ARRAY  16
#define SMBIOS_TYPE_MEMORY_DEVICE       17
#define SMBIOS_TYPE_MEM_ARRAY_MAPPED    19
#define SMBIOS_TYPE_PORTABLE_BATTERY    22
#define SMBIOS_TYPE_SYSTEM_POWER_SUPPLY 39
#define SMBIOS_TYPE_ONBOARD_DEVICES_EXT 41

// generic SMBIOS entry (raw format)
typedef struct {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
    uint8_t data[512];
} smbios_table_entry_t;

// SMBIOS table (collection of entries)
typedef struct {
    uint16_t count;
    smbios_table_entry_t entries[64];
} smbios_table_t;

// config stored in NVRAM
typedef struct {
    uint32_t magic; // 0x53484442 "SHDB"
    uint8_t flags;
    char profile_name[32];
    uint8_t reserved[64];
} nvram_cfg_t;

// complete hardware profile
typedef struct {
    char name[64];
    smbios_table_t smbios;

    // non-SMBIOS data
    uint8_t mac[6];
    char disk_serial[20];
    char disk_model[40];
    char disk_firmware[8];

    // ACPI tables (future)
    struct {
        uint8_t count;
        struct {
            char signature[4];
            uint32_t length;
            uint8_t data[2048];
        } tables[8];
    } acpi;
} hw_profile_t;

#endif
