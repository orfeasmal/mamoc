#ifndef ENTRY_H
#define ENTRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <json.h>

#include "util.h"

#define ENTRY_CATEGORY_MAX_LEN 1024

typedef enum {
	ENTRY_EXPENSE,
	ENTRY_INCOME
} EntryType;

typedef struct {
	double amount;
	EntryType type;
	Date date;
	char category[ENTRY_CATEGORY_MAX_LEN + 1];
} Entry;

typedef struct {
	Entry *data;
	size_t size;
	size_t count;
} EntryArray; // always sorted by date

EntryArray entry_array_create(size_t initial_size);
void       entry_array_add(EntryArray *array, Entry entry); // automatically sorts
void       entry_array_remove(EntryArray *array, uint32_t index);
void       entry_array_destroy(EntryArray *array);

#define ENTRY_ARRAY_JSON_SUCCESS 1
#define ENTRY_ARRAY_JSON_FAILURE 0

uint8_t entry_array_get_from_json_file(EntryArray *dest, const char *file_name); // assumes entry array is initialised
uint8_t entry_array_to_json_file(const EntryArray *array, const char *file_name);

#endif
