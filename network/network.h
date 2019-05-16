#ifndef network_h
#define network_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SUCCESS_RET 0
#define FAIL_RET    -1

#define UDP_PORT  4000
#define TCP_PORT  4001

#define BROADCASTING_MSG 666
#define WAITING_SECONDS 5

struct task_t
{
	double startValue;
	double finishValue;
	double step;
};

struct slave_t
{
	int socket;
	int number;
	struct sockaddr_in addr;
	socklen_t addrLen;

	int nThreads;
	struct task_t task;
};

int makeUdpBroadcastSocket ();
int makeTcpListeningSocket ();
int makeConnectedTcpSocket (const struct sockaddr_in* hostAddr, const socklen_t* hostAddrLen);

int getSlavesSockets (struct slave_t* slaves, int nSlaves, int skTcp);
int broadcastUdpMsg ();
int getHostAddress (struct sockaddr_in* hostAddr, socklen_t* hostAddrLen);

ssize_t sendTcp (int skTcp, const void* buffer, size_t size, int flags);
ssize_t recvTcp (int skTcp,       void* buffer, size_t size, int flags);

#endif /* network_h */
