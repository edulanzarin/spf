#ifndef UEFI_DEFS_H
#define UEFI_DEFS_H

#include <efi.h>

// SMBIOS table GUIDs
#define SMBIOS_TABLE_GUID                                  \
    {                                                      \
        0xeb9d2d31, 0x2d88, 0x11d3,                        \
        {                                                  \
            0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4f \
        }                                                  \
    }

#define SMBIOS3_TABLE_GUID                                 \
    {                                                      \
        0xf2fd1544, 0x9794, 0x4a2c,                        \
        {                                                  \
            0x99, 0x2e, 0xe5, 0xbb, 0xcf, 0x20, 0xe3, 0x94 \
        }                                                  \
    }

// SMBIOS entry point structures
typedef struct {
    UINT8 anchor[4]; // "_SM_"
    UINT8 checksum;
    UINT8 length;
    UINT8 major_ver;
    UINT8 minor_ver;
    UINT16 max_struct_sz;
    UINT8 entry_rev;
    UINT8 formatted[5];
    UINT8 intermediate_anchor[5]; // "_DMI_"
    UINT8 intermediate_checksum;
    UINT16 table_len;
    UINT32 table_addr;
    UINT16 num_structs;
    UINT8 bcd_rev;
} __attribute__((packed)) smbios_entry_t;

typedef struct {
    UINT8 anchor[5]; // "_SM3_"
    UINT8 checksum;
    UINT8 length;
    UINT8 major_ver;
    UINT8 minor_ver;
    UINT8 docrev;
    UINT8 entry_rev;
    UINT8 reserved;
    UINT32 table_max_sz;
    UINT64 table_addr;
} __attribute__((packed)) smbios3_entry_t;

#endif
