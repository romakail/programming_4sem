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
	if (head != NULL)
	{
		listElement* currentElement = head;
		listElement* temp = head;
		do
		{
			temp = currentElement->next;
			free (currentElement);
			currentElement = temp;
		} while (currentElement != 0);
	}
}

//---------------------------------------------------------------------------------------------------------------------------------

listElement* list_T::addElement (object newElementVal)
{
	// printf ("I have started adding an element\n");
	assert (nElements >= 0);

	listElement* addedElement = static_cast <listElement*> (fakeCalloc (1, sizeof(*addedElement)));
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
		else if (nElements > 0)
		{
			//printf ("case 0 < nElements < max\n")

			assert (head != NULL);
			assert (tail != NULL);

			head->prev = addedElement;
			addedElement->next = head;

			head = addedElement;
			addedElement->prev = NULL;

			addedElement->content = newElementVal;

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

int list_T::deleteElement  (listElement* deletedElement)
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

int list_T::iterate (int (*iteratedFunction) (object))
{
	if (head != NULL)
	{
		listElement* currentElement = head;
		do
		{
			iteratedFunction (currentElement->content);
			currentElement = currentElement->next;
		} while (currentElement != 0);
	}

	return 0;
}

//--------------------------------------------------------------------------------------------------------------

int dumpElement (object num)
{
	return printf ("%d\n", num);
}

//----------------------------------------------------------------------------------------------------------------------------------------

int list_T::dump ()
{
	printf ("=====DUMP=====\n");
	printf ("nElements = %d\n", nElements);
	printf ("head = %p\n", head);
	printf ("tail = %p\n\n", tail);
	printf ("--------------\n\n");

	if (head != NULL)
	{
		listElement* currentElement = head;
		do
		{
			printf ("this = %p\n", currentElement);
			printf ("val  = %d\n", currentElement->content);
			printf ("prev = %p\n", currentElement->prev);
			printf ("next = %p\n", currentElement->next);
			printf ("\n");
			currentElement = currentElement->next;
		} while (currentElement != 0);
	}

	printf ("==============\n");
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------
