#ifndef SMBIOS_HELPERS_H
#define SMBIOS_HELPERS_H

#include "../include/common.h"
#include "../include/smbios_defs.h"
#include <stdbool.h>

// init/cleanup
void smbios_table_init(smbios_table_t *table);

// add/remove entries
uint16_t smbios_add_entry(smbios_table_t *table, uint8_t type, const void *data, uint16_t data_len);
bool smbios_remove_entry(smbios_table_t *table, uint16_t handle);

// search/query
smbios_table_entry_t *smbios_find_entry(smbios_table_t *table, uint8_t type, uint16_t index);
uint16_t smbios_count_type(smbios_table_t *table, uint8_t type);

// strings (SMBIOS format: [struct][str1\0][str2\0]...\0)
bool smbios_add_strings(smbios_table_entry_t *entry, const char **strings, uint8_t count);
const char *smbios_get_string(const smbios_table_entry_t *entry, uint8_t string_index);

// serialize/deserialize
uint32_t smbios_serialize(const smbios_table_t *table, uint8_t *buffer, uint32_t buf_size);
bool smbios_deserialize(smbios_table_t *table, const uint8_t *buffer, uint32_t buf_size);

// utils
uint8_t smbios_calc_checksum(const smbios_table_t *table);
void smbios_dump_entry(const smbios_table_entry_t *entry);
void smbios_dump_table(const smbios_table_t *table);

#endif
