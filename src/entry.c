#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "entry.h"
#include "json_object.h"
#include "json_util.h"

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
		array->data = realloc(array->data, (array->size *= 2) * (sizeof *array->data)); 

	size_t i;
	for (i = 0; i < array->count; ++i) {
	  	const DateComparison date_comparison = util_date_compare(array->data[i].date, entry.date);
		if (date_comparison == UTIL_DATE_1_GREATER) {
			memmove(&array->data[i + 1], &array->data[i], (array->count - i) * (sizeof *array->data));
			break;
		}
	}

	array->data[i] = entry;
	++array->count;
}

uint8_t entry_array_remove(EntryArray *array, size_t i)
{
	if (i >= array->count)
		return ENTRY_ARRAY_OPERATION_FAILURE;
	else if (i < array->count - 1)
		memmove(&array->data[i], &array->data[i + 1], (array->count - i - 1) * (sizeof *array->data));

	--array->count;

	return ENTRY_ARRAY_OPERATION_SUCCESS;
}

void entry_array_destroy(EntryArray *array)
{
	free(array->data);
}

int32_t entry_array_get_from_json_file(EntryArray *array, const char *file_name)
{
	*array = entry_array_create(1);
	Entry new_entry;

	json_object *json = json_object_from_file(file_name);
	if (json == NULL) {
		fprintf(stderr, "json error: %s", json_util_get_last_err());
		exit(EXIT_FAILURE);
	}
	
	array_list *json_entry_array = json_object_get_array(json);
	for (uint32_t i = 0; i < json_entry_array->length; ++i) {
		json_object *json_entry = array_list_get_idx(json_entry_array, i);
		json_object *json_temp;

		new_entry = (Entry) { 0 };

		// Date
		if (!json_object_object_get_ex(json_entry, "year", &json_temp)) {
			fprintf(stderr, "error: missing year value in entry index %u of the json array\n", i);
			exit(EXIT_FAILURE);
		}
		const int32_t year = json_object_get_int(json_temp);

		if (!json_object_object_get_ex(json_entry, "month", &json_temp)) {
			fprintf(stderr, "error: missing month value in entry index %u of the json array\n", i);
			exit(EXIT_FAILURE);
		}
		const int32_t month = json_object_get_int(json_temp);

		if (!json_object_object_get_ex(json_entry, "day", &json_temp)) {
			fprintf(stderr, "error: missing day value in entry index %u of the json array\n", i);
			exit(EXIT_FAILURE);
		}
		const int32_t day = json_object_get_int(json_temp);

		const Date date = {
			.y = year,
			.m = month,
			.d = day
		};

		if (!util_date_is_valid(date)) {
			fprintf(stderr, "error: invalid date in entry index %u of the json array\n", i);
			exit(EXIT_FAILURE);
		}
		
		// Type
		if (!json_object_object_get_ex(json_entry, "type", &json_temp)) {
			fprintf(stderr, "error: missing type value in entry index %u of the json array\n", i);
			exit(EXIT_FAILURE);
		}

		const int32_t type = json_object_get_int(json_temp);
		if (type != 0 && type != 1) {
			fprintf(stderr, "error: invalid type value in entry index %u of the json array (must be 0 or 1)\n", i);
			exit(EXIT_FAILURE);
		}

		// Category
		if (!json_object_object_get_ex(json_entry, "category", &json_temp)) {
			fprintf(stderr, "error: missing category value in entry index %u of the json array\n", i);
			exit(EXIT_FAILURE);
		}

		const char * const category = json_object_get_string(json_temp);
		if (strlen(category) > ENTRY_CATEGORY_MAX_LEN) {
			fprintf(stderr, "error: category string length too large in entry index %u of the json_array\n", i);
			exit(EXIT_FAILURE);
		}

		// Amount
		if (!json_object_object_get_ex(json_entry, "amount", &json_temp)) {
			fprintf(stderr, "error: missing amount value in entry index %u of the json array\n", i);
			exit(EXIT_FAILURE);
		}
		const double amount = json_object_get_double(json_temp);

		new_entry = (Entry) {
			.type = type,
			.amount = amount,
			.date = date
		};
		strncpy(new_entry.category, category, ENTRY_CATEGORY_MAX_LEN);

		entry_array_add(array, new_entry);
	}

	json_object_put(json);

	return ENTRY_ARRAY_OPERATION_SUCCESS;
}

int32_t entry_array_to_json_file(const EntryArray *array, const char *file_name)
{
	json_object *json_array_object = json_object_new_array();

	for (uint32_t i = 0; i < array->count; ++i) {
		Entry entry = array->data[i];
		json_object *temp = json_object_new_object();

		json_object_object_add(temp, "year",     json_object_new_int(entry.date.y));
		json_object_object_add(temp, "month",    json_object_new_int(entry.date.m));
		json_object_object_add(temp, "day",      json_object_new_int(entry.date.d));
		json_object_object_add(temp, "type",     json_object_new_int(entry.type));
		json_object_object_add(temp, "category", json_object_new_string(entry.category));
		json_object_object_add(temp, "amount",   json_object_new_double(entry.amount));

		json_object_array_add(json_array_object, temp);
	}

	if (json_object_to_file_ext(file_name, json_array_object, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_PRETTY_TAB) == -1) {
		fprintf(stderr, "json error: %s\n", json_util_get_last_err());
		return ENTRY_ARRAY_OPERATION_FAILURE;
	}

	json_object_put(json_array_object);

	return ENTRY_ARRAY_OPERATION_SUCCESS;
}

void entry_print(const Entry *entry)
{
	printf("-----------------------------\n");
	printf("1. Date: %d-%d-%d\n", entry->date.y, entry->date.m, entry->date.d);

	if (entry->type == ENTRY_EXPENSE)
		printf("2. Type: Expense\n");
	else if (entry->type == ENTRY_INCOME)
		printf("2. Type: Income\n");

	printf("3. Category: %s\n",   entry->category);
	printf("4. Amount: %.2lf\n",  entry->amount);
	printf("-----------------------------\n");
}
