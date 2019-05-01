#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
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

int parseNslaves (int argc, char** argv);

int main(int argc, char** argv)
{
	printf ("Host\n");

	int nSlaves = parseNslaves(argc, argv);

	in_port_t tcpPort = htons(3000);
	in_port_t udpPort = htons(3001);

	int skUdp = socket (PF_INET, SOCK_DGRAM, 0);
	CHECK (skUdp, "Socket failed\n");

	int val = 1;
	int setsockoptRet = setsockopt (skUdp, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
	CHECK (setsockoptRet, "Setsockopt failed");

	struct sockaddr_in* addresses = (struct sockaddr_in*) calloc (nSlaves, sizeof(*addresses));

	for (int i = 0; i < nSlaves; i++)
	{
		addresses[i].sin_family      = AF_INET;
		addresses[i].sin_port        = udpPort;
		addresses[i].sin_addr.s_addr = INADDR_BROADCAST;
	}

	// int A = 666;
	// printf ("Before : %o\n", addresses[0].sin_addr.s_addr);

	for (int i = 0; i < nSlaves; i++)
	{
		int sendRet = sendto (skUdp, &i, sizeof(i), 0, (void*)&(addresses[i]), sizeof(addresses[i]));
		printf ("s_addr = %o\n", addresses[i].sin_addr.s_addr);
	}

	int skTcp = -1;


	return 0;
}

//------------------------------------------------------------------------------

int parseNslaves (int argc, char** argv)
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











//
