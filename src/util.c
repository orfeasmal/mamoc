#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

char *util_file_read(const char *name)
{
	FILE *file = fopen(name, "r");
	if (file == NULL)
		return NULL;

	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = malloc(file_size + 1);
	fread(buffer, sizeof(char), file_size, file);

	fclose(file);

	return buffer;
}

uint8_t util_file_write(const char *name, const char *src)
{
	FILE *file = fopen(name, "w+");

	if (file == NULL)
		return UTIL_FILE_WRITE_FAILURE;

	fwrite(src, sizeof(char), strlen(src), file);

	fclose(file);

	return UTIL_FILE_WRITE_SUCCESS;
}

void util_user_input(char *buffer, size_t buffer_size)
{
	while (fgets(buffer, buffer_size, stdin) == NULL)
		fprintf(stderr, "error: fgets failed, trying again\n");

	buffer[strcspn(buffer, "\n")] = 0;
}
