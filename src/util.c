#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

// char *util_file_read(const char *name)
// {
// 	FILE *file = fopen(name, "r");
// 	if (file == NULL)
// 		return NULL;
//
// 	fseek(file, 0, SEEK_END);
// 	size_t file_size = ftell(file);
// 	fseek(file, 0, SEEK_SET);
//
// 	char *buffer = malloc(file_size + 1);
// 	fread(buffer, sizeof(char), file_size, file);
//
// 	fclose(file);
//
// 	return buffer;
// }
//
// uint8_t util_file_write(const char *name, const char *src)
// {
// 	FILE *file = fopen(name, "w+");
//
// 	if (file == NULL)
// 		return UTIL_FILE_WRITE_FAILURE;
//
// 	fwrite(src, sizeof(char), strlen(src), file);
//
// 	fclose(file);
//
// 	return UTIL_FILE_WRITE_SUCCESS;
// }

void util_user_input(char *buffer, size_t buffer_size)
{
	while (fgets(buffer, buffer_size, stdin) == NULL)
		fprintf(stderr, "error: fgets failed, trying again\n");

	buffer[strcspn(buffer, "\n")] = 0;
}

bool util_is_date_valid(int32_t year, int32_t month, int32_t day)
{
	if (year < 0 || day < 1)
		return false;

	switch (month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if (day > 31)
				return false;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if (day > 30)
				return false;
			break;
		case 2:
			if (day > 28 && !(year % 4 == 0 && day == 29))
				return false;
			break;
		default: 
			return false;
	}

	return true;
}
