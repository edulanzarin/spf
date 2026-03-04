#ifndef UEFI_DEFS_H
#define UEFI_DEFS_H

#include <efi.h>

// SMBIOS 3.0 entry point
#ifndef SMBIOS3_ENTRY_POINT_STRUCTURE
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

// SMBIOS 2.x entry point
#ifndef SMBIOS_ENTRY_POINT_STRUCTURE
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
} __attribute__((packed)) smbios_ep_t;
#endif

#endif
