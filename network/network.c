#include "network.h"


int getHostsAddress (struct sockaddr_in* hostAddr, socklen_t* hostAddrLen)
{
	int skUdp = makeUdpBroadcastSocket ();

	printf ("hostPort before : %d\n", hostAddr->sin_port);

	int msg = -1;

	int recvfromRet = recvfrom (skUdp, &msg, sizeof(msg), 0, (void*)hostAddr, hostAddrLen);
	CHECK (recvfromRet, "recvfrom failed\n");
	// CHECK (msg == BROADCASTING_MSG, "Got wrong message\n")
	if (msg != BROADCASTING_MSG)
	{
		perror ("Got wrong message\n");
		return FAIL_RET;
	}


	printf ("recvfrom returned %d\n", recvfromRet);
	printf ("msg = %d\n", msg);
	printf ("hostAddr : %o\n", hostAddr->sin_addr.s_addr);
	printf ("hostPort after : %d\n", hostAddr->sin_port);

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

	int msg = BROADCASTING_MSG;
	int sendtoRet = sendto (skUdp, &msg, sizeof(msg), 0, (void*)&broadcastAddr, sizeof(broadcastAddr));
	CHECK (sendtoRet, "sendto failed\n");

	printf ("Finished broadcastUdpMsg\n");
	return SUCCESS_RET;
}

//------------------------------------------------------------------------------

int getSlavesSockets (struct slave_t* slaves, int nSlaves, int skTcp)
{
	fd_set set;
	FD_ZERO (&set);
	FD_SET  (skTcp, &set);

	struct timeval timeout =
	{
		.tv_sec  = WAITING_SECONDS,
		.tv_usec = 0,
	};

	int i = 0;
	int selectRet = 0;
	int acceptRet = 0;
	for (i = 0; i < nSlaves; i++)
	{
		selectRet = select (skTcp + 1, &set, 0, 0, &timeout);
		if (selectRet == 0)
		{
			perror ("Select's time has ran out\n");
			close (skTcp);
			return (FAIL_RET);
		}
		else
		{
			acceptRet = accept (skTcp, &(slaves[i].addr), &(slaves[i].addrLen));
			if (acceptRet == -1)
			{
				close (skTcp);
				CHECK (acceptRet, "accept failed\n");
			}
			slaves[i].number = i;
			slaves[i].socket = acceptRet;
		}
	}

	return SUCCESS_RET;
}

//------------------------------------------------------------------------------

int makeConnectedTcpSocket (const struct sockaddr* hostAddr, const socklen_t* hostAddrLen)
{
	int skTcp = socket (AF_INET, SOCK_STREAM, 0);
	CHECK (skTcp, "socket failed\n");

	printf ("Started connecting\n");
	int connectRet = connect (skTcp, hostAddr, *hostAddrLen);
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

	int listenRet = listen (skTcp, 256);
	if (listenRet == -1)
	{
		close (skTcp);
		CHECK (listenRet, "listen failed\n");
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

	printf ("Made an UdpBroadcastSocket %d\n", skUdp);
	return skUdp;
}
