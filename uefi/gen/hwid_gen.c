#include "hwid_gen.h"
#include "../utils/smbios_helpers.h"

static UINT64 seed = 0;
static EFI_SYSTEM_TABLE *systab = NULL;

// xorshift64 PRNG
static UINT64 xorshift64(void)
{
    seed ^= seed << 13;
    seed ^= seed >> 7;
    seed ^= seed << 17;
    return seed;
}

void hwid_gen_init(EFI_SYSTEM_TABLE *st)
{
    systab = st;

    // seed com timestamp UEFI
    EFI_TIME time;
    if (systab->RuntimeServices->GetTime(&time, NULL) == EFI_SUCCESS) {
        seed = (UINT64)time.Year << 32 | (UINT64)time.Month << 24 | (UINT64)time.Day << 16 |
               (UINT64)time.Hour << 8 | (UINT64)time.Minute;
    } else {
        seed = 0xDEADBEEFCAFEBABE; // fallback
    }

    // warm up
    for (int i = 0; i < 10; i++)
        xorshift64();
}

void gen_mac_address(UINT8 *mac, const UINT8 oui[3])
{
    if (!mac)
        return;

    mac[0] = oui[0];
    mac[1] = oui[1];
    mac[2] = oui[2];

    UINT64 r = xorshift64();
    mac[3] = (r >> 0) & 0xFF;
    mac[4] = (r >> 8) & 0xFF;
    mac[5] = (r >> 16) & 0xFF;

    mac[0] &= 0xFE;
    mac[0] |= 0x02;
}

void gen_uuid(UINT8 uuid[16])
{
    if (!uuid)
        return;

    for (int i = 0; i < 16; i += 8) {
        UINT64 r = xorshift64();
        CopyMem(&uuid[i], &r, 8);
    }

    uuid[6] = (uuid[6] & 0x0F) | 0x40;
    uuid[8] = (uuid[8] & 0x3F) | 0x80;
}

void gen_serial(CHAR8 *buf, UINT8 len)
{
    if (!buf || len < 2)
        return;

    const CHAR8 charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (UINT8 i = 0; i < len - 1; i++) {
        UINT64 r = xorshift64();
        buf[i] = charset[r % 36];
    }
    buf[len - 1] = '\0';
}

void gen_disk_serial(CHAR8 *buf, const CHAR8 *vendor)
{
    if (!buf)
        return;

    // Samsung format: S4XNNG0M + 6 digits
    if (AsciiStrStr(vendor, "Samsung")) {
        AsciiSPrint(buf, 20, "S4XNNG0M%06u", (UINT32)(xorshift64() % 1000000));
    }
    // WD format
    else if (AsciiStrStr(vendor, "Western") || AsciiStrStr(vendor, "WD")) {
        CHAR8 suffix[8];
        gen_serial(suffix, 8);
        AsciiSPrint(buf, 20, "WD-WMAZA%a", suffix);
    }
    // Kingston
    else if (AsciiStrStr(vendor, "Kingston")) {
        AsciiSPrint(buf, 20, "50026B%010X", (UINT32)(xorshift64() & 0xFFFFFFFFFF));
    }
    // generic
    else {
        gen_serial(buf, 16);
    }
}

void gen_asset_tag(CHAR8 *buf, UINT8 len)
{
    if (!buf || len < 2)
        return;

    if (len >= 12) {
        buf[0] = 'A';
        buf[1] = 'T';
        buf[2] = '-';
        gen_serial(&buf[3], len - 3);
    } else {
        gen_serial(buf, len);
    }
}
