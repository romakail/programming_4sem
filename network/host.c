#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "network.h"

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

const double START_VAL  = 1.0;
const double FINISH_VAL = 10.0;
const double STEP       = 1E-9;



int parseNslaves (int argc, char** argv);

int main(int argc, char** argv)
{
	printf ("Host\n");

	int nSlaves = parseNslaves(argc, argv);

	int skTcp = makeTcpListeningSocket ();
	CHECK (skTcp, "makeTcpListeningSocket failed\n");

	int broadcastRet = broadcastUdpMsg ();
	CHECK (broadcastRet, "broadcastUdpMsg failed\n");

	struct slave_t* slaves = (struct slave_t*) calloc (nSlaves, sizeof(&slaves));
	if (slaves == 0)
	{
		perror ("calloc failed");
		return FAIL_RET;
	}

	int getSlavesSocketsRet = getSlavesSockets (slaves, nSlaves, skTcp);
	CHECK (getSlavesSocketsRet, "getSlavesSockets failed\n");

	// int recvRet = recv (slaves[0].socket, &(slaves[0].nThreads), sizeof(slaves[0].nThreads), 0);
	// printf ("recv returned %d\n", recvRet);
	// printf ("Slave's no %d nThreads = %d\n", slaves[0].number, slaves[0].nThreads);

	for (int i = 0; i < nSlaves; i++)
	{
		int recvRet = recvTcp (slaves[i].socket, &(slaves[i].nThreads), slaves[i].addrLen, 0);
		CHECK (recvRet, "RecvTcp failed\n");
		printf ("Slave no [%d] has %d threads\n", slaves[i].number, slaves[i].nThreads);
	}

	



	free (slaves);
	return SUCCESS_RET;
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
