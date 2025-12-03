#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void util_user_input(char *buffer, size_t max_length);

typedef struct {
	int32_t y, m, d;
} Date;

bool util_date_is_valid(Date date);

#define UTIL_DATE_1_GREATER 0
#define UTIL_DATE_1_LESSER  1
#define UTIL_DATES_EQUAL    2

uint8_t util_date_compare(Date date1, Date date2); // assumes date is valid

#endif 
