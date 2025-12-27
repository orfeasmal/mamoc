// The code in this file is hard to maintain and hacked together!!!
// Programming menu systems is hard!!

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "util.h"
#include "entry.h"

#define MAX_PATH_LEN 4096

typedef enum {
	STATE_HOME_MENU,
	STATE_SAVE_MENU,
	STATE_EXIT_MENU,
	STATE_ADDING_ENTRY,
	STATE_REMOVING_ENTRY,
	STATE_EDITING_ENTRY
} State;

#define USER_CANCELED 0
#define USER_ENTERED_COMPONENT 1

static int32_t user_enter_entry_compoment(Entry *entry_to_modify, EntryTypeOfComponent component_to_get);

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
		if (entry_array_get_from_json_file(&entries, config_file_path) != ENTRY_ARRAY_OPERATION_SUCCESS) {
			fprintf(stderr, "Continuing with empty entry list\n");
			entries = entry_array_create(16);
		}
	}

	const size_t input_buffer_size = ENTRY_CATEGORY_MAX_LEN + 1;
	char *input_buffer = malloc(input_buffer_size * (sizeof *input_buffer));

	Entry temp_entry; // temporary_variable

	State state = STATE_HOME_MENU;

	while (!quit) {
		if (state == STATE_HOME_MENU) {
			printf("What to do? (1. Add entry, 2. Remove entry, 3. Edit entry, 4. List entries, 5. Save, 6. Exit): ");

			util_user_input(input_buffer, input_buffer_size);
			const int32_t multiple_choice_input = strtol(input_buffer, NULL, 10);

			switch (multiple_choice_input) {
				case 1:
					state = STATE_ADDING_ENTRY;
					break;
				case 2:
					state = STATE_REMOVING_ENTRY;
					break;
				case 3:
					state = STATE_EDITING_ENTRY;
					break;
				case 4:
					if (entries.count == 0)
						printf("No entries!\n");

					for (size_t i = 0; i < entries.count; ++i) {
						printf("ENTRY %lu\n", i);
						entry_print(&entries.data[i]);
					}
					break;
				case 5:
					state = STATE_SAVE_MENU;
					break;
				case 6:
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

			if (strlen(config_file_path) == 0) {
				size_t config_file_path_length;
				do {
					printf("Enter the path for the new config file: ");
					config_file_path_length = util_user_input(config_file_path, config_file_path_size);
				} while (config_file_path_length == 0);
			}

			entry_array_to_json_file(&entries, config_file_path);

			printf("Saved to %s\n", config_file_path);
		}
		else if (state == STATE_EXIT_MENU) {
			if (!saved) {
				printf("Entry list is not saved, are you sure you want to exit? (1. Yes, 2. No): ");
				util_user_input(input_buffer, input_buffer_size);
				const int32_t multiple_choice_input = strtol(input_buffer, NULL, 10);

				if (multiple_choice_input == 2) {
					state = STATE_HOME_MENU;
					continue;
				}
			}

			quit = true;
		}
		else if (state == STATE_ADDING_ENTRY) {
			printf("At any point press c to cancel adding new entry\n");

			bool user_canceled = false;

			for (uint32_t i = ENTRY_COMPONENT_START + 1; i < ENTRY_COMPONENT_END; ++i) {
				if (user_enter_entry_compoment(&temp_entry, i) == USER_CANCELED) {
					user_canceled = true;
					break;
				}
			}

			if (user_canceled)
				printf("Canceled entering entry\n");
			else
				entry_array_add(&entries, temp_entry);

			state = STATE_HOME_MENU;
			saved = false;
		}
		else if (state == STATE_REMOVING_ENTRY) {
			printf("Enter the index of the entry you want to remove (enter c to cancel): ");
			util_user_input(input_buffer, input_buffer_size);

			const size_t index = strtoull(input_buffer, NULL, 10);

			if (entry_array_remove(&entries, index) == ENTRY_ARRAY_OPERATION_SUCCESS || strcmp(input_buffer, "c") == 0)
				state = STATE_HOME_MENU;
			else
				fprintf(stderr, "error: entry with index %zu doesn't exist!\n", index);
		}
		else if (state == STATE_EDITING_ENTRY) {
			state = STATE_HOME_MENU;

			printf("Enter the index of the entry you want to edit (enter c to cancel): ");
			size_t input_buffer_length = util_user_input(input_buffer, input_buffer_size);

			if (strcmp(input_buffer, "c") == 0)
				continue;

			if (input_buffer_length == 0) {
				state = STATE_EDITING_ENTRY;
				continue;
			}

			const size_t index = strtoull(input_buffer, NULL, 10);

			if (index < 0 || index >= entries.count) {
				fprintf(stderr, "error: entry with index %zu doesn't exist!\n", index);
				state = STATE_EDITING_ENTRY;
				continue;
			}

			Entry *entry = &entries.data[index];
			entry_print(entry);

			while (true) {
				printf("Enter the number of the entry component you want to edit (enter c to cancel): ");
				input_buffer_length = util_user_input(input_buffer, input_buffer_size);

				if (input_buffer_length == 0)
					continue;

				if (strcmp(input_buffer, "c") == 0)
					break;
					
				const EntryTypeOfComponent type_of_component = strtol(input_buffer, NULL, 10);

				if (type_of_component <= ENTRY_COMPONENT_START || type_of_component >= ENTRY_COMPONENT_END) {
					input_buffer_length = 0;
					continue;
				}

				if (user_enter_entry_compoment(entry, type_of_component) == USER_CANCELED)
					printf("Canceled editing component\n");

				break;
			}

		}
	}

	free(input_buffer);
	free(config_file_path);
	entry_array_destroy(&entries);
}

