#include "list.h"

int main()
{
	list_T list1;

	list1.addElement (5);
	list1.addElement (10);
	list1.iterate (dumpElement);
	return 0;
}
