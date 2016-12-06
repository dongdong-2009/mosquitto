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

#define _GNU_SOURCE

#include <config.h>

#include <assert.h>
#ifndef WIN32
#include <poll.h>
#else
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
#  include <sys/socket.h>
#endif
#include <time.h>

#ifdef WITH_WEBSOCKETS
#  include <libwebsockets.h>
#endif

#include <mosquitto_broker.h>
#include <memory_mosq.h>
#include <send_mosq.h>
#include <time_mosq.h>
#include <util_mosq.h>

extern bool flag_reload;
#ifdef WITH_PERSISTENCE
extern bool flag_db_backup;
#endif
extern bool flag_tree_print;
extern int run;
#ifdef WITH_SYS_TREE
extern int g_clients_expired;
#endif

static void loop_handle_errors(struct mosquitto_db *db, struct pollfd *pollfds);
static void loop_handle_reads_writes(struct mosquitto_db *db, struct pollfd *pollfds);

#ifdef WITH_WEBSOCKETS
static void temp__expire_websockets_clients(struct mosquitto_db *db)
{
	struct mosquitto *context, *ctxt_tmp;
	static time_t last_check = 0;
	time_t now = mosquitto_time();
	char *id;

	if(now - last_check > 60){
		HASH_ITER(hh_id, db->contexts_by_id, context, ctxt_tmp){
			if(context->wsi && context->sock != INVALID_SOCKET){
				if(context->keepalive && now - context->last_msg_in > (time_t)(context->keepalive)*3/2){
					if(db->config->connection_messages == true){
						if(context->id){
							id = context->id;
						}else{
							id = "<unknown>";
						}
						_mosquitto_log_printf(NULL, MOSQ_LOG_NOTICE, "Client %s has exceeded timeout, disconnecting.", id);
					}
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(db, context);
				}
			}
		}
		last_check = mosquitto_time();
	}
}
#endif

/********************************************************************************************************************************************
 *                                 mosquitto核心poll监听
 * 1、监听sokcet负责处理新的客户端的连接并生成context（代表一个客户端连接）
 *		备注：在此可以设置客户端建立tcp连接之后发送的第一个包作为设备收费认证
 * 2、业务socket负责已经建立mqtt连接的客户端是否有消息收发
 *		接收消息：mosquitto服务器收到某一客户端向某一主题发布一条消息，会保存到该主题的订阅者列表的消息队列中
 *		发送消息：mosquitto服务器将消息推送给订阅者
 * 3、poll整个流程
 *										  开始
 *									  		|
 * 											V
 *       --------------------------------run=？1 <----------------------------------------------------------------------|
 *		|								    |																			|
 *		|									V																			|
 *		|			根据监听的socket数量，创建pollfd结构体数组pollfds													|
 *		|									|																			|
 *		|									V																			|
 *		|			循环处理所有客户端，对每个客户端的context进行处理													|
 *		|			1、检查客户端是否超时（一般是订阅客户端，默认60s发一次心跳）										|
 *		|				备注：可以借用这里的心跳实现XGO服务的设备在线判断												|
 *		|			2、检查是否有消息发送（发布者发布的消息暂存在订阅者的消息队列中）									|
 *		|			3、设置对应socket的pollfd结构体，并放入pollfd（接收订阅者心跳、取消订阅、断开连接等mqtt包）			|
 *		|			 						|																			|
 *		|									V																			|
 *		|				调用poll函数监听pollfds数组中的socket描述符														|
 *		|									|																			|
 *		|									V																			|
 *		|			    		返回就绪的socket描述符 -----------------------------------------------------------------|
 *		|							
 *		|--------------------------------> 结束
 ********************************************************************************************************************************************/
