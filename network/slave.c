#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


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
			exit(-1);												\
		}															\
	}while (0);

const int SUCCESS_RET =  0;
const int FAIL_RET    = -1;

const int TCP_PORT = 3000;
const int UDP_PORT = 3001;

int parseNthreads (int argc, char** argv);

int main (int argc, char** argv)
{
	int nThreads = parseNthreads (argc, argv);
	printf ("Slave, nThreads = %d\n", nThreads);

	in_port_t tcpPort = htons(TCP_PORT);
	in_port_t udpPort = htons(UDP_PORT);

	int sk = socket (PF_INET, SOCK_DGRAM, 0);
	CHECK (sk, "Socket failed\n");

	int val = 1;
	int setsockoptRet = setsockopt (sk, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
	CHECK (setsockoptRet, "Setsockopt failed");


	struct sockaddr_in addr =
	{
		.sin_family = AF_INET,
		.sin_port   = udpPort,
		.sin_addr   = INADDR_BROADCAST
	};

	int bindRet = bind (sk, &addr, sizeof(addr));
	CHECK (bindRet, "Bind failed\n");

	struct sockaddr_in hostAddr;

	int A = 3;

	socklen_t hostAddrLen = sizeof(hostAddr);
	int recvFromRet = recvfrom (sk, &A, sizeof(A), 0, &hostAddr, &hostAddrLen);

	printf ("A = %d\n", A);
	printf ("hostAddr : %o\n", hostAddr.sin_addr.s_addr);

	int skTcp = -1;
	int connectRet = connect (skTcp, &hostAddr, hostAddrLen);
	CHECK (connectRet, "Connect failed\n");


	return 0;
}

//------------------------------------------------------------------------------

int parseNthreads (int argc, char** argv)
{
	if (argc != 2)
	{
		printf ("wrong number of arguments\n");
		exit (-1);
	}

	char* endptr = 0;
	int val = strtol (argv[1], &endptr, 10);

	if (errno == ERANGE)
	{
		perror ("overflow of long int\n");
		exit (FAIL_RET);
	}
	else if (errno == EINVAL)
	{
		perror ("no digits seen\n");
		exit (FAIL_RET);
	}
	else if (val <= 0)
	{
		printf ("less or equal to zero\n");
		exit (FAIL_RET);
	}
	else if (*endptr != '\0')
	{
		printf ("I found not a digit\n");
		exit (FAIL_RET);
	}

	return val;
}
