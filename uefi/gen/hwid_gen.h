#ifndef HWID_GEN_H
#define HWID_GEN_H

#include "../../include/common.h"
#include <efi.h>
#include <efilib.h>

void gen_mac_address(UINT8 *mac, const UINT8 oui[3]);
void gen_uuid(UINT8 uuid[16]);
void gen_serial(CHAR8 *buf, UINT8 len);
void gen_disk_serial(CHAR8 *buf, const CHAR8 *vendor);
void gen_asset_tag(CHAR8 *buf, UINT8 len);
void hwid_gen_init(EFI_SYSTEM_TABLE *st);

#endif