//listensock:监听socket主要负责客户端的链接请求，当有客户端要连接的时候，立即创建一个业务socket，负责后续mosquitto服务器与该客户端的所有数据收发
int mosquitto_main_loop(struct mosquitto_db *db, mosq_sock_t *listensock, int listensock_count, int listener_max)
{
	//printf("sock:%d,count:%d,max:%d\n",*listensock,listensock_count,listener_max);		//3		2	4
	//printf("mosq_sock_t(len):%d\n",sizeof(mosq_sock_t));
#ifdef WITH_SYS_TREE
	//printf("sys tree\n");//coming
	time_t start_time = mosquitto_time();
#endif
#ifdef WITH_PERSISTENCE
	//printf("persistence\n");//coming
	time_t last_backup = mosquitto_time();
#endif
	time_t now = 0;
	time_t now_time;
	int time_count;
	int fdcount;
	struct mosquitto *context, *ctxt_tmp;
#ifndef WIN32
	//printf("win32\n");//coming
	sigset_t sigblock, origsig;
#endif
	int i;
	struct pollfd *pollfds = NULL;
	int pollfd_count = 0;
	int pollfd_index;
#ifdef WITH_BRIDGE
	//printf("bridge\n");//coming
	mosq_sock_t bridge_sock;
	int rc;
#endif
	int context_count;
	time_t expiration_check_time = 0;
	time_t last_timeout_check = 0;
	char *id;

#ifndef WIN32
	//将SIGINT信号添加到阻塞信号集中，在下面开启poll之前阻塞该信号
	sigemptyset(&sigblock);
	sigaddset(&sigblock, SIGINT);
#endif

	if(db->config->persistent_client_expiration > 0){
		expiration_check_time = time(NULL) + 3600;
	}

	while(run){
		mosquitto__free_disused_contexts(db);
#ifdef WITH_SYS_TREE
		//跟新系统子树
		if(db->config->sys_interval > 0){
			//每次有新的连接或断开连接或数据交互循环都会走一次
			mqtt3_db_sys_update(db, db->config->sys_interval, start_time);
		}
#endif

		context_count = HASH_CNT(hh_sock, db->contexts_by_sock);
		//计算当前连接客户端个数
		//printf("context_count:%d\n",context_count);
#ifdef WITH_BRIDGE
		context_count += db->bridge_count;
		//db->bridge_count的值一直为0
		//printf("db->bridge_count:%d\n",db->bridge_count);
#endif

		/*=====================================================================================================
		 * 1、根据监听socket的数量，创建pollfds结构体数组
		=======================================================================================================*/
		//listensock_count+context_count是监听连接socket和已经连接socket的总和，有新的用户连接会进入下面判断
		if(listensock_count + context_count > pollfd_count || !pollfds){
			//更新监听的sock数量
			pollfd_count = listensock_count + context_count;
			pollfds = _mosquitto_realloc(pollfds, sizeof(struct pollfd)*pollfd_count);
			if(!pollfds){
				_mosquitto_log_printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				return MOSQ_ERR_NOMEM;
			}
			//printf("pollfd_count:%d\n",pollfd_count);
		}

		memset(pollfds, -1, sizeof(struct pollfd)*pollfd_count);

		/**************************************************************************************************
		 * 循环处理所有客户端，对每个客户端context进行下面处理：
		 *		1、检查客户端超时
		 *		2、检查是否有消息发送，有则发送
		 *		3、设置对应的socket的pollfd结构体并加入pollfds数组
		 **************************************************************************************************/
		//将IPV4和IPV6两个监听socket加入poll集[0][1],监听动作为POLLIN
		pollfd_index = 0;
		for(i=0; i<listensock_count; i++){
			pollfds[pollfd_index].fd = listensock[i];
			//监听动作为输入，即监听客户端的连接
			pollfds[pollfd_index].events = POLLIN;
			pollfds[pollfd_index].revents = 0;
			pollfd_index++;
		}

		now_time = time(NULL);

		time_count = 0;
		HASH_ITER(hh_sock, db->contexts_by_sock, context, ctxt_tmp){
			if(time_count > 0){
				time_count--;
			}else{
				time_count = 1000;
				now = mosquitto_time();
			}
			context->pollfd_index = -1;

			if(context->sock != INVALID_SOCKET){
#ifdef WITH_BRIDGE
				if(context->bridge){
					_mosquitto_check_keepalive(db, context);
					if(context->bridge->round_robin == false
							&& context->bridge->cur_address != 0
							&& now > context->bridge->primary_retry){
						if(_mosquitto_try_connect(context, context->bridge->addresses[0].address, context->bridge->addresses[0].port, &bridge_sock, NULL, false) <= 0){
							COMPAT_CLOSE(bridge_sock);
							_mosquitto_socket_close(db, context);
							context->bridge->cur_address = context->bridge->address_count-1;
						}
					}
				}
#endif

				/* Local bridges never time out in this fashion. */
				if(!(context->keepalive)
						|| context->bridge
						|| now - context->last_msg_in < (time_t)(context->keepalive)*3/2){
					
					//推送发布者发布给主题的订阅者列表的消息
					//测试结果：
					//	1、MQTT客户端的mqtt报文都在这处理，注释掉都无法处理CONNECT报文
#if 1
					if(mqtt3_db_message_write(db, context) == MOSQ_ERR_SUCCESS){
						pollfds[pollfd_index].fd = context->sock;
						pollfds[pollfd_index].events = POLLIN;
						pollfds[pollfd_index].revents = 0;
						if(context->current_out_packet || context->state == mosq_cs_connect_pending){
							pollfds[pollfd_index].events |= POLLOUT;
						}
						context->pollfd_index = pollfd_index;
						pollfd_index++;
					}else{
						do_disconnect(db, context);
					}
#endif
				}else{
					/***************************************************************
					* 该客户端在keepaliave时间内没有发心跳，认为该客户超时，将其踢掉
					* 	备注：在此可以检测设备是否在线（XGO服务中）
					*		  返回context->id（应该等同设备id）
					****************************************************************/
					if(db->config->connection_messages == true){
						if(context->id){
							id = context->id;
						}else{
							id = "<unknown>";
						}
						_mosquitto_log_printf(NULL, MOSQ_LOG_NOTICE, "Client %s has exceeded timeout, disconnecting.", id);
					}
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(db, context);
				}
			}
		}

#ifdef WITH_BRIDGE
		time_count = 0;
		for(i=0; i<db->bridge_count; i++){
			if(!db->bridges[i]) continue;

			context = db->bridges[i];

			if(context->sock == INVALID_SOCKET){
				if(time_count > 0){
					time_count--;
				}else{
					time_count = 1000;
					now = mosquitto_time();
				}
				/* Want to try to restart the bridge connection */
				if(!context->bridge->restart_t){
					context->bridge->restart_t = now+context->bridge->restart_timeout;
					context->bridge->cur_address++;
					if(context->bridge->cur_address == context->bridge->address_count){
						context->bridge->cur_address = 0;
					}
					if(context->bridge->round_robin == false && context->bridge->cur_address != 0){
						context->bridge->primary_retry = now + 5;
					}
				}else{
					if(context->bridge->start_type == bst_lazy && context->bridge->lazy_reconnect){
						rc = mqtt3_bridge_connect(db, context);
						if(rc){
							context->bridge->cur_address++;
							if(context->bridge->cur_address == context->bridge->address_count){
								context->bridge->cur_address = 0;
							}
						}
					}
					if(context->bridge->start_type == bst_automatic && now > context->bridge->restart_t){
						context->bridge->restart_t = 0;
						rc = mqtt3_bridge_connect(db, context);
						if(rc == MOSQ_ERR_SUCCESS){
							pollfds[pollfd_index].fd = context->sock;
							pollfds[pollfd_index].events = POLLIN;
							pollfds[pollfd_index].revents = 0;
							if(context->current_out_packet){
								pollfds[pollfd_index].events |= POLLOUT;
							}
							context->pollfd_index = pollfd_index;
							pollfd_index++;
						}else{
							/* Retry later. */
							context->bridge->restart_t = now+context->bridge->restart_timeout;

							context->bridge->cur_address++;
							if(context->bridge->cur_address == context->bridge->address_count){
								context->bridge->cur_address = 0;
							}
						}
					}
				}
			}
		}
#endif
		now_time = time(NULL);
		if(db->config->persistent_client_expiration > 0 && now_time > expiration_check_time){
			HASH_ITER(hh_id, db->contexts_by_id, context, ctxt_tmp){
				if(context->sock == INVALID_SOCKET && context->clean_session == 0){
					/* This is a persistent client, check to see if the
					 * last time it connected was longer than
					 * persistent_client_expiration seconds ago. If so,
					 * expire it and clean up.
					 */
					if(now_time > context->disconnect_t+db->config->persistent_client_expiration){
						if(context->id){
							id = context->id;
						}else{
							id = "<unknown>";
						}
						_mosquitto_log_printf(NULL, MOSQ_LOG_NOTICE, "Expiring persistent client %s due to timeout.", id);
#ifdef WITH_SYS_TREE
						g_clients_expired++;
#endif
						context->clean_session = true;
						context->state = mosq_cs_expiring;
						do_disconnect(db, context);
					}
				}
			}
			expiration_check_time = time(NULL) + 3600;
		}

		if(last_timeout_check < mosquitto_time()){
			/* Only check at most once per second. */
			//核查客户端的超时
			mqtt3_db_message_timeout_check(db, db->config->retry_interval);
			last_timeout_check = mosquitto_time();
		}

#ifndef WIN32
		//关闭这两个sigprocmask()系统调用可以在命令行使用ctrl+c信号
		sigprocmask(SIG_SETMASK, &sigblock, &origsig);
		//为了保证监听过程不被系统信号打断，只poll监听socket和业务socket
		fdcount = poll(pollfds, pollfd_index, -1);
		sigprocmask(SIG_SETMASK, &origsig, NULL);
		//printf("this way run\n");
#else
		//开启poll监听
		fdcount = WSAPoll(pollfds, pollfd_index, 100);
#endif
		if(fdcount == -1){
			//循环处理错误
			loop_handle_errors(db, pollfds);
		}else{
			//业务socket主要通过该函数完成,在该函数中循环检测所有的context,对对应的sock执行_mosquitto_packet_read/write操作
			loop_handle_reads_writes(db, pollfds);

			//监听socket上面检测到有新用户连接,同时支持IPV4和IPV6
			for(i=0; i<listensock_count; i++){		//listensock_count = 2;
				if(pollfds[i].revents & (POLLIN | POLLPRI)){
					while(mqtt3_socket_accept(db, listensock[i]) != -1){
					}
				}
			}
		}
#ifdef WITH_PERSISTENCE
		if(db->config->persistence && db->config->autosave_interval){
			if(db->config->autosave_on_changes){
				if(db->persistence_changes >= db->config->autosave_interval){
					mqtt3_db_backup(db, false);
					db->persistence_changes = 0;
				}
			}else{
				//coming
				if(last_backup + db->config->autosave_interval < mosquitto_time()){
					//no coming
					//printf("--------------->persistence\n");
					mqtt3_db_backup(db, false);
					last_backup = mosquitto_time();
				}
			}
		}
#endif

#ifdef WITH_PERSISTENCE
		if(flag_db_backup){
			//no coming
			//printf("flag_db_backup\n");
			mqtt3_db_backup(db, false);
			flag_db_backup = false;
		}
#endif
		if(flag_reload){
			//no coming
			//printf("flag_reload\n");
			_mosquitto_log_printf(NULL, MOSQ_LOG_INFO, "Reloading config.");
			mqtt3_config_read(db->config, true);
			mosquitto_security_cleanup(db, true);
			mosquitto_security_init(db, true);
			mosquitto_security_apply(db);
			mqtt3_log_close(db->config);
			mqtt3_log_init(db->config);
			flag_reload = false;
		}
		if(flag_tree_print){
			//no coming
			//printf("flag_tree_print\n");
			mqtt3_sub_tree_print(&db->subs, 0);
			flag_tree_print = false;
		}
#ifdef WITH_WEBSOCKETS
		for(i=0; i<db->config->listener_count; i++){
			/* Extremely hacky, should be using the lws provided external poll
			 * interface, but their interface has changed recently and ours
			 * will soon, so for now websockets clients are second class
			 * citizens. */
			if(db->config->listeners[i].ws_context){
				libwebsocket_service(db->config->listeners[i].ws_context, 0);
			}
		}
		if(db->config->have_websockets_listener){
			temp__expire_websockets_clients(db);
		}
#endif
	}

	if(pollfds) _mosquitto_free(pollfds);
	return MOSQ_ERR_SUCCESS;
}

