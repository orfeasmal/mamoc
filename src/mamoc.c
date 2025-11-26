#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <json.h>

#include "util.h"
#include "mamoc.h"
#include "entry.h"


void mamoc_run(void)
{
	bool quit = false;

	// const size_t max_path_len = 1000;
	// char *config_path = malloc(max_path_len * (sizeof *config_path));
	//
	// printf("Enter config file path: ");
	// fgets(config_path, max_path_len, stdin);
	// config_path[strcspn(config_path, "\n")] = 0;
	//
	// char *config_str = util_file_read(config_path);
	// if (config_str == NULL) {
	// 	fprintf(stderr, "error: cannot open file %s\n", config_path);
	// 	exit(EXIT_FAILURE);
	// }

	size_t entries_amount = 0;
	size_t entries_size = 64;
	Entry *entries = malloc(entries_size * (sizeof *entries));

	const size_t input_buffer_size = CATEGORY_SIZE;
	char *input_buffer = malloc(input_buffer_size * (sizeof *input_buffer));

	Entry new_entry;

	do {
		printf("Enter the entry type (1. Expense, 2. Income): ");
		util_user_input(input_buffer, input_buffer_size);

		int32_t input = atoi(input_buffer);

		if (input == 1)
			new_entry.type = ENTRY_EXPENSE;
		else if (input == 2)
			new_entry.type = ENTRY_INCOME;
		else
			strcpy(input_buffer, "");
	} while (strcmp(input_buffer, "") == 0);

	do {
		printf("Enter the category name: ");
		util_user_input(input_buffer, input_buffer_size);
	} while (strcmp(input_buffer, "") == 0);

	strcpy(new_entry.category, input_buffer); // unsafe??

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

		new_entry.year = atoi(year); // negative years are allowed!
		new_entry.month = abs(atoi(month));
		new_entry.day = abs(atoi(day));
	} while (strcmp(input_buffer, "") == 0);

	entries[entries_amount++] = new_entry;

	printf("%d\n", new_entry.type);
	printf("%s\n", new_entry.category);
	printf("%lf\n", new_entry.amount);
	printf("%d %d %d\n", new_entry.year, new_entry.month, new_entry.day);

	free(input_buffer);
	free(entries);
}
