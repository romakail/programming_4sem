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

int parseNthreads (int argc, char** argv);

int main (int argc, char** argv)
{
	int nThreads = parseNthreads (argc, argv);
	printf ("Slave, nThreads = %d\n", nThreads);

	// in_port_t tcpPort = htons(TCP_PORT);
	// in_port_t udpPort = htons(UDP_PORT);

	// int sk = socket (PF_INET, SOCK_DGRAM, 0);
	// CHECK (sk, "Socket failed\n");
	//
	// int val = 1;
	// int setsockoptRet = setsockopt (sk, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
	// CHECK (setsockoptRet, "Setsockopt failed");
	//
	//
	// struct sockaddr_in addr =
	// {
	// 	.sin_family = AF_INET,
	// 	.sin_port   = udpPort,
	// 	.sin_addr   = INADDR_BROADCAST
	// };
	//
	// int bindRet = bind (sk, &addr, sizeof(addr));
	// CHECK (bindRet, "Bind failed\n");
	//
	// struct sockaddr_in hostAddr;
	//
	// int A = 3;
	//
	// socklen_t hostAddrLen = sizeof(hostAddr);
	// int recvFromRet = recvfrom (sk, &A, sizeof(A), 0, &hostAddr, &hostAddrLen);
	//
	// printf ("A = %d\n", A);
	//
	// int skTcp = -1;
	// int connectRet = connect (skTcp, &hostAddr, hostAddrLen);
	// CHECK (connectRet, "Connect failed\n");

	struct sockaddr_in hostAddr;
	socklen_t hostAddrLen = sizeof (hostAddr);

	int getHostsAddressRet = getHostsAddress (&hostAddr, &hostAddrLen);
	CHECK (getHostsAddressRet, "GetHostsAddress failed\n");
	sleep (1);

	int skTcp = makeConnectedTcpSocket (&hostAddr, &hostAddrLen);
	CHECK (skTcp, "makeConnectedTcpSocket failed\n");

	int sendtoRet = send (skTcp, &nThreads, sizeof(nThreads), 0);
	printf ("Sent %d bytes\n", sendtoRet);

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
