
/**********************************************************
 * 对外接口封装。
 * 主要包含：webserver初始化；
 * 		   相关配置文件的读取；
 * 		   数据库的初始化；
 * 		   应用程序接口的对外封装实现；
 * 		   webserver服务启动、资源管理以及服务停止等。
 * 详细功能实现，参见各接口函数定义。
 * 以多线程实现该部分。
 * */

#ifndef web_common_h
#define web_common_h

#include "def.h"
#include "readconf.h"

#include "../../include/lwan/lwan.h"
#include "../../include/lwan/lwan-serve-files.h"

/*
 * @功能		Web服务器启动入口函数；
 * @参数		void
 * @返回值	执行状态
 * */
int lwan_run();

/*
 * @功能		Web服务器启动入口函数；
 * @参数		request: 请求数据存储结构
 * 			response: 应答数据存储结构
 * @返回值	执行状态
 * */
lwan_http_status_t common_api(lwan_request_t *request,
            lwan_response_t *response,
            void *data __attribute__((unused)));


#endif	//end of web_common_h
