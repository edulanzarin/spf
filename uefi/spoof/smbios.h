#ifndef SMBIOS_SPOOF_H
#define SMBIOS_SPOOF_H

#include "../../include/common.h"
#include <efi.h>

// read original SMBIOS table from memory
EFI_STATUS smbios_read_original(smbios_table_t *table, VOID *smbios_addr, UINTN size);

// build fake SMBIOS table from profile
EFI_STATUS smbios_build_fake(smbios_table_t *fake, const hw_profile_t *profile);

// inject fake table into UEFI memory
EFI_STATUS smbios_inject(const smbios_table_t *fake);

// main spoof function (called from main.c)
EFI_STATUS smbios_spoof_full(const hw_profile_t *profile);

#endif
