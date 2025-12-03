#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <json.h>

#include "util.h"
#include "entry.h"

#define MAX_PATH_LEN 4096

int main(int argc, char **argv)
{
	Entry new_entry; // temporary_variable
	EntryArray entries = entry_array_create(128);
	json_object *json = NULL;

	if (argc == 2) { // wanted to make it clear
		json = json_object_from_file(argv[1]);
		if (json == NULL) {
			fprintf(stderr, "json error: %s", json_util_get_last_err());
			return 1;
		}
		
		array_list *json_entry_array = json_object_get_array(json);
		for (uint32_t i = 0; i < json_entry_array->length; ++i) {
			json_object *json_entry = array_list_get_idx(json_entry_array, i);
			json_object *json_temp;

			new_entry = (Entry) { 0 };
			
			if (!json_object_object_get_ex(json_entry, "type", &json_temp)) {
				fprintf(stderr, "error: missing type value in entry index %u of the json array\n", i);
				return 1;
			}

			const int32_t type = json_object_get_int(json_temp);
			if (type != 0 && type != 1) {
				fprintf(stderr, "error: invalid type value in entry index %u of the json array (must be 0 or 1)\n", i);
				return 1;
			}

			if (!json_object_object_get_ex(json_entry, "category", &json_temp)) {
				fprintf(stderr, "error: missing category value in entry index %u of the json array\n", i);
				return 1;
			}

			const char * const category = json_object_get_string(json_temp);
			if (strlen(category) > ENTRY_CATEGORY_MAX_LEN) {
				fprintf(stderr, "error: category string length too large in entry index %u of the json_array\n", i);
				return 1;
			}

			if (!json_object_object_get_ex(json_entry, "amount", &json_temp)) {
				fprintf(stderr, "error: missing amount value in entry index %u of the json array\n", i);
				return 1;
			}
			const double amount = json_object_get_double(json_temp);

			if (!json_object_object_get_ex(json_entry, "year", &json_temp)) {
				fprintf(stderr, "error: missing year value in entry index %u of the json array\n", i);
				return 1;
			}
			const int32_t year = json_object_get_int(json_temp);

			if (!json_object_object_get_ex(json_entry, "month", &json_temp)) {
				fprintf(stderr, "error: missing month value in entry index %u of the json array\n", i);
				return 1;
			}
			const int32_t month = json_object_get_int(json_temp);

			if (!json_object_object_get_ex(json_entry, "day", &json_temp)) {
				fprintf(stderr, "error: missing day value in entry index %u of the json array\n", i);
				return 1;
			}
			const int32_t day = json_object_get_int(json_temp);

			const Date date = {
				.y = year,
				.m = month,
				.d = day
			};

			if (!util_date_is_valid(date)) {
				fprintf(stderr, "error: invalid date in entry index %u of the json array\n", i);
				return 1;
			}

			new_entry = (Entry) {
				.type = type,
				.amount = amount,
				.date = date
			};
			strncpy(new_entry.category, category, ENTRY_CATEGORY_MAX_LEN);

			entry_array_add(&entries, new_entry);
		}

		json_object_put(json);
	}
	else if (argc > 2) {
		fprintf(stderr, "usage: %s\n", argv[0]);
		fprintf(stderr, "or\n");
		fprintf(stderr, "usage: %s <config file path>\n", argv[0]);
		return 1;
	}
	else
		printf("No config file provided\n");

	const size_t input_buffer_size = ENTRY_CATEGORY_MAX_LEN + 1;
	char *input_buffer = malloc(input_buffer_size * (sizeof *input_buffer));

	while (true) {
		printf("What to do? (1. Add new entry, 2. List entries, 3. Save, 4. Exit): ");

		util_user_input(input_buffer, input_buffer_size);
		int32_t multiple_choice_input = atoi(input_buffer);

		if (multiple_choice_input == 4)
			break;

		if (multiple_choice_input == 2) {
			if (entries.count == 0)
				printf("No entries!\n");

			for (uint32_t i = 0; i < entries.count; ++i) {
				Entry temp = entries.data[i];
				printf("-----------------------\n");
				printf("ENTRY %d\n", i + 1);
				printf("Type: %d\n", temp.type);
				printf("Category: %s\n", temp.category);
				printf("Amount: %.2lf\n", temp.amount);
				printf("Date: %d %d %d\n", temp.date.y, temp.date.m, temp.date.d);
				printf("-----------------------\n");
			}

			continue;
		}

		if (multiple_choice_input != 1)
			continue;

		do {
			printf("Enter the entry type (1. Expense, 2. Income): ");
			util_user_input(input_buffer, input_buffer_size);

			multiple_choice_input = atoi(input_buffer);

			if (multiple_choice_input == 1)
				new_entry.type = ENTRY_EXPENSE;
			else if (multiple_choice_input == 2)
				new_entry.type = ENTRY_INCOME;
			else
				strcpy(input_buffer, "");
		} while (strcmp(input_buffer, "") == 0);

		do {
			printf("Enter the category name: ");
			util_user_input(input_buffer, input_buffer_size);
		} while (strcmp(input_buffer, "") == 0);

		strncpy(new_entry.category, input_buffer, ENTRY_CATEGORY_MAX_LEN); // unsafe??

		do {
			printf("Enter the amount: ");
			util_user_input(input_buffer, input_buffer_size);
		} while (strcmp(input_buffer, "") == 0);

		new_entry.amount = atof(input_buffer);

		do {
			printf("Enter the date (format <year> <month> <day>): ");
			util_user_input(input_buffer, input_buffer_size);

			char *year = strtok(input_buffer, " ");
			char *month = strtok(NULL, " ");
			char *day = strtok(NULL, " ");

			if (year == NULL || month == NULL || day == NULL) {
				strcpy(input_buffer, "");
				continue;
			}

			Date date = {
				.y = atoi(year),
				.m = atoi(month),
				.d = atoi(day)
			};

			if (!util_date_is_valid(date)) {
				fprintf(stderr, "Please enter a valid date\n");
				strcpy(input_buffer, "");
				continue;
			}
			new_entry.date = date;
		} while (strcmp(input_buffer, "") == 0);

		entry_array_add(&entries, new_entry);
	}

	free(input_buffer);
	entry_array_destroy(&entries);
}
