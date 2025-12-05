#ifndef ENTRY_H
#define ENTRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <json.h>

#include "util.h"

#define ENTRY_CATEGORY_MAX_LEN 1024

#define ENTRY_ARRAY_OPERATION_SUCCESS 1
#define ENTRY_ARRAY_OPERATION_FAILURE 0

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

typedef enum {
	ENTRY_COMPONENT_START = 0,

	ENTRY_COMPONENT_DATE,
	ENTRY_COMPONENT_TYPE,
	ENTRY_COMPONENT_CATEGORY,
	ENTRY_COMPONENT_AMOUNT,

	ENTRY_COMPONENT_END
} EntryTypeOfComponent;

EntryArray entry_array_create(size_t initial_size);
void       entry_array_add(EntryArray *array, Entry entry); // automatically sorts
uint8_t    entry_array_remove(EntryArray *array, uint32_t i);
void       entry_array_destroy(EntryArray *array);

int32_t entry_array_get_from_json_file(EntryArray *dest, const char *file_name); // assumes entry array is initialised
int32_t entry_array_to_json_file(const EntryArray *array, const char *file_name);

void entry_print(const Entry *entry);

#endif
