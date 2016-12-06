#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mqtt/mqtt_broker.h"

/*
 * mosquitto broker test demo
 */
int main(int agrc,char **argv)
{
	int ret;

	system("clear");
    printf("\33[47m\33[32m***************************************************\33[0m\n");
    printf("\33[47m\33[32m*                   mosquitto demo                *\33[0m\n");
    printf("\33[47m\33[32m***************************************************\33[0m\n");
	
	printf("\33[35m\n");
	ret = broker_create();
	if(ret)
	{
		perror("broker create failed");
		exit(1);
	}

	ret = broker_init();
	if(ret)
	{
		perror("broker init failed");
		exit(1);
	}

	ret = broker_listen();
	if(ret)
	{
		perror("broker listen failed");
		exit(1);
	}

	ret = broker_start();
	if(ret)
	{
		perror("broker start failed");
		broker_stop();
		exit(1);
	}

	exit(0);
}
