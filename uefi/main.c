#include "../include/common.h"
#include "../include/hw_defs.h"
#include "../include/uefi_defs.h"
#include "gen/hwid_gen.h"
#include "spoof/smbios.h"
#include <efi.h>
#include <efilib.h>

// global state
EFI_SYSTEM_TABLE *g_st = NULL;
EFI_BOOT_SERVICES *g_bs = NULL;
EFI_RUNTIME_SERVICES *g_rt = NULL;
EFI_HANDLE g_image = NULL;

// forward declarations
EFI_STATUS locate_smbios_table(VOID **table_addr, UINTN *table_size);
EFI_STATUS spoof_smbios(VOID);
EFI_STATUS load_profile(hw_profile_t *profile);
EFI_STATUS chainload_bootloader(VOID);

/**
 * UEFI entry point
 */
EFI_STATUS EFIAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
    EFI_STATUS status;

    InitializeLib(image, systab);

    g_st = systab;
    g_bs = systab->BootServices;
    g_rt = systab->RuntimeServices;
    g_image = image;

    Print(L"[Nexus] UEFI Spoofer v1.0\n");

    // init HWID generator
    hwid_gen_init(systab);
    Print(L"[Nexus] HWID generator initialized\n");

    // locate SMBIOS table
    VOID *smbios_addr = NULL;
    UINTN smbios_size = 0;
    status = locate_smbios_table(&smbios_addr, &smbios_size);
    if (EFI_ERROR(status)) {
        Print(L"[ERROR] Failed to locate SMBIOS table: %r\n", status);
        goto cleanup;
    }
    Print(L"[Nexus] SMBIOS table found at 0x%lx (size: %u bytes)\n", smbios_addr, smbios_size);

    // load hardware profile
    hw_profile_t profile = {0};
    status = load_profile(&profile);
    if (EFI_ERROR(status)) {
        Print(L"[ERROR] Failed to load profile: %r\n", status);
        goto cleanup;
    }
    Print(L"[Nexus] Profile loaded: %a\n", profile.name);

    // perform SMBIOS spoofing
    status = spoof_smbios();
    if (EFI_ERROR(status)) {
        Print(L"[ERROR] SMBIOS spoofing failed: %r\n", status);
        goto cleanup;
    }
    Print(L"[Nexus] SMBIOS spoofing complete\n");

    // TODO: ACPI spoofing
    // TODO: TPM spoofing
    // TODO: Disk/Network protocol hooks

    Print(L"[Nexus] All spoofs applied successfully\n");
    Print(L"[Nexus] Chainloading original bootloader...\n");

    // small delay so user can see messages
    g_bs->Stall(2000000); // 2 seconds

    // chainload
    status = chainload_bootloader();
    if (EFI_ERROR(status)) {
        Print(L"[ERROR] Chainload failed: %r\n", status);
        goto cleanup;
    }

cleanup:
    // if we get here, something went wrong
    Print(L"\n[ERROR] Press any key to reboot...\n");
    // wait for key
    UINTN index;
    EFI_INPUT_KEY key;
    g_st->ConIn->Reset(g_st->ConIn, FALSE);
    g_bs->WaitForEvent(1, &g_st->ConIn->WaitForKey, &index);
    g_st->ConIn->ReadKeyStroke(g_st->ConIn, &key);

    // reboot
    g_rt->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);

    return status;
}

/**
 * locate SMBIOS table in UEFI memory
 */
