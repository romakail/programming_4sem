#include "fakeCalloc.h"

float FAIL_PROBABILITY = 0.0;

void* fakeCalloc (size_t nmemb, size_t size)
{
	// printf ("FAIL_PROBABILITY = %f\n", FAIL_PROBABILITY);
	if (rand()%1000000 >= FAIL_PROBABILITY * 1000000)
	{
		return calloc (nmemb, size);
	}
	else
	{
		return 0;
	}
}
