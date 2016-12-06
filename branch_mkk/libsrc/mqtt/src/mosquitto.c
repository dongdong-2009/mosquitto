/*
Copyright (c) 2009-2014 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License v1.0
and Eclipse Distribution License v1.0 which accompany this distribution.
 
The Eclipse Public License is available at
   http://www.eclipse.org/legal/epl-v10.html
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.
 
Contributors:
   Roger Light - initial implementation and documentation.
*/

#include <config.h>

#ifndef WIN32
/* For initgroups() */
#  define _BSD_SOURCE
#  include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#  include <grp.h>
#endif

#ifndef WIN32
#include <pwd.h>
#else
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#ifdef WITH_WRAP
#include <tcpd.h>
#endif
#ifdef WITH_WEBSOCKETS
#  include <libwebsockets.h>
#endif

#include <mosquitto_broker.h>
#include <memory_mosq.h>
#include "util_mosq.h"


#include <data_type.h>		//add by mkk  mqtt发布消息过滤


struct mosquitto_db int_db;

bool flag_reload = false;
#ifdef WITH_PERSISTENCE
bool flag_db_backup = false;
#endif
bool flag_tree_print = false;
int run;
#ifdef WITH_WRAP
#include <syslog.h>
int allow_severity = LOG_INFO;
int deny_severity = LOG_INFO;
#endif


void handle_sigint(int signal);
void handle_sigusr1(int signal);
void handle_sigusr2(int signal);

struct mosquitto_db *_mosquitto_get_db(void)
{
	return &int_db;
}

/* mosquitto shouldn't run as root.
 * This function will attempt to change to an unprivileged user and group if
 * running as root. The user is given in config->user.
 * Returns 1 on failure (unknown user, setuid/setgid failure)
 * Returns 0 on success.
 * Note that setting config->user to "root" does not produce an error, but it
 * strongly discouraged.
 */
