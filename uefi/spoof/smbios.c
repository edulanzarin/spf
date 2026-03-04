#include "smbios.h"
#include "../../include/hw_defs.h"
#include "../../include/smbios_defs.h"
#include "../../include/uefi_defs.h"
#include "../../utils/smbios_helpers.h"
#include "../gen/hwid_gen.h"
#include <efi.h>
#include <efilib.h>

extern EFI_SYSTEM_TABLE *g_st;
extern EFI_BOOT_SERVICES *g_bs;
extern EFI_RUNTIME_SERVICES *g_rt;

/**
 * read original SMBIOS table from memory
 */
EFI_STATUS smbios_read_original(smbios_table_t *table, VOID *smbios_addr, UINTN size)
{
    if (!table || !smbios_addr)
        return EFI_INVALID_PARAMETER;

    Print(L"[SMBIOS] Reading original table...\n");

    smbios_ep_t *ep = (smbios_ep_t *)smbios_addr;

    if (CompareMem(ep->anchor, "_SM_", 4) != 0) {
        smbios3_entry_t *ep3 = (smbios3_entry_t *)smbios_addr;
        if (CompareMem(ep3->anchor, "_SM3_", 5) != 0) {
            Print(L"[ERROR] Invalid SMBIOS signature\n");
            return EFI_INVALID_PARAMETER;
        }
        UINT8 *table_data = (UINT8 *)(UINTN)ep3->table_addr;
        if (!smbios_deserialize(table, table_data, ep3->table_max_sz)) {
            Print(L"[ERROR] Failed to deserialize SMBIOS 3.0 table\n");
            return EFI_DEVICE_ERROR;
        }
    } else {
        UINT8 *table_data = (UINT8 *)(UINTN)ep->table_addr;
        if (!smbios_deserialize(table, table_data, ep->table_len)) {
            Print(L"[ERROR] Failed to deserialize SMBIOS 2.x table\n");
            return EFI_DEVICE_ERROR;
        }
    }

    Print(L"[SMBIOS] Read %u entries from original table\n", table->count);
    return EFI_SUCCESS;
}

/**
 * build complete fake SMBIOS table
 */
