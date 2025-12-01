#include <stdlib.h>

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

	array->data[array->count++] = entry;
}

void entry_array_destroy(EntryArray *array)
{
	free(array->data);
}
