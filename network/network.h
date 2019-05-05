#ifndef network_h
#define network_h

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SUCCESS_RET 0
#define FAIL_RET    -1

#define UDP_PORT  4000
#define TCP_PORT  4001

#define BROADCASTING_MSG 666
#define WAITING_SECONDS 5


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
			return FAIL_RET;										\
		}															\
	}while (0);


struct slave_t
{
	int socket;
	int number;
	struct sockaddr addr;
	socklen_t addrLen;

	int nThreads;
};


int makeUdpBroadcastSocket ();
int makeTcpListeningSocket ();
int makeConnectedTcpSocket (const struct sockaddr* hostAddr, const socklen_t* hostAddrLen);


int getSlavesSockets (struct slave_t* slaves, int nSlaves, int skTcp);
int broadcastUdpMsg ();
int getHostsAddress (struct sockaddr* hostAddr, socklen_t* hostAddrLen);

#endif /* network_h */
