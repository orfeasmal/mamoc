#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

size_t util_user_input(char *buffer, size_t max_length); // returns the strlen of the user input

typedef struct {
	int32_t y;
	int16_t m, d;
} Date;

bool util_date_is_valid(Date date);

typedef enum {
	UTIL_DATE_1_GREATER,
	UTIL_DATE_1_LESSER,
	UTIL_DATES_EQUAL
} DateComparison;

DateComparison util_date_compare(Date date1, Date date2); // assumes date is valid

#endif 
