#ifndef SMBIOS_DEFS_H
#define SMBIOS_DEFS_H

#include <stdint.h>

// SMBIOS header
typedef struct {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
} __attribute__((packed)) smbios_header_t;

// type 0 - BIOS info
typedef struct {
    smbios_header_t header;
    uint8_t vendor_str;
    uint8_t version_str;
    uint16_t starting_segment;
    uint8_t release_date_str;
    uint8_t rom_size;
    uint64_t characteristics;
    uint8_t ext_characteristics[2];
    uint8_t bios_major_release;
    uint8_t bios_minor_release;
    uint8_t ec_major_release;
    uint8_t ec_minor_release;
} __attribute__((packed)) smbios_type0_t;

// type 1 - system info
typedef struct {
    smbios_header_t header;
    uint8_t manufacturer_str;
    uint8_t product_str;
    uint8_t version_str;
    uint8_t serial_str;
    uint8_t uuid[16];
    uint8_t wakeup_type;
    uint8_t sku_str;
    uint8_t family_str;
} __attribute__((packed)) smbios_type1_t;

// type 2 - baseboard
typedef struct {
    smbios_header_t header;
    uint8_t manufacturer_str;
    uint8_t product_str;
    uint8_t version_str;
    uint8_t serial_str;
    uint8_t asset_tag_str;
    uint8_t feature_flags;
    uint8_t location_str;
    uint16_t chassis_handle;
    uint8_t board_type;
    uint8_t num_contained_handles;
} __attribute__((packed)) smbios_type2_t;

// type 3 - chassis
typedef struct {
    smbios_header_t header;
    uint8_t manufacturer_str;
    uint8_t type;
    uint8_t version_str;
    uint8_t serial_str;
    uint8_t asset_tag_str;
    uint8_t bootup_state;
    uint8_t power_supply_state;
    uint8_t thermal_state;
    uint8_t security_status;
    uint32_t oem_defined;
    uint8_t height;
    uint8_t num_power_cords;
    uint8_t contained_element_count;
    uint8_t contained_element_length;
} __attribute__((packed)) smbios_type3_t;

// type 4 - processor
typedef struct {
    smbios_header_t header;
    uint8_t socket_str;
    uint8_t processor_type;
    uint8_t processor_family;
    uint8_t processor_manufacturer_str;
    uint64_t processor_id;
    uint8_t processor_version_str;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_upgrade;
    uint16_t l1_cache_handle;
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;
    uint8_t serial_str;
    uint8_t asset_tag_str;
    uint8_t part_number_str;
    uint8_t core_count;
    uint8_t core_enabled;
    uint8_t thread_count;
    uint16_t processor_characteristics;
    uint16_t processor_family2;
} __attribute__((packed)) smbios_type4_t;

// type 7 - cache info
typedef struct {
    smbios_header_t header;
    uint8_t socket_str;
    uint16_t cache_config;
    uint16_t max_cache_size;
    uint16_t installed_size;
    uint16_t supported_sram_type;
    uint16_t current_sram_type;
    uint8_t cache_speed;
    uint8_t error_correction_type;
    uint8_t system_cache_type;
    uint8_t associativity;
} __attribute__((packed)) smbios_type7_t;

// type 9 - system slots (PCIe)
typedef struct {
    smbios_header_t header;
    uint8_t designation_str;
    uint8_t slot_type;
    uint8_t slot_data_bus_width;
    uint8_t current_usage;
    uint8_t slot_length;
    uint16_t slot_id;
    uint8_t slot_characteristics1;
    uint8_t slot_characteristics2;
    uint16_t segment_group_number;
    uint8_t bus_number;
    uint8_t device_function_number;
} __attribute__((packed)) smbios_type9_t;

// type 11 - OEM strings
typedef struct {
    smbios_header_t header;
    uint8_t count;
} __attribute__((packed)) smbios_type11_t;

// type 16 - physical memory array
typedef struct {
    smbios_header_t header;
    uint8_t location;
    uint8_t use;
    uint8_t error_correction;
    uint32_t max_capacity;
    uint16_t error_info_handle;
    uint16_t num_devices;
    uint64_t extended_max_capacity;
} __attribute__((packed)) smbios_type16_t;

// type 17 - memory device (RAM sticks)
typedef struct {
    smbios_header_t header;
    uint16_t physical_array_handle;
    uint16_t error_info_handle;
    uint16_t total_width;
    uint16_t data_width;
    uint16_t size;
    uint8_t form_factor;
    uint8_t device_set;
    uint8_t device_locator_str;
    uint8_t bank_locator_str;
    uint8_t memory_type;
    uint16_t type_detail;
    uint16_t speed;
    uint8_t manufacturer_str;
    uint8_t serial_str;
    uint8_t asset_tag_str;
    uint8_t part_number_str;
    uint8_t attributes;
    uint32_t extended_size;
    uint16_t configured_speed;
    uint16_t min_voltage;
    uint16_t max_voltage;
    uint16_t configured_voltage;
} __attribute__((packed)) smbios_type17_t;

// type 19 - memory array mapped address
typedef struct {
    smbios_header_t header;
    uint32_t starting_address;
    uint32_t ending_address;
    uint16_t memory_array_handle;
    uint8_t partition_width;
    uint64_t extended_starting_address;
    uint64_t extended_ending_address;
} __attribute__((packed)) smbios_type19_t;

// chassis types (type 3)
#define CHASSIS_TYPE_OTHER      0x01
#define CHASSIS_TYPE_DESKTOP    0x03
#define CHASSIS_TYPE_TOWER      0x07
#define CHASSIS_TYPE_MINI_TOWER 0x06
#define CHASSIS_TYPE_LAPTOP     0x09
#define CHASSIS_TYPE_NOTEBOOK   0x0A

// processor types (type 4)
#define PROC_TYPE_OTHER             0x01
#define PROC_TYPE_CENTRAL_PROCESSOR 0x03

// processor family (type 4)
#define PROC_FAMILY_INTEL_CORE_I3    0xB1
#define PROC_FAMILY_INTEL_CORE_I5    0xB5
#define PROC_FAMILY_INTEL_CORE_I7    0xB3
#define PROC_FAMILY_INTEL_CORE_I9    0xBF
#define PROC_FAMILY_INTEL_XEON       0xB0
#define PROC_FAMILY_AMD_RYZEN        0x6B
#define PROC_FAMILY_AMD_RYZEN_3      0x6B
#define PROC_FAMILY_AMD_RYZEN_5      0x6B
#define PROC_FAMILY_AMD_RYZEN_7      0x6B
#define PROC_FAMILY_AMD_RYZEN_9      0x6B
#define PROC_FAMILY_AMD_THREADRIPPER 0x6B
#define PROC_FAMILY_AMD_EPYC         0x83

// processor upgrades (sockets - type 4)
#define PROC_UPGRADE_LGA1200 0x37
#define PROC_UPGRADE_LGA1700 0x39
#define PROC_UPGRADE_AM4     0x2E
#define PROC_UPGRADE_AM5     0x40
#define PROC_UPGRADE_sTRX4   0x3D

// memory types (type 17)
#define MEM_TYPE_DDR3 0x18
#define MEM_TYPE_DDR4 0x1A
#define MEM_TYPE_DDR5 0x22

// memory form factors (type 17)
#define MEM_FORM_DIMM   0x09
#define MEM_FORM_SODIMM 0x0D

#endif
