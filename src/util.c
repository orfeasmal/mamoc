#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

size_t util_user_input(char *buffer, size_t buffer_size)
{
	while (fgets(buffer, buffer_size, stdin) == NULL)
		fprintf(stderr, "error: fgets failed, trying again\n");

	const size_t newline_index = strcspn(buffer, "\n");

	buffer[newline_index] = 0;

	return newline_index;
}

bool util_date_is_valid(Date date)
{
	if (date.y < 0 || date.d < 1)
		return false;

	switch (date.m) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if (date.d > 31)
				return false;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if (date.d > 30)
				return false;
			break;
		case 2:
			if (date.y % 4 == 0) {
				if (date.d > 29)
					return false;
			}
			else {
				if (date.d > 28)
					return false;
			}
			break;
		default: 
			return false;
	}

	return true;
}

DateComparison util_date_compare(Date date1, Date date2)
{
	if (date1.y < date2.y)
		return UTIL_DATE_1_LESSER;
	else if (date1.y > date2.y)
		return UTIL_DATE_1_GREATER;

	if (date1.m < date2.m)
		return UTIL_DATE_1_LESSER;
	else if (date1.m > date2.m)
		return UTIL_DATE_1_GREATER;

	if (date1.d < date2.d)
		return UTIL_DATE_1_LESSER;
	else if (date1.d > date2.d)
		return UTIL_DATE_1_GREATER;

	return UTIL_DATES_EQUAL;
}
