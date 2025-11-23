#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <json.h>

#include "util.h"
#include "mamoc.h"

void mamoc_run(void)
{
	json_object *parsed_json, *currency, *amount, *things;

	char *str = file_read("test/test.json");
	parsed_json = json_tokener_parse(str);
	free(str);

	json_object_object_get_ex(parsed_json, "currency", &currency);
	json_object_object_get_ex(parsed_json, "amount", &amount);
	json_object_object_get_ex(parsed_json, "things", &things);
	size_t things_size = json_object_array_length(things);

	printf("You have %d %ss\n", json_object_get_int(amount), json_object_get_string(currency));

	for (uint32_t i = 0; i < things_size; ++i) {
		json_object *thing = json_object_array_get_idx(things, i);
		printf("%lu. %s\n", (unsigned long)i + 1, json_object_get_string(thing));
	}

	json_object_put(parsed_json);
	json_object_put(currency);
	json_object_put(amount);
	json_object_put(things);
}