EFI_STATUS smbios_build_fake(smbios_table_t *fake, const hw_profile_t *profile)
{
    if (!fake || !profile)
        return EFI_INVALID_PARAMETER;

    Print(L"[SMBIOS] Building fake table for profile: %a\n", profile->name);

    smbios_table_init(fake);

    // type 0 - BIOS
    {
        smbios_type0_t bios = {0};
        bios.header.type = SMBIOS_TYPE_BIOS;
        bios.header.length = sizeof(smbios_type0_t);
        bios.vendor_str = 1;
        bios.version_str = 2;
        bios.starting_segment = 0xE800;
        bios.release_date_str = 3;
        bios.rom_size = 0x80;
        bios.characteristics = 0x0B0DE98000;
        bios.bios_major_release = 5;
        bios.bios_minor_release = 17;

        smbios_add_entry(fake, SMBIOS_TYPE_BIOS, &bios, sizeof(bios) - 4);

        const CHAR8 *strings[] = {(CHAR8 *)BIOS_VENDOR_AMI, (CHAR8 *)"1220", (CHAR8 *)"07/14/2023"};
        smbios_table_entry_t *entry = smbios_find_entry(fake, SMBIOS_TYPE_BIOS, 0);
        smbios_add_strings(entry, (const char **)strings, 3);

        Print(L"[SMBIOS] Added Type 0 (BIOS)\n");
    }

    // type 1 - system
    {
        smbios_type1_t sys = {0};
        sys.header.type = SMBIOS_TYPE_SYSTEM;
        sys.header.length = sizeof(smbios_type1_t);
        sys.manufacturer_str = 1;
        sys.product_str = 2;
        sys.version_str = 3;
        sys.serial_str = 4;
        sys.wakeup_type = 0x06;
        sys.sku_str = 5;
        sys.family_str = 6;

        gen_uuid(sys.uuid);

        smbios_add_entry(fake, SMBIOS_TYPE_SYSTEM, &sys, sizeof(sys) - 4);

        CHAR8 serial[32];
        gen_serial(serial, 20);

        const CHAR8 *strings[] = {(CHAR8 *)MOBO_ASUS,
                                  (CHAR8 *)"System Product Name",
                                  (CHAR8 *)"System Version",
                                  serial,
                                  (CHAR8 *)"SKU",
                                  (CHAR8 *)"Desktop"};
        smbios_table_entry_t *entry = smbios_find_entry(fake, SMBIOS_TYPE_SYSTEM, 0);
        smbios_add_strings(entry, (const char **)strings, 6);

        Print(L"[SMBIOS] Added Type 1 (System) - UUID: %02X%02X%02X%02X-...\n", sys.uuid[0],
              sys.uuid[1], sys.uuid[2], sys.uuid[3]);
    }

    // type 2 - baseboard
    {
        smbios_type2_t board = {0};
        board.header.type = SMBIOS_TYPE_BASEBOARD;
        board.header.length = sizeof(smbios_type2_t);
        board.manufacturer_str = 1;
        board.product_str = 2;
        board.version_str = 3;
        board.serial_str = 4;
        board.asset_tag_str = 5;
        board.feature_flags = 0x09;
        board.location_str = 6;
        board.chassis_handle = 0x0003;
        board.board_type = 0x0A;

        smbios_add_entry(fake, SMBIOS_TYPE_BASEBOARD, &board, sizeof(board) - 4);

        CHAR8 serial[32];
        gen_serial(serial, 20);
        CHAR8 asset[32];
        gen_asset_tag(asset, 32);

        const CHAR8 *strings[] = {(CHAR8 *)MOBO_ASUS,
                                  (CHAR8 *)"PRIME B450M-GAMING/BR",
                                  (CHAR8 *)"Rev X.0x",
                                  serial,
                                  asset,
                                  (CHAR8 *)"Default string"};
        smbios_table_entry_t *entry = smbios_find_entry(fake, SMBIOS_TYPE_BASEBOARD, 0);
        smbios_add_strings(entry, (const char **)strings, 6);

        Print(L"[SMBIOS] Added Type 2 (Baseboard)\n");
    }

    // type 3 - chassis
    {
        smbios_type3_t chassis = {0};
        chassis.header.type = SMBIOS_TYPE_CHASSIS;
        chassis.header.length = sizeof(smbios_type3_t);
        chassis.manufacturer_str = 1;
        chassis.type = CHASSIS_TYPE_DESKTOP;
        chassis.version_str = 2;
        chassis.serial_str = 3;
        chassis.asset_tag_str = 4;
        chassis.bootup_state = 0x03;
        chassis.power_supply_state = 0x03;
        chassis.thermal_state = 0x03;
        chassis.security_status = 0x03;

        smbios_add_entry(fake, SMBIOS_TYPE_CHASSIS, &chassis, sizeof(chassis) - 4);

        CHAR8 serial[32];
        gen_serial(serial, 16);
        CHAR8 asset[32];
        gen_asset_tag(asset, 32);

        const CHAR8 *strings[] = {(CHAR8 *)MOBO_ASUS, (CHAR8 *)"Default string", serial, asset};
        smbios_table_entry_t *entry = smbios_find_entry(fake, SMBIOS_TYPE_CHASSIS, 0);
        smbios_add_strings(entry, (const char **)strings, 4);

        Print(L"[SMBIOS] Added Type 3 (Chassis)\n");
    }

    // type 4 - processor
    {
        smbios_type4_t cpu = {0};
        cpu.header.type = SMBIOS_TYPE_PROCESSOR;
        cpu.header.length = sizeof(smbios_type4_t);
        cpu.socket_str = 1;
        cpu.processor_type = PROC_TYPE_CENTRAL_PROCESSOR;
        cpu.processor_family = PROC_FAMILY_AMD_RYZEN_5;
        cpu.processor_manufacturer_str = 2;
        cpu.processor_id = 0x0870F10;
        cpu.processor_version_str = 3;
        cpu.voltage = 0x8C;
        cpu.external_clock = 100;
        cpu.max_speed = 4600;
        cpu.current_speed = 3600;
        cpu.status = 0x41;
        cpu.processor_upgrade = 0x2E;
        cpu.core_count = 6;
        cpu.core_enabled = 6;
        cpu.thread_count = 12;
        cpu.processor_characteristics = 0x04;

        smbios_add_entry(fake, SMBIOS_TYPE_PROCESSOR, &cpu, sizeof(cpu) - 4);

        const CHAR8 *strings[] = {(CHAR8 *)"AM4", (CHAR8 *)CPU_AMD,
                                  (CHAR8 *)"AMD Ryzen 5 5600X 6-Core Processor"};
        smbios_table_entry_t *entry = smbios_find_entry(fake, SMBIOS_TYPE_PROCESSOR, 0);
        smbios_add_strings(entry, (const char **)strings, 3);

        Print(L"[SMBIOS] Added Type 4 (Processor)\n");
    }

    // type 17 - memory
    for (UINT8 slot = 0; slot < 4; slot++) {
        smbios_type17_t ram = {0};
        ram.header.type = SMBIOS_TYPE_MEMORY_DEVICE;
        ram.header.length = sizeof(smbios_type17_t);
        ram.physical_array_handle = 0x0010;
        ram.total_width = 64;
        ram.data_width = 64;
        ram.size = 8192;
        ram.form_factor = MEM_FORM_DIMM;
        ram.device_locator_str = 1;
        ram.bank_locator_str = 2;
        ram.memory_type = MEM_TYPE_DDR4;
        ram.speed = 3200;
        ram.manufacturer_str = 3;
        ram.serial_str = 4;
        ram.part_number_str = 5;
        ram.configured_speed = 3200;

        smbios_add_entry(fake, SMBIOS_TYPE_MEMORY_DEVICE, &ram, sizeof(ram) - 4);

        CHAR8 loc[16];
        AsciiSPrint(loc, sizeof(loc), "DIMM %d", slot);
        CHAR8 bank[16];
        AsciiSPrint(bank, sizeof(bank), "BANK %d", slot);
        CHAR8 serial[16];
        gen_serial(serial, 12);

        const CHAR8 *strings[] = {loc, bank, (CHAR8 *)"Kingston", serial,
                                  (CHAR8 *)"KF3200C16D4/8GX"};
        smbios_table_entry_t *entry = smbios_find_entry(fake, SMBIOS_TYPE_MEMORY_DEVICE, slot);
        smbios_add_strings(entry, (const char **)strings, 5);
    }
    Print(L"[SMBIOS] Added 4x Type 17 (Memory)\n");

    Print(L"[SMBIOS] Fake table built with %u entries\n", fake->count);
    return EFI_SUCCESS;
}

