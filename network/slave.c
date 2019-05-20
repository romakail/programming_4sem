#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include "network.h"
#include "integral.h"

const int MAX_COMPUTING_TIME = 200;

int observeConnectionLoss (pthread_t* threadPtr, int skTcp);
void* waitingTcpFailRoutine (void* skPtr);

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
	initSigHandlers ();

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

	pthread_t observingThread;
	int observeConnectionLossRet = observeConnectionLoss (&observingThread, skTcp);
	CHECK (observeConnectionLossRet, "observeConnectionLoss failed\n");

	double result = integral (nThreads, task.startValue, task.finishValue, task.step);

	pthread_kill (observingThread, SIGKILL);

	printf ("Local result : %lg\n", result);

	sendTcpRet = sendTcp(skTcp, &result, sizeof(result), 0);
	CHECK (sendTcpRet, "SendTcpRet failed\n");

	return SUCCESS_RET;
}

//------------------------------------------------------------------------------

int observeConnectionLoss (pthread_t* threadPtr, int skTcp)
{
	printf ("Started observeConnectionLoss\n");
	pthread_attr_t threadAttributes;
	int initRet = pthread_attr_init (&threadAttributes);
	if (initRet != 0)
	{
		perror("Problem with pthread_attr_init\n");
		exit (FAIL_RET);
	}
	printf ("1 skTcp = %d\n", skTcp);

	pthread_create (threadPtr, &threadAttributes, waitingTcpFailRoutine, &skTcp);
	printf ("2\n");
	return SUCCESS_RET;
}

//------------------------------------------------------------------------------

void* waitingTcpFailRoutine (void* skPtr)
{
	printf ("Started Routine\n");
	int skTcp = *((int*)skPtr);
	printf ("skTcp = %d\n", skTcp);

	fd_set set;
	FD_ZERO (&set);
	FD_SET (skTcp, &set);

	struct timeval timeout = {
		.tv_sec     = MAX_COMPUTING_TIME,
		.tv_usec    = 0
	};

	int selectRet = select (skTcp + 1, 0, 0, &set, &timeout);
	if (selectRet == 0)
	{
		printf ("You have been computing for too long on this device\n");
		close (skTcp);
		exit (FAIL_RET);
	}
	if (selectRet == 1)
	{
		printf ("Connection lost\n");
		close (skTcp);
		exit (FAIL_RET);
	}

	int retVal = FAIL_RET;
	pthread_exit (&retVal);
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
