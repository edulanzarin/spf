#ifndef HW_DEFS_H
#define HW_DEFS_H

#include <stdint.h>

// vendor IDs (PCI)
#define VENDOR_INTEL   0x8086
#define VENDOR_AMD     0x1022
#define VENDOR_NVIDIA  0x10DE
#define VENDOR_AMD_GPU 0x1002
#define VENDOR_REALTEK 0x10EC

// common device IDs
#define DEVICE_RTX_3060   0x2503
#define DEVICE_RTX_3070   0x2484
#define DEVICE_RTX_4090   0x2684
#define DEVICE_RX_6700XT  0x73DF
#define DEVICE_RX_7900XTX 0x744C

// MAC vendor prefixes (OUI)
#define MAC_REALTEK_1 0x00, 0xE0, 0x4C
#define MAC_REALTEK_2 0xD8, 0x5E, 0xD3
#define MAC_INTEL_1   0x00, 0x1B, 0x21
#define MAC_INTEL_2   0xA4, 0xBF, 01
#define MAC_ASUS_1    0x04, 0x42, 0x1A

// UEFI GUIDs
#define EFI_SMBIOS_TABLE_GUID                              \
    {                                                      \
        0xeb9d2d31, 0x2d88, 0x11d3,                        \
        {                                                  \
            0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4f \
        }                                                  \
    }

#define EFI_ACPI_TABLE_GUID                                \
    {                                                      \
        0x8868e871, 0xe4f1, 0x11d3,                        \
        {                                                  \
            0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81 \
        }                                                  \
    }

// limits
#define MAX_DISKS     8
#define MAX_NICS      16
#define MAX_RAM_SLOTS 8
#define HWID_BUF_SIZE 256

// common vendors (strings)
#define BIOS_VENDOR_AMI     "American Megatrends Inc."
#define BIOS_VENDOR_AWARD   "Award Software International, Inc."
#define BIOS_VENDOR_PHOENIX "Phoenix Technologies Ltd."

#define MOBO_ASUS     "ASUSTeK COMPUTER INC."
#define MOBO_MSI      "Micro-Star International Co., Ltd."
#define MOBO_GIGABYTE "Gigabyte Technology Co., Ltd."
#define MOBO_ASROCK   "ASRock"

#define CPU_INTEL "GenuineIntel"
#define CPU_AMD   "AuthenticAMD"

#define DISK_SAMSUNG  "Samsung"
#define DISK_WD       "Western Digital"
#define DISK_KINGSTON "Kingston"

#endif
