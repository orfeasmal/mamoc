#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// #define UTIL_FILE_WRITE_SUCCESS 1
// #define UTIL_FILE_WRITE_FAILURE 0

// char *util_file_read(const char *name); // returns malloc'd buffer, returns null if failed
// uint8_t util_file_write(const char *name, const char *src); // returns success code

void util_user_input(char *buffer, size_t max_length);
bool util_is_date_valid(int32_t year, int32_t month, int32_t day);

#endif 
