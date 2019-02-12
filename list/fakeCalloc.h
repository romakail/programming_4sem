#ifndef FAKE_CALLOC_H_INCLUDED
#define FAKE_CALLOC_H_INCLUDED

#include <stdlib.h>

extern float FAIL_PROBABILITY;

void* fakeCalloc (size_t nmemb, size_t size);

#endif
