#ifndef ENTRY_H
#define ENTRY_H

#include <stddef.h>
#include <stdint.h>

#define CATEGORY_SIZE 1024

typedef enum {
	ENTRY_EXPENSE,
	ENTRY_INCOME
} EntryType;

typedef struct {
	double amount;
	EntryType type;
	int32_t year;
	uint8_t day, month;
	char category[CATEGORY_SIZE];
} Entry;

#endif