EFI_STATUS locate_smbios_table(VOID **table_addr, UINTN *table_size)
{
    EFI_GUID smbios_guid = SMBIOS_TABLE_GUID;
    EFI_GUID smbios3_guid = SMBIOS3_TABLE_GUID;

    // try SMBIOS 3.0 first (64-bit)
    for (UINTN i = 0; i < g_st->NumberOfTableEntries; i++) {
        if (CompareGuid(&g_st->ConfigurationTable[i].VendorGuid, &smbios3_guid) == 0) {
            *table_addr = g_st->ConfigurationTable[i].VendorTable;
            // SMBIOS 3.0 entry point contains table size
            smbios3_entry_t *entry = (smbios3_entry_t *)*table_addr;
            *table_size = entry->table_max_sz;
            return EFI_SUCCESS;
        }
    }

    // fallback to SMBIOS 2.x (32-bit)
    for (UINTN i = 0; i < g_st->NumberOfTableEntries; i++) {
        if (CompareGuid(&g_st->ConfigurationTable[i].VendorGuid, &smbios_guid) == 0) {
            *table_addr = g_st->ConfigurationTable[i].VendorTable;
            // SMBIOS 2.x entry point
            smbios_entry_t *entry = (smbios_entry_t *)*table_addr;
            *table_size = entry->table_len;
            return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}

/**
 * perform SMBIOS table spoofing
 * (will be implemented in spoof/smbios.c)
 */

EFI_STATUS spoof_smbios(VOID)
{
    hw_profile_t profile = {0};
    EFI_STATUS status = load_profile(&profile);
    if (EFI_ERROR(status))
        return status;

    return smbios_spoof_full(&profile);
}
/**
 * load hardware profile from NVRAM or use default
 */
EFI_STATUS load_profile(hw_profile_t *profile)
{
    if (!profile)
        return EFI_INVALID_PARAMETER;

    // TODO: try to load from NVRAM variable
    // TODO: if not found, use default gaming profile

    // for now, use hardcoded default
    AsciiStrCpyS(profile->name, sizeof(profile->name), "Default Gaming Rig");

    // generate random IDs
    UINT8 oui[] = {MAC_REALTEK_1};
    gen_mac_address(profile->mac, oui);
    gen_disk_serial(profile->disk_serial, DISK_SAMSUNG);
    AsciiStrCpyS(profile->disk_model, sizeof(profile->disk_model), "Samsung SSD 980 PRO 1TB");

    // TODO: build complete SMBIOS table

    return EFI_SUCCESS;
}

/**
 * chainload original bootloader (GRUB/Windows Boot Manager)
 */
EFI_STATUS chainload_bootloader(VOID)
{
    EFI_STATUS status;
    EFI_HANDLE *handles = NULL;
    UINTN handle_count = 0;

    // locate all simple file system protocols
    status = g_bs->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL,
                                      &handle_count, &handles);
    if (EFI_ERROR(status)) {
        Print(L"[ERROR] Failed to locate file systems: %r\n", status);
        return status;
    }

    Print(L"[Nexus] Found %u file systems\n", handle_count);

    // try to find bootloader
    // common paths: \EFI\ubuntu\grubx64.efi, \EFI\Microsoft\Boot\bootmgfw.efi
    CHAR16 *bootloader_paths[] = {
        L"\\EFI\\ubuntu\\grubx64.efi", L"\\EFI\\debian\\grubx64.efi",
        L"\\EFI\\fedora\\grubx64.efi", L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi",
        L"\\EFI\\Boot\\bootx64.efi",   NULL};

    for (UINTN i = 0; i < handle_count; i++) {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
        status = g_bs->HandleProtocol(handles[i], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fs);
        if (EFI_ERROR(status))
            continue;

        EFI_FILE_PROTOCOL *root;
        status = fs->OpenVolume(fs, &root);
        if (EFI_ERROR(status))
            continue;

        // try each bootloader path
        for (UINTN j = 0; bootloader_paths[j] != NULL; j++) {
            EFI_FILE_PROTOCOL *bootloader;
            status = root->Open(root, &bootloader, bootloader_paths[j], EFI_FILE_MODE_READ, 0);

            if (!EFI_ERROR(status)) {
                Print(L"[Nexus] Found bootloader: %s\n", bootloader_paths[j]);
                bootloader->Close(bootloader);
                root->Close(root);

                // load and execute bootloader
                EFI_DEVICE_PATH_PROTOCOL *path;
                path = FileDevicePath(handles[i], bootloader_paths[j]);
                if (!path) {
                    Print(L"[ERROR] Failed to create device path\n");
                    continue;
                }

                EFI_HANDLE loader_handle;
                status = g_bs->LoadImage(FALSE, g_image, path, NULL, 0, &loader_handle);
                if (EFI_ERROR(status)) {
                    Print(L"[ERROR] Failed to load image: %r\n", status);
                    continue;
                }

                status = g_bs->StartImage(loader_handle, NULL, NULL);
                if (EFI_ERROR(status)) {
                    Print(L"[ERROR] Failed to start image: %r\n", status);
                    continue;
                }

                // if we get here, bootloader returned (shouldn't happen)
                return EFI_SUCCESS;
            }
        }

        root->Close(root);
    }

    g_bs->FreePool(handles);
    return EFI_NOT_FOUND;
}
