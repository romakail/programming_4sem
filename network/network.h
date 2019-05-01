#ifndef network_h
#define network_h

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SUCCESS_RET 0
#define FAIL_RET    -1

#define TCP_PORT  3000
#define UDP_PORT  3001

#define PRINT(args...) 		    \
	do 							\
	{							\
		printf(args);           \
		fflush (stdout);        \
	}while (0);


#define CHECK(what, message)										\
	do 																\
	{																\
		if (what == -1)												\
		{															\
			printf ("Error, line = %d\n", __LINE__);				\
			perror (message);										\
			exit(FAIL_RET);											\
		}															\
	}while (0);

int makeUdpBroadcastSocket ();


#endif /* network_h */
