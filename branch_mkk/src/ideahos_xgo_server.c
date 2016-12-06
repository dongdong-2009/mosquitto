#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <data_type.h>
#include "ideahos_xgo_server.h"
#include "mqtt/mqtt_broker.h"

#include "web_common.h"
#include "def.h"
#include "mysql/mysql.h"

extern MYSQL *mysql;

int ideahos_log_init(zlog_category_t **zc)
{
	int rc;

	rc = zlog_init(ZLOG_CONFIG_FILE);
	if (rc) {
		return LOG_READ_CONFIG_ERROR;
	}

	*zc = zlog_get_category("my_cat");
	if (!(*zc)) {
		return LOG_GET_CATEGORY_ERROR;
	}

	return OK;
}

void ideahos_log_destory(void)
{
	zlog_fini();
}

#if 0
static void* lwan_start(void *lwan_config)
{
	lwan_run();
	pthread_exit(NULL);
}
#endif

int ideahos_lwan_start(void)
{
	//启动lwan服务
#if 0
	haosi_server_ptr->lwan_state = pthread_create(&haosi_server_ptr->lwan_pid,NULL,lwan_start,NULL);
	if(haosi_server_ptr->lwan_state < 0)
	{
		return	LWAN_THREAD_CREATE_ERROR; 
	}
#endif	
	haosi_server_ptr->lwan_pid = fork();
	if(haosi_server_ptr->lwan_pid < 0)
	{
		return LWAN_THREAD_CREATE_ERROR;
	}
	else if(haosi_server_ptr->lwan_pid == 0)
	{
		lwan_run();
		return 0;
	}
	//haosi_server_ptr->lwan_pid = getpid();
	return 0;
}

#if 0
static void * mqtt_agient_start(void *mqtt_config)
{
	//下面是无限循环，负责所有设备和云端的数据交互，使用haosi_server_ptr->mqtt_agent_state\
	监测返回值，提供其它监测模块监视MQTT的运行状态
	haosi_server_ptr->mqtt_agent_state = broker_start();

	//此处代表MQTT代理异常终止
	pthread_exit(NULL);
}
#endif

static void ideahos_mqtt_common_data_filter(char *payload,unsigned int len)
{
	payload[len] = '\0';
	zlog_info(haosi_server_ptr->zc,"[common message]:%s[%d]\n",payload,len);
}

static void ideahos_mqtt_audio_data_filter(char *payload,unsigned int len)
{
	payload[len] = '\0';
	zlog_info(haosi_server_ptr->zc,"[audio message]:%s[%d]\n",payload,len);
}

static void ideahos_mqtt_media_data_filter(char *payload,unsigned int len)
{
	payload[len] = '\0';
	zlog_info(haosi_server_ptr->zc,"[media message]:%s[%d]\n",payload,len);
}


int ideahos_mqtt_broker_start(void)
{
	int ret;
	int mqtt_pid_file;
	char mqtt_pid_str[10];

	ret = broker_create();
	if(ret)
		return NO_MEMORY;	

	ret = broker_init();
	if(ret)
		return MQTT_AGIENT_INIT_ERROR;

	ret = broker_listen();
	if(ret)
		return MQTT_AGIENT_LISTEN_ERROR;

#if 0
	ret = pthread_create(&haosi_server_ptr->mqtt_agent_pid,NULL,mqtt_agient_start,NULL);
	if(ret)
		return MQTT_AGINET_THREAD_ERROR;
	//printf("=======================%ld\n",haosi_server_ptr->mqtt_agent_pid);
#endif

#if 1
	haosi_server_ptr->mqtt_agent_pid = fork();
	if(haosi_server_ptr->mqtt_agent_pid < 0)
		return MQTT_AGINET_PROGRESS_ERROR;
	else if(haosi_server_ptr->mqtt_agent_pid == 0)
	{
		//在子进程中启动MQTT代理,在main进程中需要waitpid(pid,NULL,0)监视进程状态
		broker_start(haosi_server_ptr->mqtt_data_filter);
		//使用return 0，该子进程结束之后，还会使用父进程的资源???????
		exit(0);
	}
	//haosi_server_ptr->mqtt_agent_pid = getpid();
#endif

#if 0
	sync();
	mqtt_pid_file = open("/tmp/mqtt_pid_file",O_RDWR);
	if(mqtt_pid_file < 0){
		return MQTT_GET_THREAD_PID_ERROR; 
	}
	ret = read(mqtt_pid_file,mqtt_pid_str,10);
	if(ret < 0){
		close(mqtt_pid_file);
		return MQTT_GET_THREAD_PID_ERROR;
	}
	close(mqtt_pid_file);
	haosi_server_ptr->mqtt_agent_pid = atoi(mqtt_pid_str);
#endif
	return OK;
}



int ideahos_server_data_create(Server_Data **haosi_server_ptr)
{
	*haosi_server_ptr = malloc(sizeof(Server_Data));
	if(!(*haosi_server_ptr))
	{
		return NO_MEMORY;
	}

	//分配成功，初始化结构体成员
	//普通成员
	(*haosi_server_ptr)->log_init_state = NO;
	(*haosi_server_ptr)->zc = NULL;
	(*haosi_server_ptr)->lwan_state = NO;
	(*haosi_server_ptr)->mqtt_agent_state = NO;

	//方法成员
	(*haosi_server_ptr)->log_init = ideahos_log_init;
	(*haosi_server_ptr)->log_destory = ideahos_log_destory;
	(*haosi_server_ptr)->lwan_start = ideahos_lwan_start;
	(*haosi_server_ptr)->mqtt_agent_start = ideahos_mqtt_broker_start;

	//监视进程
	(*haosi_server_ptr)->xgo_monitor = ideahos_xgo_monitor;

	//数据过滤接口
	(*haosi_server_ptr)->mqtt_data_filter = malloc(sizeof(Data_filter));
	if(!((*haosi_server_ptr)->mqtt_data_filter))
	{
		return MQTT_FILTER_INIT_ERROR;
	}
	//列举三个主题的过滤
	(*haosi_server_ptr)->mqtt_data_filter->common_data_filter = ideahos_mqtt_common_data_filter;
	(*haosi_server_ptr)->mqtt_data_filter->audio_data_filter = ideahos_mqtt_audio_data_filter;
	(*haosi_server_ptr)->mqtt_data_filter->media_data_filter = ideahos_mqtt_media_data_filter;

	return OK;
}


void ideahos_server_data_destory(Server_Data *haosi_server_ptr)
{
	if(haosi_server_ptr->log_init_state == OK)
	{
		haosi_server_ptr->log_destory();
	}

	if(haosi_server_ptr->lwan_state == OK)
	{
		//lwan资源回收,lwan启动成功，但之后的其他资源导致整体服务无法正常运行
	}

	if(haosi_server_ptr->mqtt_agent_state == OK)
	{
		//MQTT其启动成功，由于其他资源（待定）获取失败，无法正常运行整体服务\
		需要杀死MQTT线程，并回收资源
	}


	free(haosi_server_ptr);
}


void ideahos_xgo_monitor(void)
{
#if 0
	while(haosi_server_ptr->lwan_pid == OK && haosi_server_ptr->mqtt_agent_pid == OK)
	{
		
	}
#endif
}
