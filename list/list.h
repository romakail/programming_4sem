#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define PRINT(args...)      \
	printf(args);           \
	fflush (stdout);

typedef int object;

#ifdef TEST
#	include "fakeCalloc.h"
#	define calloc(args...) fakeCalloc(args)
#endif

// extern float FAIL_PROBABILITY;
const int SUCCESS = 0;
const int FAIL = 1;

enum ERROR_CODE
{
	NORM,
	CYCLE,
	WRONG_N_ELEMENTS,
};

struct listElement
{
	listElement* next;
	listElement* prev;
	object content;
};

class list_T
{
	private:
		unsigned int nElements;

		listElement* head;
		listElement* tail;

	public:
		//Fundamental functions
		list_T  ();
		~list_T ();
		listElement* addElementToHead (object      newElement);
		listElement* addElementToTail (object      newElement);
		listElement* findElement      (object desiredValue);
		int          deleteElement    (listElement* deletedElement);
		int          elementsNumber ();
		int iterate (int (*iteratedFunction) (object));

		int verification ();
		int checkCycle   ();
		int countElements();

		// int dump (int outputFd);
};

void* fakeCalloc (size_t nmemb, size_t size);

#endif // LIST_H
