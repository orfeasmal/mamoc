#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JSIMPLON_IMPLEMENTATION
#include <jsimplon.h>

#include "entry.h"
#include "util.h"

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

/*
	TODO: Implement this thing
void entry_array_print_stats(const EntryArray *array, Date lower, Date upper)
{
	if (util_date_compare(lower, upper) == UTIL_DATE_1_LESSER) {
		printf("Second date must be greater than or equal to the first!");
		return;
	}

	if (array->count == 0) {
		printf("No entries to print stats of!!!\n");
		return;
	}

	size_t lower_index;
	for (lower_index = 0; lower_index < array->count - 1; ++lower_index) {
		if (util_date_compare(array->data[lower_index].date, lower) == UTIL_DATE_1_GREATER)
			break;
	}

	size_t upper_index;
	for (upper_index = array->count - 1; upper_index > 0; --upper_index) {
		if (util_date_compare(array->data[upper_index].date, lower) == UTIL_DATE_1_LESSER)
			break;
	}

	for (size_t i = lower_index; i <= upper_index; ++i) {
		
	}
}
*/

int32_t entry_array_get_from_json_file(EntryArray *array, const char *file_name)
{
	*array = entry_array_create(1);
	Entry new_entry;

	char *error;
	Jsimplon_Value *json_root_value = jsimplon_tree_from_file(&error, file_name);

	if (error != NULL) {
		fprintf(stderr, "json error: %s\n", error);
		free(error);
		exit(EXIT_FAILURE);
	}

	Jsimplon_Array *json_entry_array = jsimplon_value_get_array(json_root_value);
	size_t json_entry_array_count = jsimplon_array_get_count(json_entry_array);

	for (size_t i = 0; i < json_entry_array_count; ++i) {
		Jsimplon_Value *json_entry_value = jsimplon_array_get_value_at_index(json_entry_array, i);
		Jsimplon_Object *json_entry = jsimplon_value_get_object(json_entry_value);

		Jsimplon_Member *json_temp;

		new_entry = (Entry) { 0 };

		// Date
		json_temp = jsimplon_object_get_member(json_entry, "year");
		if (!json_temp) {
			fprintf(stderr, "error: missing year value in entry index %lu of the json array\n", i);
			exit(EXIT_FAILURE);
		}
		const int32_t year = jsimplon_member_get_number(json_temp);

		json_temp = jsimplon_object_get_member(json_entry, "month");
		if (!json_temp) {
			fprintf(stderr, "error: missing month value in entry index %lu of the json array\n", i);
			exit(EXIT_FAILURE);
		}
		const int32_t month = jsimplon_member_get_number(json_temp);

		json_temp = jsimplon_object_get_member(json_entry, "day");
		if (!json_temp) {
			fprintf(stderr, "error: missing day value in entry index %lu of the json array\n", i);
			exit(EXIT_FAILURE);
		}
		const int32_t day = jsimplon_member_get_number(json_temp);

		const Date date = {
			.y = year,
			.m = month,
			.d = day
		};

		if (!util_date_is_valid(date)) {
			fprintf(stderr, "error: invalid date in entry index %zu of the json array\n", i);
			exit(EXIT_FAILURE);
		}
		
		// Type
		json_temp = jsimplon_object_get_member(json_entry, "type");
		if (!json_temp) {
			fprintf(stderr, "error: missing type value in entry index %zu of the json array\n", i);
			exit(EXIT_FAILURE);
		}

		const int32_t type = jsimplon_member_get_number(json_temp);
		if (type != 0 && type != 1) {
			fprintf(stderr, "error: invalid type value in entry index %zu of the json array (must be 0 or 1)\n", i);
			exit(EXIT_FAILURE);
		}

		// Category
		json_temp = jsimplon_object_get_member(json_entry, "category");
		if (!json_temp) {
			fprintf(stderr, "error: missing category value in entry index %zu of the json array\n", i);
			exit(EXIT_FAILURE);
		}

		const char * const category = jsimplon_member_get_str(json_temp);
		if (strlen(category) > ENTRY_CATEGORY_MAX_LEN) {
			fprintf(stderr, "error: category string length too large in entry index %zu of the json_array\n", i);
			exit(EXIT_FAILURE);
		}

		// Amount
		json_temp = jsimplon_object_get_member(json_entry, "amount");
		if (!json_temp) {
			fprintf(stderr, "error: missing amount value in entry index %zu of the json array\n", i);
			exit(EXIT_FAILURE);
		}
		const double amount = jsimplon_member_get_number(json_temp);

		new_entry = (Entry) {
			.type = type,
			.amount = amount,
			.date = date
		};
		strncpy(new_entry.category, category, ENTRY_CATEGORY_MAX_LEN);

		entry_array_add(array, new_entry);
	}

	jsimplon_tree_destroy(json_root_value);

	return ENTRY_ARRAY_OPERATION_SUCCESS;
}

int32_t entry_array_to_json_file(const EntryArray *array, const char *file_name)
{
	Jsimplon_Value *json_root_value = jsimplon_tree_root_create();
	Jsimplon_Array *json_entry_array = jsimplon_value_set_array(json_root_value);

	for (size_t i = 0; i < array->count; ++i) {
		Entry entry = array->data[i];

		Jsimplon_Object *json_object = jsimplon_array_push_object(json_entry_array);

		jsimplon_object_add_member_number(json_object, "year", entry.date.y);
		jsimplon_object_add_member_number(json_object, "month", entry.date.m);
		jsimplon_object_add_member_number(json_object, "day", entry.date.d);
		jsimplon_object_add_member_number(json_object, "type", entry.type);
		jsimplon_object_add_member_str(json_object, "category", entry.category);
		jsimplon_object_add_member_number(json_object, "amount", entry.amount);
	}

	char *error;
	jsimplon_tree_to_file(&error, json_root_value, file_name);
	jsimplon_tree_destroy(json_root_value);

	if (error != NULL) {
		fprintf(stderr, "json error: %s\n", error);
		return ENTRY_ARRAY_OPERATION_FAILURE;
	}

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
