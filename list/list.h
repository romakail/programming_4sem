#include <stdio.h>
#include <stdlib.h>

#define PRINT(args...)      \
	printf(args);           \
	fflush (stdout);        \

typedef int object;

static const float FAIL_PROBABILITY = 0;

struct listElement
{
	listElement* next;
	listElement* prev;
	object content;
};

class list_T
{
	private:
		int nElements;

		listElement* head;
		listElement* tail;

	public:
		//Fundamental functions
		list_T  ();
		~list_T ();
		listElement* addElement  (object      newElement);
		int       deleteElement  (listElement* deletedElement);

		int iterate (int (*iteratedFunction) (object));
		int dump ();
		//For dump:
		// int dump ();
		// int dumpAllElements();
		// int calculateNElements();
};

void* fakeCalloc (size_t nmemb, size_t size);
int dumpElement (object num);