void do_disconnect(struct mosquitto_db *db, struct mosquitto *context)
{
	char *id;

	if(context->state == mosq_cs_disconnected){
		return;
	}
#ifdef WITH_WEBSOCKETS
	if(context->wsi){
		if(context->state != mosq_cs_disconnecting){
			context->state = mosq_cs_disconnect_ws;
		}
		if(context->wsi){
			libwebsocket_callback_on_writable(context->ws_context, context->wsi);
		}
		context->sock = INVALID_SOCKET;
	}else
#endif
	{
		if(db->config->connection_messages == true){
			if(context->id){
				id = context->id;
			}else{
				id = "<unknown>";
			}
			if(context->state != mosq_cs_disconnecting){
				_mosquitto_log_printf(NULL, MOSQ_LOG_NOTICE, "Socket error on client %s, disconnecting.", id);
			}else{
				_mosquitto_log_printf(NULL, MOSQ_LOG_NOTICE, "Client %s disconnected.", id);
			}
		}
		mqtt3_context_disconnect(db, context);
#ifdef WITH_BRIDGE
		if(context->clean_session && !context->bridge){
#else
		if(context->clean_session){
#endif
			mosquitto__add_context_to_disused(db, context);
			if(context->id){
				HASH_DELETE(hh_id, db->contexts_by_id, context);
				_mosquitto_free(context->id);
				context->id = NULL;
			}
		}
		context->state = mosq_cs_disconnected;
	}
}

/* Error ocurred, probably an fd has been closed. 
 * Loop through and check them all.
 */
static void loop_handle_errors(struct mosquitto_db *db, struct pollfd *pollfds)
{
	struct mosquitto *context, *ctxt_tmp;

	HASH_ITER(hh_sock, db->contexts_by_sock, context, ctxt_tmp){
		if(context->pollfd_index < 0){
			continue;
		}

		if(pollfds[context->pollfd_index].revents & (POLLERR | POLLNVAL)){
			do_disconnect(db, context);
		}
	}
}

static void loop_handle_reads_writes(struct mosquitto_db *db, struct pollfd *pollfds)
{
	struct mosquitto *context, *ctxt_tmp;
	int err;
	socklen_t len;

	HASH_ITER(hh_sock, db->contexts_by_sock, context, ctxt_tmp){
		if(context->pollfd_index < 0){
			continue;
		}

		assert(pollfds[context->pollfd_index].fd == context->sock);
#ifdef WITH_TLS
		if(pollfds[context->pollfd_index].revents & POLLOUT ||
				context->want_write ||
				(context->ssl && context->state == mosq_cs_new)){
#else
		if(pollfds[context->pollfd_index].revents & POLLOUT){
#endif
			if(context->state == mosq_cs_connect_pending){
				len = sizeof(int);
				if(!getsockopt(context->sock, SOL_SOCKET, SO_ERROR, (char *)&err, &len)){
					if(err == 0){
						context->state = mosq_cs_new;
					}
				}else{
					do_disconnect(db, context);
					continue;
				}
			}
			if(_mosquitto_packet_write(context)){
				do_disconnect(db, context);
				continue;
			}
		}
	}

	HASH_ITER(hh_sock, db->contexts_by_sock, context, ctxt_tmp){
		if(context->pollfd_index < 0){
			continue;
		}

#ifdef WITH_TLS
		if(pollfds[context->pollfd_index].revents & POLLIN ||
				(context->ssl && context->state == mosq_cs_new)){
#else
		if(pollfds[context->pollfd_index].revents & POLLIN){
#endif
			do{
				if(_mosquitto_packet_read(db, context)){
					do_disconnect(db, context);
					continue;
				}
			}while(SSL_DATA_PENDING(context));
		}
	}
}