int drop_privileges(struct mqtt3_config *config, bool temporary)
{
#if !defined(__CYGWIN__) && !defined(WIN32)
	struct passwd *pwd;
	char err[256];
	int rc;

	if(geteuid() == 0){
		if(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			if(!pwd){
				_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Invalid user '%s'.", config->user);
				return 1;
			}
			if(initgroups(config->user, pwd->pw_gid) == -1){
				strerror_r(errno, err, 256);
				_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				return 1;
			}
			if(temporary){
				rc = setegid(pwd->pw_gid);
			}else{
				rc = setgid(pwd->pw_gid);
			}
			if(rc == -1){
				strerror_r(errno, err, 256);
				_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				return 1;
			}
			if(temporary){
				rc = seteuid(pwd->pw_uid);
			}else{
				rc = setuid(pwd->pw_uid);
			}
			if(rc == -1){
				strerror_r(errno, err, 256);
				_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				return 1;
			}
		}
		if(geteuid() == 0 || getegid() == 0){
			_mosquitto_log_printf(NULL, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}
#endif
	return MOSQ_ERR_SUCCESS;
}

int restore_privileges(void)
{
#if !defined(__CYGWIN__) && !defined(WIN32)
	char err[256];
	int rc;

	if(getuid() == 0){
		rc = setegid(0);
		if(rc == -1){
			strerror_r(errno, err, 256);
			_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error setting gid whilst restoring privileges: %s.", err);
			return 1;
		}
		rc = seteuid(0);
		if(rc == -1){
			strerror_r(errno, err, 256);
			_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error setting uid whilst restoring privileges: %s.", err);
			return 1;
		}
	}
#endif
	return MOSQ_ERR_SUCCESS;
}

#ifdef SIGHUP
/* Signal handler for SIGHUP - flag a config reload. */
void handle_sighup(int signal)
{
	flag_reload = true;
}
#endif

/* Signal handler for SIGINT and SIGTERM - just stop gracefully. */
void handle_sigint(int signal)
{
	run = 0;
}

/* Signal handler for SIGUSR1 - backup the db. */
void handle_sigusr1(int signal)
{
#ifdef WITH_PERSISTENCE
	flag_db_backup = true;
#endif
}

/* Signal handler for SIGUSR2 - vacuum the db. */
void handle_sigusr2(int signal)
{
	flag_tree_print = true;
}


typedef struct broker{
	mosq_sock_t *listensock;
	int listensock_count;
	int listensock_index;
	struct mqtt3_config config;	//整个broker的配置文件
#ifdef WITH_SYS_TREE
	char buf[1024];
#endif
	FILE *pid;
	int listener_max;
#ifdef WIN32
	SYSTEMTIME st;
#else
	char err[256];
	struct timeval tv;
#endif
	struct mosquitto *ctxt, *ctxt_tmp;
}Broker;

Broker *ideahos_broker = NULL;

int broker_create(void)
{
	ideahos_broker = malloc(sizeof(Broker));
	if(!ideahos_broker)
	{
		return -1;
	}

	ideahos_broker->listensock = NULL;
	ideahos_broker->listensock_count = 0;
	ideahos_broker->listensock_index = 0;
	ideahos_broker->pid = NULL;
	ideahos_broker->listener_max = 0;
	ideahos_broker->ctxt = ideahos_broker->ctxt_tmp = NULL;

	return 0;
}

int broker_init(void)
{
	int ret;
	int argc = 4;
	char *argv[] = {"mosquitto","-c","./src/config/mosquitto.conf","-v"};
	//char *argv[] = {"mosquitto"};

#ifdef WIN32
	GetSystemTime(&ideahos_broker->st);
	srand(ideahos_broker->st.wSecond + ideahos_broker->st.wMilliseconds);
#else
	gettimeofday(&ideahos_broker->tv, NULL);
	srand(ideahos_broker->tv.tv_sec + ideahos_broker->tv.tv_usec);
#endif

	//int_db:broker的内存数据库
	memset(&int_db, 0, sizeof(struct mosquitto_db));

	//初始化broker使用的网络环境
	_mosquitto_net_init();
	
	//初始化broker的整个配置
	mqtt3_config_init(&ideahos_broker->config);

	//可以通过argv参数来配置broker的启动方式
	ret = mqtt3_config_parse_args(&ideahos_broker->config, argc, argv);
	if(ret != MOSQ_ERR_SUCCESS) 
		return ret;

	//初始化broker内存库的配置
	int_db.config = &ideahos_broker->config;

	//创建树根、订阅子树和系统子树
	ret = mqtt3_db_open(&ideahos_broker->config, &int_db);
	if(ret != MOSQ_ERR_SUCCESS){
		_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		return ret;
	}

	//初始化log
	mqtt3_log_init(&ideahos_broker->config);
	_mosquitto_log_printf(NULL, MOSQ_LOG_INFO, "mosquitto version %s (build date %s) starting", VERSION, TIMESTAMP);

	//判断是有配置文件
	if(ideahos_broker->config.config_file){
		_mosquitto_log_printf(NULL, MOSQ_LOG_INFO, "Config loaded from %s.", ideahos_broker->config.config_file);
	}else{
		_mosquitto_log_printf(NULL, MOSQ_LOG_INFO, "Using default config.");
	}

	ret = mosquitto_security_module_init(&int_db);
	if(ret) 
		return ret;
	ret = mosquitto_security_init(&int_db, false);
	if(ret) 
		return ret;

#ifdef WITH_SYS_TREE
	//生成系统子树的两个主题（版本号,创建时间）
	if(ideahos_broker->config.sys_interval > 0){
		/* Set static $SYS messages */
		snprintf(ideahos_broker->buf, 1024, "mosquitto version %s", VERSION);
		mqtt3_db_messages_easy_queue(&int_db, NULL, "$SYS/broker/version", 2, strlen(ideahos_broker->buf), ideahos_broker->buf, 1);
		snprintf(ideahos_broker->buf, 1024, "%s", TIMESTAMP);
		mqtt3_db_messages_easy_queue(&int_db, NULL, "$SYS/broker/timestamp", 2, strlen(ideahos_broker->buf), ideahos_broker->buf, 1);
	}
#endif

	return 0;
}

int broker_listen(void)
{
	int i,j;
	ideahos_broker->listener_max = -1;
	ideahos_broker->listensock_index = 0;

	for(i=0; i<ideahos_broker->config.listener_count; i++){
		if(ideahos_broker->config.listeners[i].protocol == mp_mqtt){
			if(mqtt3_socket_listen(&ideahos_broker->config.listeners[i])){
				mqtt3_db_close(&int_db);
				if(ideahos_broker->config.pid_file){
					remove(ideahos_broker->config.pid_file);
				}
				return 1;
			}
			ideahos_broker->listensock_count += ideahos_broker->config.listeners[i].sock_count;
			ideahos_broker->listensock = _mosquitto_realloc(ideahos_broker->listensock, sizeof(mosq_sock_t)*ideahos_broker->listensock_count);
			if(!ideahos_broker->listensock){
				mqtt3_db_close(&int_db);
				if(ideahos_broker->config.pid_file){
					remove(ideahos_broker->config.pid_file);
				}
				return 1;
			}
			for(j=0; j < ideahos_broker->config.listeners[i].sock_count; j++){
				if(ideahos_broker->config.listeners[i].socks[j] == INVALID_SOCKET){
					mqtt3_db_close(&int_db);
					if(ideahos_broker->config.pid_file){
						remove(ideahos_broker->config.pid_file);
					}
					return 1;
				}
				ideahos_broker->listensock[ideahos_broker->listensock_index] = ideahos_broker->config.listeners[i].socks[j];
				if(ideahos_broker->listensock[ideahos_broker->listensock_index] > ideahos_broker->listener_max){
					ideahos_broker->listener_max = ideahos_broker->listensock[ideahos_broker->listensock_index];
				}
				ideahos_broker->listensock_index++;
			}
		}else if(ideahos_broker->config.listeners[i].protocol == mp_websockets){
#ifdef WITH_WEBSOCKETS
			ideahos_broker->config.listeners[i].ws_context = mosq_websockets_init(&ideahos_broker->config.listeners[i], ideahos_broker->config.websockets_log_level);
			if(!ideahos_broker->config.listeners[i].ws_context){
				_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", ideahos_broker->config.listeners[i].port);
				return 1;
			}
#endif
		}
	}

	return 0;
}

int broker_start(Data_filter *mqtt_data_filter)
{
	int rc;
	int i;
	pid_t mqtt_pid;
	int mqtt_pid_file;
	char str_mqtt_pid[10];

	mqtt_pid = getpid();
	sprintf(str_mqtt_pid,"%d",mqtt_pid);
	//printf("mqtt agient process id %d,str:%s.\n",mqtt_pid,str_mqtt_pid);

	mqtt_pid_file = open("/tmp/mqtt_pid_file",O_WRONLY|O_TRUNC|O_CREAT,0600);
	if(mqtt_pid_file < 0)
	{
		return -1;
	}
	rc = write(mqtt_pid_file,str_mqtt_pid,strlen(str_mqtt_pid));
	if(rc < 0)
	{
		close(mqtt_pid_file);
		return -1;
	}
	close(mqtt_pid_file);

	rc = drop_privileges(&ideahos_broker->config, false);
	if(rc != MOSQ_ERR_SUCCESS) 
		return rc;

	signal(SIGINT, handle_sigint);
	signal(SIGTERM, handle_sigint);
#ifdef SIGHUP
	signal(SIGHUP, handle_sighup);
#endif
#ifndef WIN32
	signal(SIGUSR1, handle_sigusr1);
	signal(SIGUSR2, handle_sigusr2);
	signal(SIGPIPE, SIG_IGN);
#endif

#ifdef WITH_BRIDGE
	for(i=0; i<ideahos_broker->config.bridge_count; i++){
		if(mqtt3_bridge_new(&int_db, &(ideahos_broker->config.bridges[i]))){
			_mosquitto_log_printf(NULL, MOSQ_LOG_WARNING, "Warning: Unable to connect to bridge %s.", 
					ideahos_broker->config.bridges[i].name);
		}
	}
#endif

	//poll option deal with mosquitto message send/receive
	run = 1;
	mosquitto_main_loop(&int_db, ideahos_broker->listensock, ideahos_broker->listensock_count, ideahos_broker->listener_max,mqtt_data_filter);
	
	return 0;
}


void broker_stop(void)
{
	int i;

	_mosquitto_log_printf(NULL, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);
	mqtt3_log_close(&ideahos_broker->config);

#ifdef WITH_PERSISTENCE
	if(ideahos_broker->config.persistence){
		mqtt3_db_backup(&int_db, true);
	}
#endif

#ifdef WITH_WEBSOCKETS
	for(i=0; i<int_db.config->listener_count; i++){
		if(int_db.config->listeners[i].ws_context){
			libwebsocket_context_destroy(int_db.config->listeners[i].ws_context);
		}
		if(int_db.config->listeners[i].ws_protocol){
			_mosquitto_free(int_db.config->listeners[i].ws_protocol);
		}
	}
#endif

	HASH_ITER(hh_id, int_db.contexts_by_id, ideahos_broker->ctxt, ideahos_broker->ctxt_tmp){
#ifdef WITH_WEBSOCKETS
		if(!ideahos_broker->ctxt->wsi){
			mqtt3_context_cleanup(&int_db, ideahos_broker->ctxt, true);
		}
#else
		mqtt3_context_cleanup(&int_db, ideahos_broker->ctxt, true);
#endif
	}
	HASH_ITER(hh_sock, int_db.contexts_by_sock, ideahos_broker->ctxt, ideahos_broker->ctxt_tmp){
		mqtt3_context_cleanup(&int_db, ideahos_broker->ctxt, true);
	}
#ifdef WITH_BRIDGE
	for(i=0; i<int_db.bridge_count; i++){
		if(int_db.bridges[i]){
			mqtt3_context_cleanup(&int_db, int_db.bridges[i], true);
		}
	}
	if(int_db.bridges){
		_mosquitto_free(int_db.bridges);
	}
#endif
	mosquitto__free_disused_contexts(&int_db);

	mqtt3_db_close(&int_db);

	if(ideahos_broker->listensock){
		for(i=0; i<ideahos_broker->listensock_count; i++){
			if(ideahos_broker->listensock[i] != INVALID_SOCKET){
#ifndef WIN32
				close(ideahos_broker->listensock[i]);
#else
				closesocket(ideahos_broker->listensock[i]);
#endif
			}
		}
		_mosquitto_free(ideahos_broker->listensock);
	}

	mosquitto_security_module_cleanup(&int_db);

	if(ideahos_broker->config.pid_file){
		remove(ideahos_broker->config.pid_file);
	}

	mqtt3_config_cleanup(int_db.config);
	_mosquitto_net_cleanup();
}

#if 0
int broker(int argc,char *argv[])
//int main(int argc, char *argv[])
{
	mosq_sock_t *listensock = NULL;
	int listensock_count = 0;
	int listensock_index = 0;
	struct mqtt3_config config;
#ifdef WITH_SYS_TREE
	char buf[1024];
#endif
	int i, j;
	FILE *pid;
	int listener_max;
	int rc;
#ifdef WIN32
	SYSTEMTIME st;
#else
	char err[256];
	struct timeval tv;
#endif
	struct mosquitto *ctxt, *ctxt_tmp;

#if defined(WIN32) || defined(__CYGWIN__)
	if(argc == 2){
		if(!strcmp(argv[1], "run")){
			service_run();
			return 0;
		}else if(!strcmp(argv[1], "install")){
			service_install();
			return 0;
		}else if(!strcmp(argv[1], "uninstall")){
			service_uninstall();
			return 0;
		}
	}
#endif


#ifdef WIN32
	GetSystemTime(&st);
	srand(st.wSecond + st.wMilliseconds);
#else
	gettimeofday(&tv, NULL);
	srand(tv.tv_sec + tv.tv_usec);
#endif

	memset(&int_db, 0, sizeof(struct mosquitto_db));

	_mosquitto_net_init();

	mqtt3_config_init(&config);
	rc = mqtt3_config_parse_args(&config, argc, argv);
	if(rc != MOSQ_ERR_SUCCESS) return rc;
	int_db.config = &config;

	if(config.daemon){
#ifndef WIN32
		switch(fork()){
			case 0:
				break;
			case -1:
				strerror_r(errno, err, 256);
			_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error in fork: %s", err);
				return 1;
			default:
				return MOSQ_ERR_SUCCESS;
		}
#else
		_mosquitto_log_printf(NULL, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
	}

	if(config.daemon && config.pid_file){
		pid = _mosquitto_fopen(config.pid_file, "wt");
		if(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}else{
			_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			return 1;
		}
	}

	rc = mqtt3_db_open(&config, &int_db);
	if(rc != MOSQ_ERR_SUCCESS){
		_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		return rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	mqtt3_log_init(&config);
	_mosquitto_log_printf(NULL, MOSQ_LOG_INFO, "mosquitto version %s (build date %s) starting", VERSION, TIMESTAMP);
	if(config.config_file){
		_mosquitto_log_printf(NULL, MOSQ_LOG_INFO, "Config loaded from %s.", config.config_file);
	}else{
		_mosquitto_log_printf(NULL, MOSQ_LOG_INFO, "Using default config.");
	}

	rc = mosquitto_security_module_init(&int_db);
	if(rc) return rc;
	rc = mosquitto_security_init(&int_db, false);
	if(rc) return rc;

#ifdef WITH_SYS_TREE
	if(config.sys_interval > 0){
		/* Set static $SYS messages */
		snprintf(buf, 1024, "mosquitto version %s", VERSION);
		mqtt3_db_messages_easy_queue(&int_db, NULL, "$SYS/broker/version", 2, strlen(buf), buf, 1);
		snprintf(buf, 1024, "%s", TIMESTAMP);
		mqtt3_db_messages_easy_queue(&int_db, NULL, "$SYS/broker/timestamp", 2, strlen(buf), buf, 1);
	}
#endif

	listener_max = -1;
	listensock_index = 0;
	for(i=0; i<config.listener_count; i++){
		if(config.listeners[i].protocol == mp_mqtt){
			if(mqtt3_socket_listen(&config.listeners[i])){
				mqtt3_db_close(&int_db);
				if(config.pid_file){
					remove(config.pid_file);
				}
				return 1;
			}
			listensock_count += config.listeners[i].sock_count;
			listensock = _mosquitto_realloc(listensock, sizeof(mosq_sock_t)*listensock_count);
			if(!listensock){
				mqtt3_db_close(&int_db);
				if(config.pid_file){
					remove(config.pid_file);
				}
				return 1;
			}
			for(j=0; j<config.listeners[i].sock_count; j++){
				if(config.listeners[i].socks[j] == INVALID_SOCKET){
					mqtt3_db_close(&int_db);
					if(config.pid_file){
						remove(config.pid_file);
					}
					return 1;
				}
				listensock[listensock_index] = config.listeners[i].socks[j];
				if(listensock[listensock_index] > listener_max){
					listener_max = listensock[listensock_index];
				}
				listensock_index++;
			}
		}else if(config.listeners[i].protocol == mp_websockets){
#ifdef WITH_WEBSOCKETS
			config.listeners[i].ws_context = mosq_websockets_init(&config.listeners[i], config.websockets_log_level);
			if(!config.listeners[i].ws_context){
				_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Unable to create websockets listener on port %d.", config.listeners[i].port);
				return 1;
			}
#endif
		}
	}

	rc = drop_privileges(&config, false);
	if(rc != MOSQ_ERR_SUCCESS) return rc;

	signal(SIGINT, handle_sigint);
	signal(SIGTERM, handle_sigint);
#ifdef SIGHUP
	signal(SIGHUP, handle_sighup);
#endif
#ifndef WIN32
	signal(SIGUSR1, handle_sigusr1);
	signal(SIGUSR2, handle_sigusr2);
	signal(SIGPIPE, SIG_IGN);
#endif

#ifdef WITH_BRIDGE
	for(i=0; i<config.bridge_count; i++){
		if(mqtt3_bridge_new(&int_db, &(config.bridges[i]))){
			_mosquitto_log_printf(NULL, MOSQ_LOG_WARNING, "Warning: Unable to connect to bridge %s.", 
					config.bridges[i].name);
		}
	}
#endif

	//poll option deal with mosquitto message send/receive
	run = 1;
	rc = mosquitto_main_loop(&int_db, listensock, listensock_count, listener_max);

	_mosquitto_log_printf(NULL, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);
	mqtt3_log_close(&config);

#ifdef WITH_PERSISTENCE
	if(config.persistence){
		mqtt3_db_backup(&int_db, true);
	}
#endif

#ifdef WITH_WEBSOCKETS
	for(i=0; i<int_db.config->listener_count; i++){
		if(int_db.config->listeners[i].ws_context){
			libwebsocket_context_destroy(int_db.config->listeners[i].ws_context);
		}
		if(int_db.config->listeners[i].ws_protocol){
			_mosquitto_free(int_db.config->listeners[i].ws_protocol);
		}
	}
#endif

	HASH_ITER(hh_id, int_db.contexts_by_id, ctxt, ctxt_tmp){
#ifdef WITH_WEBSOCKETS
		if(!ctxt->wsi){
			mqtt3_context_cleanup(&int_db, ctxt, true);
		}
#else
		mqtt3_context_cleanup(&int_db, ctxt, true);
#endif
	}
	HASH_ITER(hh_sock, int_db.contexts_by_sock, ctxt, ctxt_tmp){
		mqtt3_context_cleanup(&int_db, ctxt, true);
	}
#ifdef WITH_BRIDGE
	for(i=0; i<int_db.bridge_count; i++){
		if(int_db.bridges[i]){
			mqtt3_context_cleanup(&int_db, int_db.bridges[i], true);
		}
	}
	if(int_db.bridges){
		_mosquitto_free(int_db.bridges);
	}
#endif
	mosquitto__free_disused_contexts(&int_db);

	mqtt3_db_close(&int_db);

	if(listensock){
		for(i=0; i<listensock_count; i++){
			if(listensock[i] != INVALID_SOCKET){
#ifndef WIN32
				close(listensock[i]);
#else
				closesocket(listensock[i]);
#endif
			}
		}
		_mosquitto_free(listensock);
	}

	mosquitto_security_module_cleanup(&int_db);

	if(config.pid_file){
		remove(config.pid_file);
	}

	mqtt3_config_cleanup(int_db.config);
	_mosquitto_net_cleanup();

	return rc;
}
#endif

#ifdef WIN32
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char **argv;
	int argc = 1;
	char *token;
	char *saveptr = NULL;
	int rc;

	argv = _mosquitto_malloc(sizeof(char *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lpCmdLine, " ", &saveptr);
	while(token){
		argc++;
		argv = _mosquitto_realloc(argv, sizeof(char *)*argc);
		if(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			return MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(NULL, " ", &saveptr);
	}
	rc = main(argc, argv);
	_mosquitto_free(argv);
	return rc;
}
#endif
