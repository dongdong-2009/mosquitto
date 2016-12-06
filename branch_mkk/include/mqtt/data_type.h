#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <pthread.h>
#include <unistd.h>
#include "../log/zlog.h"


#define ZLOG_CONFIG_FILE			"/mnt/hgfs/server/branches/branch_mkk/src/config/cslog.conf"
//定义各种返回值
#define 	OK								0
#define 	NO								-1
#define 	NO_MEMORY						-2
#define 	LOG_READ_CONFIG_ERROR			-3
#define		LOG_GET_CATEGORY_ERROR			-4
#define 	MQTT_AGIENT_INIT_ERROR			-5
#define 	MQTT_AGIENT_LISTEN_ERROR		-6
#define 	MQTT_AGINET_PROGRESS_ERROR		-7
//#define 	MQTT_AGINET_THREAD_ERROR		-7
#define 	MQTT_GET_THREAD_PID_ERROR		-8
#define 	LWAN_THREAD_CREATE_ERROR		-9
#define		MQTT_FILTER_INIT_ERROR			-10

//根据主题过滤对应的MQTT消息
typedef struct data_filter{
	void				(*common_data_filter)(char *payload,unsigned int len);
	void				(*audio_data_filter)(char *payload,unsigned int len);
	void				(*media_data_filter)(char *payload,unsigned int len);
}Data_filter;

//服务相关数据封装
typedef struct server_data{
	int					log_init_state;
	zlog_category_t 	*zc;
	int 				(*log_init)(zlog_category_t**);
	void 				(*log_destory)(void);
	int					lwan_state;
	//pthread_t			lwan_pid;
	pid_t				lwan_pid;
	int					(*lwan_start)(void);
	int 				mqtt_agent_state;
	pid_t				mqtt_agent_pid;	//mqtt代理进程id
	//pthread_t			mqtt_agent_pid;	//mqtt代理进程id
	int					(*mqtt_agent_start)(void);

	Data_filter			*mqtt_data_filter;

	//创建监视所有服务是否正常运行的线程
	pid_t				xgo_monitor_pid;
	void				(*xgo_monitor)(void);
}Server_Data;



#endif
