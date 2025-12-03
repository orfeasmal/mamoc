#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <json.h>

#include "util.h"
#include "entry.h"

#define MAX_PATH_LEN 4096

typedef enum {
	STATE_HOME_MENU,
	STATE_SAVE_MENU,
	STATE_EXIT_MENU,
	STATE_ENTERING_ENTRY
} State;

int main(int argc, char **argv)
{
	bool quit = false;
	bool saved = false;

	EntryArray entries;

	const size_t config_file_path_size = MAX_PATH_LEN + 1;
	char *config_file_path = calloc(config_file_path_size, sizeof *config_file_path);

	if (argc > 2) {
		fprintf(stderr, "usage: %s\n", argv[0]);
		fprintf(stderr, "or\n");
		fprintf(stderr, "usage: %s <config file path>\n", argv[0]);
		return 1;
	}
	else if (argc < 2) {
		printf("No config file provided, entry list is empty\n");
		entries = entry_array_create(16);
	}
	else {
		strncpy(config_file_path, argv[1], MAX_PATH_LEN);
		if (entry_array_get_from_json_file(&entries, config_file_path) != ENTRY_ARRAY_JSON_SUCCESS) {
			fprintf(stderr, "Continuing with empty entry list\n");
			entries = entry_array_create(16);
		}
	}

	const size_t input_buffer_size = ENTRY_CATEGORY_MAX_LEN + 1;
	char *input_buffer = malloc(input_buffer_size * (sizeof *input_buffer));

	Entry new_entry; // temporary_variable

	State state = STATE_HOME_MENU;

	while (!quit) {
		int32_t multiple_choice_input;

		if (state == STATE_HOME_MENU) {
			printf("What to do? (1. Add new entry, 2. List entries, 3. Save, 4. Exit): ");

			util_user_input(input_buffer, input_buffer_size);
			multiple_choice_input = atoi(input_buffer);

			switch (multiple_choice_input) {
				case 1:
					state = STATE_ENTERING_ENTRY;
					break;
				case 2:
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
						break;
					case 3:
						state = STATE_SAVE_MENU;
						break;
					case 4:
						state = STATE_EXIT_MENU;
						break;
				}
		}
		else if (state == STATE_SAVE_MENU) {
			state = STATE_HOME_MENU;

			if (saved) {
				printf("Already saved!\n");
				continue;
			}

			saved = true;

			if (strcmp(config_file_path, "") == 0) {
				printf("Enter the path for the new config file: ");
				util_user_input(config_file_path, config_file_path_size);
			}

			entry_array_to_json_file(&entries, config_file_path);

			printf("Saved to %s\n", config_file_path);
		}
		else if (state == STATE_ENTERING_ENTRY) {
			state = STATE_HOME_MENU;
			saved = false;

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
		else if (state == STATE_EXIT_MENU) {
			if (!saved) {
				printf("Entry list is not saved, are you sure you want to exit? (1. Yes, 2. No): ");
				util_user_input(input_buffer, input_buffer_size);
				multiple_choice_input = atoi(input_buffer);

				if (multiple_choice_input == 2) {
					state = STATE_HOME_MENU;
					continue;
				}
			}

			quit = true;
		}
	}

	free(input_buffer);
	free(config_file_path);
	entry_array_destroy(&entries);
}
