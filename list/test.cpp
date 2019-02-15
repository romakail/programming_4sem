#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

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
void* testIterateFunc (listElement* currentElement, void* params);
int testIterate ();
int testCycle ();
int testElementsCounter ();
// int testDump ();


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
	TEST (testCycle ());
	TEST (testElementsCounter());
	// TEST (testDump ());

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

void* testIterateFunc (listElement* currentElement, void* params)
{
	// static int sum = 0;
	// sum = sum + currentElement->content;
	*(int*) params += currentElement->content;

	return NULL;
}

//------------------------------------------------------------------------------

int testIterate ()
{
	FAIL_PROBABILITY = 0;
	list_T list;

	listElement* elem5 = list.addElementToTail(5);
	listElement* elem1 = list.addElementToTail(1);
	listElement* elem2 = list.addElementToTail(2);
	listElement* elem3 = list.addElementToTail(3);
	listElement* elem4 = list.addElementToTail(4);

	int sum = 0;

	list.iterate (testIterateFunc, &sum);

	if (sum != 1+2+3+4+5)
		return FAIL;

	return SUCCESS;
}

//------------------------------------------------------------------------------

int testCycle ()
{
	FAIL_PROBABILITY = 0;
	list_T list;

	listElement* elem1 = list.addElementToTail(1);
	listElement* elem2 = list.addElementToTail(2);
	listElement* elem3 = list.addElementToTail(3);

	elem3->next = elem1;
	// list.dump ();

	if (list.verification() != CYCLE)
	{
		elem3->next = NULL;
		return FAIL;
	}
	elem3->next = NULL;
	return SUCCESS;
}

//------------------------------------------------------------------------------

int testElementsCounter ()
{
	list_T list;

	listElement* elem1 = list.addElementToTail(1);
	listElement* elem2 = list.addElementToTail(2);
	listElement* elem3 = list.addElementToTail(3);

	if (list.countElements() != 3)
		return FAIL;

	listElement* temp = elem2->next;
	elem2->next = NULL;
	if (list.verification() != WRONG_N_ELEMENTS)
	{
		elem2->next = temp;
		return FAIL;
	}

	elem2->next = temp;

	return SUCCESS;
}

//------------------------------------------------------------------------------

// int testDump ()
// {
// 	int fd = open ("TestDumpOutput", O_RDWR);
// 	if (fd == -1)
// 	{
// 		perror ("\nSomething wrong with open\n");
// 		return FAIL;
// 	}
// 	// if (chmod("TestDumpOutput", 00444) == -1)
// 	// {
// 	// 	perror ("chmod fails\n");
// 	// 	return FAIL;
// 	// }
//
// 	list_T list;
//
// 	listElement* elem1 = list.addElementToTail(1);
// 	listElement* elem2 = list.addElementToTail(2);
// 	listElement* elem3 = list.addElementToTail(3);
//
// 	list.dump (fd);
//
// 	if (close (fd) == -1)
// 	{
// 		perror ("close fails\n");
// 		return FAIL;
// 	}
//
// 	int fd1 = open ("TestDumpOutput", O_RDONLY);
// 	if (fd1 == -1)
// 	{
// 		perror ("\nSomething wrong with open TestDumpOutput\n");
// 		return FAIL;
// 	}
//
// 	int fd2 = open ("testDumpTemplate", O_RDONLY);
// 	if (fd2 == -1)
// 	{
// 		perror ("\nSomething wrong with open testDumpTemplate\n");
// 		return FAIL;
// 	}
//
// 	printf ("====1====\n");
// 	struct stat sb1;
// 	struct stat sb2;
//
// 	if (fstat(fd1, &sb1) == -1)
// 	{
// 		perror("stat");
// 		return FAIL;
// 	}
// 	if (fstat(fd2, &sb2) == -1)
// 	{
// 		perror("stat");
// 		return FAIL;
// 	}
//
// 	if (sb1.st_size != sb2.st_size)
// 	{
// 		printf ("====2====\n");
// 		return FAIL;
// 	}
// 	else
// 	{
// 		printf ("====3====\n");
// 		char* fileContent1 = static_cast <char*> (calloc (sb1.st_size, sizeof(*fileContent1)));
// 		char* fileContent2 = static_cast <char*> (calloc (sb2.st_size, sizeof(*fileContent2)));
//
// 		printf ("====4====\n");
// 		if (read (fd1, fileContent1, sb1.st_size) != sb1.st_size)
// 		{
// 			perror ("write");
// 			return FAIL;
// 		}
// 		if (read (fd2, fileContent2, sb2.st_size) != sb2.st_size)
// 		{
// 			perror ("write");
// 			return FAIL;
// 		}
//
// 		printf ("====5====\n");
// 		if (memcmp (fileContent1, fileContent2, sb1.st_size) != 0)
// 		{
// 			return FAIL;
// 		}
// 	}
//
//
// 	return SUCCESS;
// }








//
