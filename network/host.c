#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "network.h"

int parseNslaves (int argc, char** argv);

int main(int argc, char** argv)
{
	printf ("Host\n");

	int nSlaves = parseNslaves(argc, argv);

	// in_port_t tcpPort = htons(TCP_PORT);
	// in_port_t udpPort = htons(UDP_PORT);
	//
	// int skUdp = socket (PF_INET, SOCK_DGRAM, 0);
	// CHECK (skUdp, "Socket failed\n");
	//
	// int val = 1;
	// int setsockoptRet = setsockopt (skUdp, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
	// CHECK (setsockoptRet, "Setsockopt failed");
	//
	// struct sockaddr_in* addresses = (struct sockaddr_in*) calloc (nSlaves, sizeof(*addresses));
	//
	// for (int i = 0; i < nSlaves; i++)
	// {
	// 	addresses[i].sin_family      = AF_INET;
	// 	addresses[i].sin_port        = udpPort;
	// 	addresses[i].sin_addr.s_addr = INADDR_BROADCAST;
	// }

	// int A = 666;
	// printf ("Before : %o\n", addresses[0].sin_addr.s_addr);


	//===============================Broadcasting===============================

	int broadcastRet = broadcastUdpMsg ();
	CHECK (broadcastRet, "broadcastUdpMsg failed\n");

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
