#ifndef _HACKER_NAMES_
#define _HACKER_NAMES_

#include <Arduino.h>

#include <stdint.h>
#include <stddef.h>

size_t hacker_name_count(void);
void hacker_name(size_t index, char *out, size_t out_size);

#endif // _HACKER_NAMES_
