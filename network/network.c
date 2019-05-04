#include "network.h"


int getHostsAddress (struct sockaddr_in* hostAddr, socklen_t* hostAddrLen)
{
	int skUdp = makeUdpBroadcastSocket ();

	printf ("hostPort before : %d", hostAddr->sin_port);

	int msg = -1;

	int recvfromRet = recvfrom (skUdp, &msg, sizeof(msg), 0, (void*)hostAddr, hostAddrLen);
	CHECK (recvfromRet, "recvfrom failed");
	CHECK (msg == BROADCASTING_MSG, "Got wrong message\n");


	printf ("recvfrom returned %d\n", recvfromRet);
	printf ("msg = %d\n", msg);
	printf ("hostAddr : %o\n", hostAddr->sin_addr.s_addr);
	printf ("hostPort after : %d", hostAddr->sin_port);

	hostAddr->sin_port = htons (TCP_PORT);

	return SUCCESS_RET;
}

//-------------------------------------------------------------------------------

int broadcastUdpMsg ()
{
	int skUdp = makeUdpBroadcastSocket ();

	struct sockaddr_in broadcastAddr =
	{
        .sin_family = AF_INET,
        .sin_port   = htons(UDP_PORT),
        .sin_addr   = INADDR_BROADCAST
    };

	int msg = 666;
	int sendtoRet = sendto (skUdp, &msg, sizeof(msg), 0, (void*)&broadcastAddr, sizeof(broadcastAddr));
	CHECK (sendtoRet, "sendto failed\n");

	return SUCCESS_RET;
}

//------------------------------------------------------------------------------

int getSlavesSockets (struct slave_t* slaves, int nSlaves)
{
	return SUCCESS_RET;
}

//------------------------------------------------------------------------------

int makeConnectedTcpSk (const struct sockaddr* hostAddr)
{
	int skTcp = socket (AF_INET, SOCK_STREAM, 0);
	CHECK (skTcp, "socket failed\n");

	socklen_t hostAddrLen = sizeof (*hostAddr);

	printf ("Started connecting\n");
	int connectRet = connect (skTcp, hostAddr, hostAddrLen);
	if (connectRet == -1)
	{
		close (skTcp);
		CHECK (connectRet, "connect failed\n");
	}

	return skTcp;
}

//------------------------------------------------------------------------------

int makeTcpListeningSocket ()
{
	int skTcp = socket (AF_INET, SOCK_STREAM, 0);
	CHECK (skTcp, "socket failed\n");

	// Might have some setsockopt operations

	int listenRet = listen (skTcp, 256);
	if (listenRet == -1)
	{
		close (skTcp);
		CHECK (listenRet, "listen failed\n");
	}

	struct sockaddr_in addr =
	{
		.sin_family      = AF_INET,
		.sin_port        = htons(TCP_PORT),
		.sin_addr        = INADDR_ANY,
	};

	int bindRet = bind (skTcp, (void*)&addr, sizeof (addr));
	if (bindRet == -1)
	{
		close (skTcp);
		CHECK (bindRet, "bind failed\n");
	}

	return skTcp;
}

//------------------------------------------------------------------------------

int makeUdpBroadcastSocket ()
{
	int skUdp = socket (PF_INET, SOCK_DGRAM, 0);
	CHECK (skUdp, "socket failed\n");

	int val = 1;
	int setsockoptRet = setsockopt (skUdp, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
	if (setsockoptRet == -1)
	{
		close (skUdp);
		CHECK (setsockoptRet, "Setsockopt failed\n");
	}

	struct sockaddr_in addr =
	{
		.sin_family      = AF_INET,
		.sin_port        = htons(UDP_PORT),
		.sin_addr        = INADDR_BROADCAST,
	};

	int bindRet = bind (skUdp, (void*)&addr, sizeof (addr));
	if (bindRet == -1)
	{
		close (skUdp);
		CHECK (bindRet, "bind failed\n");
	}

	return skUdp;
}
