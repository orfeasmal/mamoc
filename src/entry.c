#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "entry.h"

EntryArray entry_array_create(size_t initial_size)
{
	EntryArray array = {
		.size = initial_size,
	};

	array.data = malloc(initial_size * (sizeof *array.data));

	return array;
}

void entry_array_add(EntryArray *array, Entry entry)
{
	if (array->count == array->size)
		array = realloc(array->data, (array->size *= 2) * (sizeof *array->data)); 

	uint32_t i;
	for (i = 0; i < array->count; ++i) {
	  	const uint8_t date_comparison = util_date_compare(array->data[i].date, entry.date);
		if (date_comparison == UTIL_DATE_1_GREATER) {
			memmove(&array->data[i + 1], &array->data[i], (array->count - i) * (sizeof *array->data));
			break;
		}
	}

	array->data[i] = entry;
	++array->count;
}

void entry_array_remove(EntryArray *array, uint32_t index)
{
	if (index >= array->count) {
		fprintf(stderr, "internal error: entry_array_remove: there is no entry with index %u in the entry array\n", index);
		return;
	}
	else if (index < array->count - 1)
		memmove(&array->data[index + 1], &array->data[index], array->count - 1 - index); // might be wrong

	--array->count;
}

void entry_array_destroy(EntryArray *array)
{
	free(array->data);
}
