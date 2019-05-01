#include "network.h"

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
