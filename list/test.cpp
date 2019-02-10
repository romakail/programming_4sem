#include "list.h"

int testList ();

int main()
{
	return testList ();
}

int testList ()
{
	list_T list1;

	list1.addElement (1);
	list1.addElement (2);
	list1.addElement (3);
	list1.addElement (4);

	list1.dump ();

	return 0;
}
