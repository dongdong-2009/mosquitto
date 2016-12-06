#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <mosquitto_sub_pub.h>


static char *ip = "127.0.0.1";
static char *sub_topic = "action/ideahos/server_1/device_1";
static char *pub_topic = "event/ideahos/server_1/device_1";
static char *msg = NULL;
static int publish_flag = 0;

/*
 * 功能：发布消息至broker
 */
int publish(char *str)
{
	int ret;
	Pub_Data *pub_s;

	if(!strcmp(str,"on"))
	{
		str = "The led have on!!!";
	}
	else if(!strcmp(str,"off"))
	{
		str = "The led have off!!!";
	}
	else
	{
		if(publish_flag == 1)
			publish_flag = 0;
		else
			str = "Sorry,you input the error command!!!";
	}
		

	pub_s = malloc(sizeof(Pub_Data));
	if(pub_s == NULL)
	{
		printf("malloc failed\n");
		return -1;
	}

	pub_s->ip = ip;
	pub_s->topic = pub_topic;
	pub_s->msg = str;
	pub_s->port = "1883";
	pub_s->qos = "0";

	ret = mosquitto_pub_init(pub_s);
	if(ret == 1)
	{
		printf("para vailed\n");
		return -1;
	}
	else if(ret == 2)
	{
		printf("create client id failed\n");
		return -1;
	}

	ret = mosquitto_pub_create();
	if(ret)
	{
		printf("create pub failed\n");
		return -1;
	}

	ret = mosquitto_pub_connect();
	if(ret)
	{
		printf("connect broker failed\n");
		return -1;
	}

	ret = mosquitto_pub_publish();
	if(ret)
	{
		printf("publish failed\n");
	}
	
	mosquitto_pub_destory();

	return 0;
}

/*创建线程去发布信息到broker*/
void* publish_dealwith(void *parm)
{
	char *rcv_data = (char *)parm;

	publish(rcv_data);	
	
	pthread_exit(NULL);
}


/*订阅端调用此回调函数，将接收到的信息回传，在此可以进行json数据解析，并对设备进行相应操作*/
int publish_callback(char *rcv_data)
{
	pthread_t publish_pid;
	int ret;

	/*
	  	此处应该做json解析，并把真正的命令传给二次开发者
	   	开发者解析命令，对设备做出操作，然后调用下面的创建线程接口去发布操作结果给broker
	 */
	ret = pthread_create(&publish_pid,NULL,publish_dealwith,(void *)rcv_data);
	if(ret)
	{
		printf("pub pthread create failed\n");
		return -1;
	}

	pthread_join(publish_pid,NULL);

	return 0;
}

int subscribe(void)
{
	int ret;
	Sub_Data *sub_s;
	
	sub_s = malloc(sizeof(Sub_Data));
	if(sub_s == NULL)
	{
		printf("malloc failed\n");
		return -1;
	}
	
	sub_s->ip = ip;
	sub_s->port = "1883";
	sub_s->topic = sub_topic;
	sub_s->qos = "0";
	
	ret = mosquitto_sub_init(sub_s);
	if(ret == 1)
	{
		printf("parm vailed\n");
		return -1;
	}
	else if(ret == 2)
	{
		printf("init sub failed\n");
		return -1;
	}

	ret = mosquitto_sub_create();
	if(ret)
	{
		printf("create sub failed\n");
		return -1;
	}

	ret = mosquitto_sub_connect();
	if(ret)
	{
		printf("sub connect broker failed\n");
		return -1;
	}


	mosquitto_sub_start(publish_callback);

	mosquitto_sub_destory();

	printf("^_^ Bad result ^_^\n");

	return 0;
}

void * sub_dealwith(void *parm)
{
	/*读配置订阅*/
	subscribe();

	printf("sub end error!!\n");
}

#if 0
//设备端开发流程
int main()
{
	1、从本地读取ID2（设备标识）;
	
	2、带着ID2从服务器（柏峰数据库）获取主题（action/企业名字/企业服务器/设备ID2）;

	3、订阅主题（服务器）;

	4、向SDK（昆：封装库）注册回调函数用于接收服务器（正阳）下发的json解析后的字符串命令;

	5、解析命令对设备进行相应操作，并调用库函数（昆：创建线程publish内容向broker）;
}
#endif


int main(void)
{
	pthread_t sub_pid;
	int ret;

	//从本地读取ID2

	//带着ID2去服务器认证并获取主题资源，生成主题

	//订阅主题
	ret = pthread_create(&sub_pid,NULL,sub_dealwith,NULL);
	if(ret)
	{
		printf("sub pthread create failed\n");
		return -1;
	}

	//上报需要开灯event
	publish_flag = 1;
	publish_callback("hello,you need open led");
	
	pthread_join(sub_pid,NULL);

	printf("sub end\n");

	return 0;
}

#if 0
//多次publish内容到broker之后订阅端收不到
int main(void)
{
	char *argv[] = {"pub","-t","event/ideahos/server_1/device_1","-m","aa"};
	char *argv1[] = {"pub","-t","event/ideahos/server_1/device_1","-m","bb"};

	while(1)
	{
		client_pub(5,argv);
		sleep(1);
		client_pub(5,argv1);
		sleep(1);
	}

	return 0;
}
#endif
