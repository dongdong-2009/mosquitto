#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "http.h"

int net_connect(char *IP, int port)
{
	int socked;
	struct sockaddr_in servaddr;

	socklen_t socklen;

	if ((socked = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("[ERR] [Create socket failed].\n");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(IP);
	//servaddr.sin_addr = *((struct in_addr *)host)

	//if (inet_pton(AF_INET, IP, &servaddr.sin_addr) <= 0)
	//{
	//	printf("[ERR] [Covert IP failed].\n");
	//	exit(-1);
	//}

	if (connect(socked, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("[ERR] [Connect Server failed].\n");
		exit(-1);
	}

	printf("[info] [Create TCP connection OK].\n");

	return socked;
}


int net_disconn(int socked)
{
	if (socked < 0)
	{
		printf("[ERR] [Network has disconnected.]\n");
		return -1;
	}

	close(socked);
	return 0;
}

int net_send(int socked, char *buf_send, int len)
{
	int ret = write(socked, buf_send, len);
	if (ret < 0)
	{
		printf("[ERR] [Send data failed].\n");
		return -1;
	}

	return ret;
}

int net_recv(int socked, char *buf_recv, int len)
{
	int ret = read(socked, buf_recv, len);

	if (ret <= 0)
	{
		printf("[ERR] [Recv data failed].\n");
		return -1;
	}

	return ret;
}
