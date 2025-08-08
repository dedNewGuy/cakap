#include <ctype.h>
#include <stdbool.h>
#include "common_util.h"

void
str_trim_end(int size, char src[size])
{
	int i = size;
	while (isspace(src[i - 1]) || src[i - 1] == '\0') --i;

	src[++i] = '\0';
}