static UINT8 calc_smbios_checksum(UINT8 *data, UINTN length)
{
    UINT8 sum = 0;
    for (UINTN i = 0; i < length; i++)
        sum += data[i];
    return (UINT8)(0x100 - sum);
}

EFI_STATUS smbios_inject(const smbios_table_t *fake)
{
    if (!fake || fake->count == 0 || fake->count > 64)
        return EFI_INVALID_PARAMETER;

    Print(L"[SMBIOS] Injecting fake table into memory...\n");
    Print(L"[SMBIOS] Table has %u entries\n", fake->count);

    EFI_STATUS status;
    EFI_GUID smbios3_guid = SMBIOS3_TABLE_GUID;
    EFI_GUID smbios_guid = SMBIOS_TABLE_GUID;

    UINT8 *table_buffer;
    UINTN buf_size = 32768;
    status = g_bs->AllocatePool(EfiRuntimeServicesData, buf_size, (VOID **)&table_buffer);
    if (EFI_ERROR(status)) {
        Print(L"[ERROR] Failed to allocate table buffer: %r\n", status);
        return status;
    }

    UINT32 table_size = smbios_serialize(fake, table_buffer, buf_size);
    if (table_size == 0) {
        Print(L"[ERROR] Failed to serialize table\n");
        g_bs->FreePool(table_buffer);
        return EFI_DEVICE_ERROR;
    }
    Print(L"[SMBIOS] Serialized %u bytes\n", table_size);

    smbios3_entry_t *entry3 = NULL;
    for (UINTN i = 0; i < g_st->NumberOfTableEntries; i++) {
        if (CompareGuid(&g_st->ConfigurationTable[i].VendorGuid, &smbios3_guid) == 0) {
            entry3 = (smbios3_entry_t *)g_st->ConfigurationTable[i].VendorTable;
            break;
        }
    }

    if (entry3) {
        Print(L"[SMBIOS] Updating SMBIOS 3.0 entry point...\n");

        smbios3_entry_t *new_entry;
        status = g_bs->AllocatePool(EfiRuntimeServicesData, sizeof(smbios3_entry_t),
                                    (VOID **)&new_entry);
        if (EFI_ERROR(status)) {
            g_bs->FreePool(table_buffer);
            return status;
        }

        CopyMem(new_entry, entry3, sizeof(smbios3_entry_t));
        new_entry->table_addr = (UINT64)(UINTN)table_buffer;
        new_entry->table_max_sz = table_size;

        new_entry->checksum = 0;
        new_entry->checksum = calc_smbios_checksum((UINT8 *)new_entry, new_entry->length);

        for (UINTN i = 0; i < g_st->NumberOfTableEntries; i++) {
            if (CompareGuid(&g_st->ConfigurationTable[i].VendorGuid, &smbios3_guid) == 0) {
                g_st->ConfigurationTable[i].VendorTable = new_entry;
                Print(L"[SMBIOS] ConfigurationTable updated (3.0)\n");
                break;
            }
        }
    } else {
        smbios_ep_t *entry2 = NULL;
        for (UINTN i = 0; i < g_st->NumberOfTableEntries; i++) {
            if (CompareGuid(&g_st->ConfigurationTable[i].VendorGuid, &smbios_guid) == 0) {
                entry2 = (smbios_ep_t *)g_st->ConfigurationTable[i].VendorTable;
                break;
            }
        }

        if (!entry2) {
            Print(L"[ERROR] No SMBIOS entry point found\n");
            g_bs->FreePool(table_buffer);
            return EFI_NOT_FOUND;
        }

        Print(L"[SMBIOS] Updating SMBIOS 2.x entry point...\n");

        smbios_ep_t *new_entry;
        status =
            g_bs->AllocatePool(EfiRuntimeServicesData, sizeof(smbios_ep_t), (VOID **)&new_entry);
        if (EFI_ERROR(status)) {
            g_bs->FreePool(table_buffer);
            return status;
        }

        CopyMem(new_entry, entry2, sizeof(smbios_ep_t));
        new_entry->table_addr = (UINT32)(UINTN)table_buffer;
        new_entry->table_len = (UINT16)table_size;
        new_entry->num_structs = fake->count;

        new_entry->checksum = 0;
        new_entry->checksum = calc_smbios_checksum((UINT8 *)new_entry, new_entry->length);

        new_entry->intermediate_checksum = 0;
        new_entry->intermediate_checksum =
            calc_smbios_checksum((UINT8 *)&new_entry->intermediate_anchor[0], 15);

        for (UINTN i = 0; i < g_st->NumberOfTableEntries; i++) {
            if (CompareGuid(&g_st->ConfigurationTable[i].VendorGuid, &smbios_guid) == 0) {
                g_st->ConfigurationTable[i].VendorTable = new_entry;
                Print(L"[SMBIOS] ConfigurationTable updated (2.x)\n");
                break;
            }
        }
    }

    Print(L"[SMBIOS] Injection complete!\n");
    Print(L"[SMBIOS] New table addr: 0x%lx\n", (UINT64)(UINTN)table_buffer);

    return EFI_SUCCESS;
}

EFI_STATUS smbios_spoof_full(const hw_profile_t *profile)
{
    EFI_STATUS status;

    if (!profile)
        return EFI_INVALID_PARAMETER;

    smbios_table_t fake;
    status = smbios_build_fake(&fake, profile);
    if (EFI_ERROR(status))
        return status;

    status = smbios_inject(&fake);
    if (EFI_ERROR(status))
        return status;

    return EFI_SUCCESS;
}
