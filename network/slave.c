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

#include "network.h"
#include "integral.h"

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

int parseNthreads (int argc, char** argv);

int main (int argc, char** argv)
{
	int nThreads = parseNthreads (argc, argv);
	printf ("Slave, nThreads = %d\n", nThreads);
	initSigHandlers ()

	struct sockaddr_in hostAddr;
	socklen_t hostAddrLen = sizeof (hostAddr);

	int getHostAddressRet = getHostAddress (&hostAddr, &hostAddrLen);
	CHECK (getHostAddressRet, "getHostAddress failed\n");

	int skTcp = makeConnectedTcpSocket (&hostAddr, &hostAddrLen);
	CHECK (skTcp, "makeConnectedTcpSocket failed\n");

	// int sendtoRet = send (skTcp, &nThreads, sizeof(nThreads), 0);
	// printf ("Sent %d bytes\n", sendtoRet);

	int sendTcpRet = sendTcp (skTcp, &nThreads, sizeof(nThreads), 0);
	CHECK (sendTcpRet, "SendTcp failed\n");

	struct task_t task;
	int recvTcpRet = recvTcp (skTcp, &task, sizeof(task), 0);
	CHECK (recvTcpRet, "RecvTcp failed\n");

	printf ("========Task========\n");
	printf ("Start  : %lg\n", task.startValue );
	printf ("Finish : %lg\n", task.finishValue);
	printf ("Step   : %lg\n", task.step       );

	double result = integral (nThreads, task.startValue, task.finishValue, task.step);

	printf ("Local result : %lg\n", result);

	sendTcpRet = sendTcp(skTcp, &result, sizeof(result), 0);
	CHECK (sendTcpRet, "SendTcpRet failed\n");

	return SUCCESS_RET;
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
