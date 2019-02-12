
#include "list.h"
#include "fakeCalloc.h"

#define NRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"



#define TEST(function)							\
	printf ("Test no %d: ", counter++);			\
	if (function == SUCCESS)					\
		printf (GRN "SUCCESS" NRM "\n");		\
	else										\
		printf (RED "FAIL" NRM "\n");


int testList ();
int testDestruction ();
int testDeleting ();
int testAllocateProblem ();
int testSearch ();
int testIterateFunc (object elemContent);
int testIterate ();

int main()
{
	// FAIL_PROBABILITY = 1;
	return testList ();
}



//------------------------------------------------------------------------------

int testList ()
{
	FAIL_PROBABILITY = 0;
	int counter = 0;
	printf ("==========TESTS==========\n");

	TEST (testDestruction ());
	TEST (testDeleting ());
	TEST (testAllocateProblem ());
	TEST (testSearch ());
	TEST (testIterate ());

	printf ("=========================\n");
	return 0;
}

//------------------------------------------------------------------------------

int testDestruction ()
{
	FAIL_PROBABILITY = 0;
	list_T list;
	list.addElementToTail (1);
	list.addElementToTail (2);
	list.addElementToTail (3);
	list.addElementToTail (4);
	list.addElementToTail (5);

	return SUCCESS;
}

//------------------------------------------------------------------------------

int testDeleting ()
{
	FAIL_PROBABILITY = 0;
	list_T list;
	listElement* elem2 = list.addElementToHead (2);
	listElement* elem1 = list.addElementToHead (1);
	listElement* elem3 = list.addElementToTail (3);
	listElement* elem4 = list.addElementToTail (4);
	listElement* elem5 = list.addElementToTail (5);

	if (list.elementsNumber() != 5)
		return FAIL;
	if (list.verification() != SUCCESS)
		return FAIL;


	list.deleteElement (elem1);    // deleting element in the head

	if (list.elementsNumber() != 4)
		return FAIL;
	if (list.verification() != SUCCESS)
		return FAIL;

	list.deleteElement (elem5);    // deleting element in the tail

	if (list.elementsNumber() != 3)
		return FAIL;
	if (list.verification() != SUCCESS)
		return FAIL;

	list.deleteElement (elem3);    // deleting element in the middle

	if (list.elementsNumber() != 2)
		return FAIL;
	if (list.verification() != SUCCESS)
		return FAIL;

	list.deleteElement (elem4);

	if (list.elementsNumber() != 1)
		return FAIL;
	if (list.verification() != SUCCESS)
		return FAIL;

	list.deleteElement (elem2);    // deleting final element

	if (list.elementsNumber() != 0)
		return FAIL;
	if (list.verification() != SUCCESS)
		return FAIL;

	return SUCCESS;
}

//------------------------------------------------------------------------------

int testAllocateProblem ()
{

	list_T list;

	listElement* elemPtr = list.addElementToHead(1);
	if (elemPtr == NULL)
		return FAIL;

	FAIL_PROBABILITY = 1;

	elemPtr = list.addElementToHead (2);
	if (elemPtr != 0)
	{
		FAIL_PROBABILITY = 0;
		return FAIL;
	}
	if (list.elementsNumber() != 1)
	{
		FAIL_PROBABILITY = 0;
		return FAIL;
	}

	elemPtr = list.addElementToTail (2);
	if (elemPtr != 0)
	{
		FAIL_PROBABILITY = 0;
		return FAIL;
	}
	if (list.elementsNumber() != 1)
	{
		FAIL_PROBABILITY = 0;
		return FAIL;
	}

	FAIL_PROBABILITY = 0;
	return SUCCESS;
}

//------------------------------------------------------------------------------

int testSearch ()
{
	FAIL_PROBABILITY = 0;
	list_T list;

	listElement* elem1 = list.addElementToTail(1);
	listElement* elem2 = list.addElementToTail(2);
	listElement* elem3 = list.addElementToTail(3);
	listElement* elem100 = list.addElementToTail(100);
	listElement* elem1000 = list.addElementToTail(1000);

	if (list.findElement(1) != elem1)
		return FAIL;
	if (list.findElement(3) != elem3)
		return FAIL;
	if (list.findElement(1000) != elem1000)
		return FAIL;
	if (list.findElement(500) != NULL)
		return FAIL;
	if (list.findElement(5000) != NULL)
		return FAIL;
	if (list.findElement(4) != NULL)
		return FAIL;

	return SUCCESS;
}

//------------------------------------------------------------------------------

int testIterateFunc (object elemContent)
{
	static int sum = 0;
	sum = sum + elemContent;
	return sum;
}

//------------------------------------------------------------------------------

int testIterate ()
{
	FAIL_PROBABILITY = 0;
	list_T list;

	listElement* elem1 = list.addElementToTail(1);
	listElement* elem2 = list.addElementToTail(2);
	listElement* elem3 = list.addElementToTail(3);
	listElement* elem4 = list.addElementToTail(4);
	listElement* elem5 = list.addElementToTail(5);

	list.iterate (testIterateFunc);

	if (testIterateFunc (0) != 1+2+3+4+5)
		return FAIL;

	return SUCCESS;
}









//
