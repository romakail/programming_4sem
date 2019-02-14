#include "list.h"

list_T::list_T ()
{
	nElements = 0;

	head = NULL;
	tail = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------

list_T::~list_T ()
{

	listElement* currentElement = head;
	listElement* temp = 0;
	while (currentElement != NULL)
	{
		temp = currentElement->next;
		free (currentElement);
		currentElement = temp;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------

listElement* list_T::addElementToHead (object newElementVal)
{
	// printf ("I have started adding an element\n");
	assert (nElements >= 0);
	if (nElements == 0xffffffff /* UINT_MAX */)
		return NULL;

	listElement* addedElement = static_cast <listElement*> (calloc (1, sizeof(*addedElement)));
	if (addedElement == 0)
	{
		// printf ("Error with allocating memory")
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
		else // if (nElements > 0)
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
	}

	// return NULL;
}

//---------------------------------------------------------------------------------------------------------------------------------

listElement* list_T::addElementToTail (object newElementVal)
{
	// printf ("I have started adding an element\n");
	assert (nElements >= 0);
	if (nElements == 0xffffffff /* UINT_MAX */)
		return NULL;

	listElement* addedElement = static_cast <listElement*> (calloc (1, sizeof(*addedElement)));
	if (addedElement == 0)
	{
		// printf ("Error with allocating memory")
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
		else // if (nElements > 0)
		{
			//printf ("case 0 < nElements < max\n")

			assert (head != NULL);
			assert (tail != NULL);

			tail->next = addedElement;
			addedElement->prev = tail;

			tail = addedElement;
			addedElement->next = NULL;

			addedElement->content = newElementVal;

			nElements++;

			return addedElement;
		}
	}

	// return NULL;
}

//-----------------------------------------------------------------------------------------------------------------

int list_T::deleteElement  (listElement* deletedElement)
{
	// printf ("I have started deleting a certain object from a list\n");
	assert (deletedElement);
	// printf ("nElements = %d\n", nElements);
	//assert (nElements > 0);
	// printf ("Deleting %p\n", deletedElement);

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
	// else
	// {
	// 	printf ("What might have happened  line = %d\n", __LINE__);
	// 	assert (0);
	// }

	free (deletedElement);
	nElements--;

	// printf ("in the end of delete nElements = %d\n", nElements);
	// printf ("I have finished deleting a cerain object\n");
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------

listElement* list_T::findElement (object desiredValue)
{
	listElement* currentElement = head;
	while (currentElement != NULL)
	{
		if (currentElement->content == desiredValue)
		{
			return currentElement;
		}
		currentElement = currentElement->next;
	}
	return 0;
}

//------------------------------------------------------------------------------

int list_T::elementsNumber ()
{
	return nElements;
}

//----------------------------------------------------------------------------------------------------------------------------------------

int list_T::iterate (int (*iteratedFunction) (object))
{
	listElement* currentElement = head;
	while (currentElement != NULL)
	{
		iteratedFunction (currentElement->content);
		currentElement = currentElement->next;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------------------------

// int dumpElement (object num)
// {
// 	return printf ("%d\n", num);
// }

//----------------------------------------------------------------------------------------------------------------------------------------

// int list_T::dump (int outputFd)
// {
// 	dprintf (outputFd, "=====DUMP=====\n");
// 	dprintf (outputFd, "nElements = %d\n", nElements);
// 	dprintf (outputFd, "head = %p\n", head);
// 	dprintf (outputFd, "tail = %p\n\n", tail);
// 	dprintf (outputFd, "--------------\n\n");
//
// 	listElement* currentElement = head;
// 	while (currentElement != NULL)
// 	{
// 		dprintf (outputFd, "this = %p\n", currentElement);
// 		dprintf (outputFd, "val  = %d\n", currentElement->content);
// 		dprintf (outputFd, "prev = %p\n", currentElement->prev);
// 		dprintf (outputFd, "next = %p\n", currentElement->next);
// 		dprintf (outputFd, "\n");
// 		currentElement = currentElement->next;
// 	}
//
// 	dprintf (outputFd, "==============\n");
// 	return 0;
// }

//----------------------------------------------------------------------------------------------------------------------------------------

int list_T::verification ()
{
	if (this->checkCycle() != 0)
		return CYCLE;
	if (nElements != this->countElements())
		return WRONG_N_ELEMENTS;
	return SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------------------------

int list_T::checkCycle ()
{
	listElement* ptr1 = head;
	listElement* ptr2 = head;
	int fail = 0;

	while (fail == 0)
	{
		if (ptr2 == NULL)
			break;
		else
			ptr2 = ptr2->next;

		if (ptr2 == NULL)
			break;
		else
			ptr2 = ptr2->next;

		if (ptr1 != NULL)
			ptr1 = ptr1->next;

		if (ptr1 == ptr2)
			fail = 1;
	}
	return fail;
}

//----------------------------------------------------------------------------------------------------------------------------------------

int list_T::countElements ()
{
	listElement* currentElement = head;
	int counter = 0;
	while (currentElement != NULL)
	{
		counter++;
		currentElement = currentElement->next;
	}
	return counter;
}




















//
