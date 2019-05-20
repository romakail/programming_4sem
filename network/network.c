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
			return FAIL_RET;										\
		}															\
	}while (0);

#include "network.h"

int getHostAddress (struct sockaddr_in* hostAddr, socklen_t* hostAddrLen)
{
	int skUdp = makeUdpBroadcastSocket ();

	// printf ("hostPort before : %d\n", hostAddr->sin_port);

	int msg = -1;

	// printf ("---------//---------\n");
	// printf ("skUdp = %d\n"          , skUdp);
	// printf ("&msg  = %p\n"          , &msg);
	// printf ("sizeof(msg)  = %ld\n"  , sizeof(msg));
	// printf ("hostAddr = %d\n", (*hostAddr).sin_addr.s_addr);
	// printf ("hostAddr = %o\n", (*hostAddr).sin_addr.s_addr);
	// printf ("hostAddrLen = %p\n"    , hostAddrLen);

	int recvfromRet = recvfrom (skUdp, &msg, sizeof(msg), 0, (void*)hostAddr, hostAddrLen);
	CHECK (recvfromRet, "recvfrom failed\n");

	if (msg != BROADCASTING_MSG)
	{
		perror ("Got wrong message\n");
		return FAIL_RET;
	}


	// printf ("recvfrom returned %d\n", recvfromRet);
	printf ("msg = %d\n", msg);
	// printf ("hostAddr = %o\n", (*hostAddr).sin_addr.s_addr);
	// printf ("hostAddr : %o\n", hostAddr->sin_addr.s_addr);
	// printf ("hostPort after : %d\n", hostAddr->sin_port);

	// hostAddr->sin_port = htons (TCP_PORT);

	hostAddr->sin_port = htons(TCP_PORT);

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
			acceptRet = accept (skTcp, (void*)&(slaves[i].addr), &(slaves[i].addrLen));
			printf ("AddrLen = %d\n", addrLen);
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

	int enableKeepAliveRet = enableKeepAlive (skTcp);
	CHECK (enableKeepAliveRet, "enableKeepAlive failed\n");

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
			printf ("connection error [%d] : %s\n", errval, strerror(errval));
			return FAIL_RET;
		}
	}
	fcntlRet = fcntl (skTcp, F_SETFL, 0);
	if (fcntlRet == -1)
	{
		close (skTcp);
		CHECK (fcntlRet, "Fcntl failed\n");
	}

	int setOwnerRet = setOwner (skTcp);
	CHECK (setOwnerRet, "setOwner failed\n");

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
		CHECK (setsockoptRet, "Setsockopt with SO_REUSEADR failed\n");
	}

	int enableKeepAliveRet = enableKeepAlive (skTcp);
	CHECK (enableKeepAliveRet, "enableKeepAlive failed\n");

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

//------------------------------------------------------------------------------

ssize_t sendTcp (int skTcp, const void* buffer, size_t size, int flags)
{
	ssize_t sendRet = send (skTcp, buffer, size, flags);
	if (sendRet == -1)
	{
		close (skTcp);
		perror ("Send failed\n");
		return FAIL_RET;
	}
	if (size != sendRet)
	{
		close (skTcp);
		printf ("Not everything was sent\n");
		printf ("Expected [%ld] but sent [%ld]\n", size, sendRet);
		return FAIL_RET;
	}

	return SUCCESS_RET;
}

//------------------------------------------------------------------------------

ssize_t recvTcp (int skTcp, void* buffer, size_t size, int flags)
{
	ssize_t recvRet = recv (skTcp, buffer, size, flags);
	if (recvRet == -1)
	{
		close (skTcp);
		perror ("Send failed\n");
		return FAIL_RET;
	}
	if (size != recvRet)
	{
		printf ("Not everything was received\n");
		printf ("Expected [%ld] but got [%ld]\n", size, recvRet);
		return FAIL_RET;
	}
	return SUCCESS_RET;
}

//------------------------------------------------------------------------------

int enableKeepAlive (int skTcp)
{
	int kaVal = 1;
	int setsockoptRet = setsockopt (skTcp, SOL_SOCKET, SO_KEEPALIVE, &kaVal, sizeof(kaVal));
	if (setsockoptRet == -1)
	{
		close (skTcp);
		CHECK (setsockoptRet, "Setsockopt with SO_KEEPALIVE failed\n");
	}

	int cntVal = 1;
	setsockoptRet = setsockopt (skTcp, IPPROTO_TCP, TCP_KEEPCNT, &cntVal, sizeof(cntVal));
	if (setsockoptRet == -1)
	{
		close (skTcp);
		CHECK (setsockoptRet, "Setsockopt with TCP_KEEPCNT failed\n");
	}

	int idleVal = 1;
	setsockoptRet = setsockopt (skTcp, IPPROTO_TCP, TCP_KEEPIDLE, &idleVal, sizeof(idleVal));
	if (setsockoptRet == -1)
	{
		close (skTcp);
		CHECK (setsockoptRet, "Setsockopt with TCP_KEEPIDLE failed\n");
	}

	int intvlVal = 1;
	setsockoptRet = setsockopt (skTcp, IPPROTO_TCP, TCP_KEEPINTVL, &intvlVal, sizeof(intvlVal));
	if (setsockoptRet == -1)
	{
		close (skTcp);
		CHECK (setsockoptRet, "Setsockopt with TCP_KEEPINTVL failed\n");
	}
	return SUCCESS_RET;
}

//------------------------------------------------------------------------------

int initSigHandlers ()
{
	struct sigaction actionIgnore = {};
    actionIgnore.sa_handler = SIG_IGN;

    int sigActRet = sigaction (SIGPIPE, &actionIgnore, 0);
    CHECK (sigActRet, "sigaction failed\n");

	sigActRet = sigaction (SIGURG, &actionIgnore, 0);
	CHECK (sigActRet, "sigaction failed\n");

	struct sigaction actionTerminate = {};
    actionTerminate.sa_handler = sigIOHandler;

	sigActRet = sigaction (SIGIO, &actionTerminate, 0);
	CHECK (sigActRet, "sigaction failed\n");

    return SUCCESS_RET;
}

//------------------------------------------------------------------------------

int setOwner (int skTcp)
{
	struct f_owner_ex self =
	{
		.type = F_OWNER_PID,
		.pid = getpid()
	};

	int fcntlRet = fcntl (skTcp, F_SETOWN_EX, &self);
	CHECK (fcntlRet, "fcntl failed\n");

	return SUCCESS_RET;
}


//------------------------------------------------------------------------------

void sigIOHandler (int signal)
{
    printf ("I am sigIO and I am terminationg this process because of timeout\n");
	exit (FAIL_RET);
}
















//