static int32_t user_enter_entry_compoment(Entry *entry_to_modify, EntryTypeOfComponent component_to_get)
{
	int32_t return_value = USER_ENTERED_COMPONENT;

	size_t input_buffer_size = ENTRY_CATEGORY_MAX_LEN + 1;
	char *input_buffer = calloc(input_buffer_size, sizeof *input_buffer);
	size_t input_buffer_length = 0;

	while (true) {
		if (component_to_get == ENTRY_COMPONENT_DATE) {
			printf("Enter the date (format <year> <month> <day>, or enter c to cancel): ");
			input_buffer_length = util_user_input(input_buffer, input_buffer_size);

			if (strcmp(input_buffer, "c") == 0) {
				return_value = USER_CANCELED;
				break;
			}

			if (input_buffer_length == 0)
				continue;

			char *year = strtok(input_buffer, " ");
			char *month = strtok(NULL, " ");
			char *day = strtok(NULL, " ");

			if (year == NULL || month == NULL || day == NULL)
				continue;

			Date date = {
				.y = strtol(year, NULL, 10),
				.m = strtol(month, NULL, 10),
				.d = strtol(day, NULL, 10)
			};

			if (!util_date_is_valid(date)) {
				fprintf(stderr, "Please enter a valid date\n");
				continue;
			}

			entry_to_modify->date = date;
		}
		else if (component_to_get == ENTRY_COMPONENT_TYPE) {
			printf("Enter the entry type (1. Expense, 2. Income, or enter c to cancel): ");
			input_buffer_length = util_user_input(input_buffer, input_buffer_size);

			if (strcmp(input_buffer, "c") == 0) {
				return_value = USER_CANCELED;
				break;
			}

			if (input_buffer_length == 0)
				continue;

			const int32_t multiple_choice_input = strtol(input_buffer, NULL, 10);
			if (multiple_choice_input == 1)
				entry_to_modify->type = ENTRY_EXPENSE;
			else if (multiple_choice_input == 2)
				entry_to_modify->type = ENTRY_INCOME;
			else
				continue;
		}
		else if (component_to_get == ENTRY_COMPONENT_CATEGORY) {
			printf("Enter the category name (enter c to cancel): ");
			const size_t input_buffer_length = util_user_input(input_buffer, input_buffer_size);

			if (strcmp(input_buffer, "c") == 0) {
				return_value = USER_CANCELED;
				break;
			}

			if (input_buffer_length == 0)
				continue;

			strncpy(entry_to_modify->category, input_buffer, ENTRY_CATEGORY_MAX_LEN);
		}
		else if (component_to_get == ENTRY_COMPONENT_AMOUNT) {
			printf("Enter the amount (enter c to cancel): ");
			const size_t input_buffer_length = util_user_input(input_buffer, input_buffer_size);

			if (strcmp(input_buffer, "c") == 0) {
				return_value = USER_CANCELED;
				break;
			}

			if (input_buffer_length == 0)
				continue;
			entry_to_modify->amount = strtod(input_buffer, NULL);
		}

		break;
	}

	free(input_buffer);

	return return_value;
}
