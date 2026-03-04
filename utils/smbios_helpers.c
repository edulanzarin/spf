#include "smbios_helpers.h"
#include <stdio.h>
#include <string.h>

void smbios_table_init(smbios_table_t *table)
{
    if (!table)
        return;
    memset(table, 0, sizeof(smbios_table_t));
}

uint16_t smbios_add_entry(smbios_table_t *table, uint8_t type, const void *data, uint16_t data_len)
{
    if (!table || !data || table->count >= 64)
        return 0xFFFF;

    smbios_table_entry_t *entry = &table->entries[table->count];
    uint16_t handle = table->count;

    entry->type = type;
    entry->length = data_len + 4;
    entry->handle = handle;

    if (data_len > 512)
        data_len = 512;
    memcpy(entry->data, data, data_len);

    table->count++;
    return handle;
}

smbios_table_entry_t *smbios_find_entry(smbios_table_t *table, uint8_t type, uint16_t index)
{
    if (!table)
        return NULL;

    uint16_t found = 0;
    for (uint16_t i = 0; i < table->count; i++) {
        if (table->entries[i].type == type) {
            if (found == index)
                return &table->entries[i];
            found++;
        }
    }
    return NULL;
}

uint16_t smbios_count_type(smbios_table_t *table, uint8_t type)
{
    if (!table)
        return 0;

    uint16_t count = 0;
    for (uint16_t i = 0; i < table->count; i++) {
        if (table->entries[i].type == type)
            count++;
    }
    return count;
}

bool smbios_remove_entry(smbios_table_t *table, uint16_t handle)
{
    if (!table)
        return false;

    int idx = -1;
    for (uint16_t i = 0; i < table->count; i++) {
        if (table->entries[i].handle == handle) {
            idx = i;
            break;
        }
    }

    if (idx == -1)
        return false;

    memmove(&table->entries[idx], &table->entries[idx + 1],
            (table->count - idx - 1) * sizeof(smbios_table_entry_t));

    table->count--;
    return true;
}

bool smbios_add_strings(smbios_table_entry_t *entry, const char **strings, uint8_t count)
{
    if (!entry || !strings || count == 0)
        return false;

    uint16_t offset = entry->length - 4;
    uint8_t *str_area = entry->data + offset;
    uint16_t space_left = 512 - offset;

    for (uint8_t i = 0; i < count; i++) {
        if (!strings[i])
            continue;

        size_t len = strlen(strings[i]) + 1;
        if (len > space_left)
            return false;

        memcpy(str_area, strings[i], len);
        str_area += len;
        space_left -= len;
        offset += len;
    }

    if (space_left < 1)
        return false;
    *str_area = 0;

    return true;
}

const char *smbios_get_string(const smbios_table_entry_t *entry, uint8_t string_index)
{
    if (!entry || string_index == 0)
        return NULL;

    const uint8_t *str_area = entry->data + (entry->length - 4);

    uint8_t current = 1;
    while (current < string_index) {
        while (*str_area != 0)
            str_area++;
        str_area++;
        current++;

        if (*str_area == 0)
            return NULL;
    }

    return (const char *)str_area;
}

uint8_t smbios_calc_checksum(const smbios_table_t *table)
{
    if (!table)
        return 0;

    uint8_t sum = 0;
    for (uint16_t i = 0; i < table->count; i++) {
        smbios_table_entry_t *e = (smbios_table_entry_t *)&table->entries[i];
        for (uint16_t j = 0; j < e->length; j++)
            sum += ((uint8_t *)e)[j];
    }
    return (256 - sum) & 0xFF;
}

uint32_t smbios_serialize(const smbios_table_t *table, uint8_t *buffer, uint32_t buf_size)
{
    if (!table || !buffer)
        return 0;

    uint32_t offset = 0;
    for (uint16_t i = 0; i < table->count; i++) {
        const smbios_table_entry_t *e = &table->entries[i];
        uint32_t entry_size = e->length + 2;

        if (offset + entry_size > buf_size)
            return 0;

        buffer[offset++] = e->type;
        buffer[offset++] = e->length;
        buffer[offset++] = e->handle & 0xFF;
        buffer[offset++] = (e->handle >> 8) & 0xFF;

        memcpy(buffer + offset, e->data, e->length - 4);
        offset += e->length - 4;

        buffer[offset++] = 0;
        buffer[offset++] = 0;
    }

    return offset;
}

bool smbios_deserialize(smbios_table_t *table, const uint8_t *buffer, uint32_t buf_size)
{
    if (!table || !buffer)
        return false;

    smbios_table_init(table);

    uint32_t offset = 0;
    while (offset < buf_size && table->count < 64) {
        uint8_t type = buffer[offset];
        uint8_t length = buffer[offset + 1];
        uint16_t handle = buffer[offset + 2] | (buffer[offset + 3] << 8);

        if (type == 127)
            break;

        smbios_table_entry_t *e = &table->entries[table->count];
        e->type = type;
        e->length = length;
        e->handle = handle;

        memcpy(e->data, buffer + offset + 4, length - 4);

        offset += length;
        while (offset < buf_size - 1) {
            if (buffer[offset] == 0 && buffer[offset + 1] == 0) {
                offset += 2;
                break;
            }
            offset++;
        }

        table->count++;
    }

    return true;
}

void smbios_dump_entry(const smbios_table_entry_t *entry)
{
    if (!entry)
        return;

    printf("Type: %d | Length: %d | Handle: 0x%04X\n", entry->type, entry->length, entry->handle);
    printf("Data: ");
    for (int i = 0; i < entry->length - 4 && i < 32; i++)
        printf("%02X ", entry->data[i]);
    printf("\n");
}

void smbios_dump_table(const smbios_table_t *table)
{
    if (!table)
        return;

    printf("SMBIOS Table (%d entries)\n", table->count);
    for (uint16_t i = 0; i < table->count; i++)
        printf("[%d] Type %d, Handle 0x%04X\n", i, table->entries[i].type,
               table->entries[i].handle);
}
