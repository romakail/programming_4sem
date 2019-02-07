#include "list.h"

void* fakeCalloc (size_t nmemb, size_t size)
{
	if (rand()%1000000 >= FAIL_PROBABILITY * 1000000)
	{
		return calloc (nmemb, size);
	}
	else
	{
		return 0;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------

list_T::list_T ()
{
	nElements = 0;

	head = NULL;
	tail = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------

list_T::~list_T ()
{
	// Will be filled later
}

//---------------------------------------------------------------------------------------------------------------------------------

template <class object>
listElement* slist_T<object>::addElement (object newElementVal)
{
	// printf ("I have started adding an element\n");
	assert (nElements >= 0);

	listElement* addedElement = fakeCalloc (1, sizeof(*addedElement));
	if (addedElement == 0)
	{
		PRINT ("Error with allocating memory")
		return 0;
	}
	else
	{
		if (nElements == 0)
		{
			//printf ("case nElements == 0\n");
			assert (head == NULL);
			assert (tail == NULL);

			head = addedElement;
			tail = addedElement;

			addedElement->next = NULL;
			addedElement->prev = NULL;
			addedElement->content = newElementVal;

			nElements++;

			return addedElement;
		}
		else if ((nElements > 0) && (nElements < maxElements))
		{
			//printf ("case 0 < nElements < max\n")

			assert (head != NULL);
			assert (tail != NULL);

			head->prev = addedElement;
			addedElement->next = head;

			head = addedElement;
			addedElement->prev = NULL

			nElements++;

			return addedElement;
		}
		else
		{
			printf ("something went wrong line = %d\n", __LINE__);
			exit (0);
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------------------------------------------

int list_T::deleteElement (object* deletedElement)
{
	// printf ("I have started deleting a certain object from a list\n");
	assert (deletedElement);
	// printf ("nElements = %d\n", nElements);
	//assert (nElements > 0);

	if      ((deletedElement->next != NULL) && (deletedElement->prev != NULL))
	{
		deletedElement->prev->next = deletedElement->next;
		deletedElement->next->prev = deletedElement->prev;
	}
	else if ((deletedElement->next == NULL) && (deletedElement->prev != NULL))
	{
		deletedElement->prev->next = NULL;
		tail = deletedElement->prev;
	}
	else if ((deletedElement->next != NULL) && (deletedElement->prev == NULL))
	{
		deletedElement->next->prev = NULL;
		head = deletedElement->next;
	}
	else if ((deletedElement->next == NULL) && (deletedElement->prev == NULL))
	{
		head = deletedElement->next;
		tail = deletedElement->prev;
	}
	else
	{
		printf ("What might have happened  line = %d\n", __LINE__);
		assert (0);
	}

	nElements--;

	// printf ("in the end of delete nElements = %d\n", nElements);
	// printf ("I have finished deleting a cerain object\n");
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------

int iterate (int (*iteratedFunction) (object));
{
	if (head != NULL)
	{
		listElement* currentElement = head;
		do
		{
			iteratedFunction (currentElement->content);
		} while (currentElement->next != 0)
	}

	return 0;
}

//--------------------------------------------------------------------------------------------------------------
int dumpElement (object num)
{
	return printf ("%d\n");
}


//----------------------------------------------------------------------------------------------------------------------------------------
//
// template <class object>
// int list_T<object>::dump()
// {
// 	printf ("maxElements = %d, nElements = %d\n", maxElements, nElements);
// 	printf ("calculatednElements = %d\n", this->calculateNElements());
// 	printf ("head =     %p\n", head);
// 	printf ("tail =     %p\n", tail);
// 	printf ("freeHead = %p\n", freeHead);
// 	printf ("freeTail = %p\n", freeTail);
// 	if (freeTail != NULL)
// 		printf ("freeTail->next = %p\n", freeTail->next);
// 	//printf ("//------------------------------------------//\n");
// 	//printf ("")
//
// 	return 0;
// }
//
// //-------------------------------------------------------------------------------------------------------------------------------
//
// template <class object>
// int list_T<object>::calculateNElements ()
// {
// 	int nElems = 0;
// 	object* checkedElement = head;
//
// 	if (head != NULL)
// 	{
// 		while (checkedElement != NULL)
// 		{
// 			nElems++;
// 			checkedElement = checkedElement->next;
// 		}
// 	}
// 	printf ("ActivatedElements = %d\n", nElems);
//
// 	int nFreeElems = 0;
// 	if (freeHead != NULL)
// 	{
// 		checkedElement = freeHead;
// 		while (checkedElement != NULL)
// 		{
// 			nElems++;
// 			nFreeElems++;
// 			checkedElement = checkedElement->next;
// 		}
// 	}
// 	printf ("FreeElements = %d\n", nFreeElems);
// 	return nElems;
// }
//
// //---------------------------------------------------------------------------------------------------
//
// template <class object>
// int list_T<object>::dumpAllElements()
// {
// 	printf ("ACTIVE ELEMENTS:\n");
//
// 	object* dumpedElement = head;
// 	while (dumpedElement != NULL)
// 	{
// 		printf ("prev = %p\n", dumpedElement->prev);
// 		printf ("this = %p\n", dumpedElement);
// 		printf ("next = %p\n\n", dumpedElement->next);
// 		dumpedElement = dumpedElement->next;
// 	}
// 	printf ("FREE ELEMENTS:\n");
// 	dumpedElement = freeHead;
//
// 	while (dumpedElement != NULL)
// 	{
// 		printf ("prev = %p\n", dumpedElement->prev);
// 		printf ("this = %p\n", dumpedElement);
// 		printf ("next = %p\n\n", dumpedElement->next);
// 		dumpedElement = dumpedElement->next;
// 	}
// 	printf ("================================================================\n");
// 	return 0;
// }



















































/*
template<>
int list_T<zombie>::printfSmth ()
{
	printf ("cheeeeeeee!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1\n");
	return 0;
}
*/
