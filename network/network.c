#include "network.h"


int getHostAddress (struct sockaddr_in* hostAddr, socklen_t* hostAddrLen)
{
	int skUdp = makeUdpBroadcastSocket ();

	// printf ("hostPort before : %d\n", hostAddr->sin_port);

	int msg = -1;

	printf ("---------//---------\n");
	printf ("skUdp = %d\n"          , skUdp);
	printf ("&msg  = %p\n"          , &msg);
	printf ("sizeof(msg)  = %ld\n"  , sizeof(msg));
	printf ("hostAddr = %d\n", (*hostAddr).sin_addr.s_addr);
	printf ("hostAddr = %o\n", (*hostAddr).sin_addr.s_addr);
	printf ("hostAddrLen = %p\n"    , hostAddrLen);

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
	printf ("hostAddr = %o\n", (*hostAddr).sin_addr.s_addr);
	// printf ("hostAddr : %o\n", hostAddr->sin_addr.s_addr);
	// printf ("hostPort after : %d\n", hostAddr->sin_port);

	// hostAddr->sin_port = htons (TCP_PORT);

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
			struct sockaddr addr;
			socklen_t addrLen = sizeof (addr);
			acceptRet = accept (skTcp, (void*)&addr, &addrLen);
			// printf ("Address = %o\n", addr.sin_addr.s_addr);
			printf ("AddrLen = %d\n", addrLen);
			if (acceptRet == -1)
			{
				close (skTcp);
				CHECK (acceptRet, "accept failed\n");
			}
			slaves[i].number = i;
			slaves[i].socket = acceptRet;
			// slaves[i].addr = addr;
			slaves[i].addrLen = addrLen;
		}
	}

	return SUCCESS_RET;
}

//------------------------------------------------------------------------------

// int makeConnectedTcpSocket (const struct sockaddr_in* hostAddr, const socklen_t* hostAddrLen)
// {
// 	int skTcp = socket (AF_INET, SOCK_STREAM, 0);
// 	CHECK (skTcp, "socket failed\n");
//
// 	int ruaVal = 1;
// 	int setsockoptRet = setsockopt (skTcp, SOL_SOCKET, SO_REUSEADDR, &ruaVal, sizeof(ruaVal));
// 	if (setsockoptRet == -1)
// 	{
// 		close (skTcp);
// 		CHECK (setsockoptRet, "Setsockopt failed\n");
// 	}
//
// 	printf ("Started connecting\n");
// 	int connectRet = connect (skTcp, (void*)hostAddr, *hostAddrLen);
// 	if (errno == EINPROGRESS)
// 	{
// 		printf ("Errno in progress\n");
// 	}
// 	if (connectRet == -1)
// 	{
// 		close (skTcp);
// 		CHECK (connectRet, "connect failed\n");
// 	}
//
// 	printf ("Returning from make makeConnectedTcpSocket\n");
// 	return skTcp;
// }

//------------------------------------------------------------------------------

int makeConnectedTcpSocket (const struct sockaddr_in* hostAddr, const socklen_t* hostAddrLen)
{
	int skTcp = socket (AF_INET, SOCK_STREAM, 0);
	CHECK (skTcp, "socket failed\n");

	int ruaVal = 1;
	int setsockoptRet = setsockopt (skTcp, SOL_SOCKET, SO_REUSEADDR, &ruaVal, sizeof(ruaVal));
	if (setsockoptRet == -1)
	{
		close (skTcp);
		CHECK (setsockoptRet, "Setsockopt failed\n");
	}

	int fcntlRet = fcntl (skTcp, F_SETFL, O_NONBLOCK);
	if (fcntlRet == -1)
	{
		close (skTcp);
		CHECK (fcntlRet, "Fcntl failed\n");
	}

	fd_set set;
	FD_ZERO(&set);
	FD_SET(skTcp, &set);

	struct timeval timeout =
	{
		.tv_sec  = WAITING_SECONDS,
		.tv_usec = 0,
	};

	printf ("Started connecting\n");
	int connectRet = connect (skTcp, (void*)hostAddr, *hostAddrLen);

	if ((connectRet == -1) && (errno = EINPROGRESS))
	{
		errno = 0;
		int selectRet = select(skTcp + 1, 0, &set, 0, &timeout);
		if (selectRet == 0)
		{
			close (skTcp);
			printf ("Select timed out\n");
			return FAIL_RET;
		}
		if (selectRet == -1)
		{
			close (skTcp);
			perror ("Select failed\n");
			return FAIL_RET;
		}

		int errval = 0;
		socklen_t len = sizeof(errval);
		if(getsockopt(skTcp, SOL_SOCKET, SO_ERROR, &errval, &len) < 0)
		{
			close (skTcp);
			perror ("Getsockopt failed\n");
			return FAIL_RET;
		}

		if (errval != 0)
		{
			close(skTcp);
			printf ("connection error [%d] : %s", errval, strerror(errval));
			return FAIL_RET;
		}
	}
	fcntlRet = fcntl (skTcp, F_SETFL, 0);
	if (fcntlRet == -1)
	{
		close (skTcp);
		CHECK (fcntlRet, "Fcntl failed\n");
	}

	printf ("Returning from make makeConnectedTcpSocket\n");
	return skTcp;
}

//------------------------------------------------------------------------------

int makeTcpListeningSocket ()
{
	int skTcp = socket (AF_INET, SOCK_STREAM, 0);
	CHECK (skTcp, "socket failed\n");

	// Might have some setsockopt operations

	int ruaVal = 1;
	int setsockoptRet = setsockopt (skTcp, SOL_SOCKET, SO_REUSEADDR, &ruaVal, sizeof(ruaVal));
	if (setsockoptRet == -1)
	{
		close (skTcp);
		CHECK (setsockoptRet, "Setsockopt failed\n");
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

	int listenRet = listen (skTcp, 256);
	if (listenRet == -1)
	{
		close (skTcp);
		CHECK (listenRet, "listen failed\n");
	}

	printf ("Now we have a listening socket %d\n", skTcp);
	return skTcp;
}

//------------------------------------------------------------------------------

int makeUdpBroadcastSocket ()
{
	int skUdp = socket (PF_INET, SOCK_DGRAM, 0);
	CHECK (skUdp, "socket failed\n");

	int brcVal = 1;
	int setsockoptRet = setsockopt (skUdp, SOL_SOCKET, SO_BROADCAST, &brcVal, sizeof(brcVal));
	if (setsockoptRet == -1)
	{
		close (skUdp);
		CHECK (setsockoptRet, "Setsockopt failed\n");
	}

	int ruaVal = 1;
	setsockoptRet = setsockopt (skUdp, SOL_SOCKET, SO_REUSEADDR, &ruaVal, sizeof(ruaVal));
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
