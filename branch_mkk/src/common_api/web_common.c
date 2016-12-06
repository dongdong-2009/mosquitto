
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
#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>

#include "def.h"
#include "readconf.h"
#include "db_common.h"
#include "json_common.h"

#include "web_common.h"


//#include "../../include/lwan/lwan.h"
//#include "../../include/lwan/lwan-serve-files.h"


extern MYSQL *mysql;

lwan_http_status_t quit_lwan(lwan_request_t *request __attribute__((unused)),
           lwan_response_t *response __attribute__((unused)),
           void *data __attribute__((unused)))
{
    exit(0);
    return HTTP_OK;
}

lwan_http_status_t gif_beacon(lwan_request_t *request __attribute__((unused)),
           lwan_response_t *response,
           void *data __attribute__((unused)))
{
    /*
     * 1x1 transparent GIF image generated with tinygif
     * http://www.perlmonks.org/?node_id=7974
     */
    static const unsigned char gif_beacon_data[] = {
        0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x01, 0x00, 0x01, 0x00, 0x90,
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0xF9, 0x04,
        0x05, 0x10, 0x00, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x00, 0x02, 0x02, 0x04, 0x01
    };

    response->mime_type = "image/gif";
    strbuf_set_static(response->buffer, (char*)gif_beacon_data, sizeof(gif_beacon_data));

    return HTTP_OK;
}

lwan_http_status_t test_chunked_encoding(lwan_request_t *request,
            lwan_response_t *response,
            void *data __attribute__((unused)))
{
    int i;

    response->mime_type = "text/plain";

    strbuf_printf(response->buffer, "Testing chunked encoding! First chunk\n");
    lwan_response_send_chunk(request);

    for (i = 0; i <= 10; i++) {
        strbuf_printf(response->buffer, "*This is chunk %d*\n", i);
        lwan_response_send_chunk(request);
    }

    strbuf_printf(response->buffer, "Last chunk\n");
    lwan_response_send_chunk(request);

    return HTTP_OK;
}

lwan_http_status_t test_server_sent_event(lwan_request_t *request,
            lwan_response_t *response,
            void *data __attribute__((unused)))
{
    int i;

    for (i = 0; i <= 10; i++) {
        strbuf_printf(response->buffer, "Current value is %d", i);
        lwan_response_send_event(request, "currval");
    }

    return HTTP_OK;
}

lwan_http_status_t test_proxy(lwan_request_t *request,
           lwan_response_t *response,
           void *data __attribute__((unused)))
{
    lwan_key_value_t *headers = coro_malloc(request->conn->coro, sizeof(*headers) * 2);
    if (UNLIKELY(!headers))
        return HTTP_INTERNAL_ERROR;

    char *buffer = coro_malloc(request->conn->coro, INET6_ADDRSTRLEN);
    if (UNLIKELY(!buffer))
        return HTTP_INTERNAL_ERROR;

    headers[0].key = "X-Proxy";
    headers[0].value = (char*) lwan_request_get_remote_address(request, buffer);
    headers[1].key = NULL;
    headers[1].value = NULL;

    response->headers = headers;

    return HTTP_OK;
}

lwan_http_status_t test_post(lwan_request_t *request, lwan_response_t *response,
    void *data __attribute__((unused)))
{
    static const char type[] = "application/json";
    static const char request_body[] = "{\"will-it-blend\": true}";
    static const char response_body[] = "{\"did-it-blend\": \"oh-hell-yeah\"}";

    if (!request->header.content_type)
        return HTTP_BAD_REQUEST;
    if (!request->header.content_type->value)
        return HTTP_BAD_REQUEST;
    if (request->header.content_type->len != sizeof(type) - 1)
        return HTTP_BAD_REQUEST;
    if (memcmp(request->header.content_type->value, type, sizeof(type) - 1) != 0)
        return HTTP_BAD_REQUEST;

    if (!request->header.body)
        return HTTP_BAD_REQUEST;
    if (!request->header.body->value)
        return HTTP_BAD_REQUEST;
    if (request->header.body->len != sizeof(request_body) - 1)
        return HTTP_BAD_REQUEST;
    if (memcmp(request->header.body->value, request_body, sizeof(request_body) - 1) != 0)
        return HTTP_BAD_REQUEST;

    response->mime_type = type;
    strbuf_set_static(response->buffer, response_body, sizeof(response_body) -1);

    return HTTP_OK;
}

lwan_http_status_t hello_world(lwan_request_t *request,
            lwan_response_t *response,
            void *data __attribute__((unused)))
{
    static lwan_key_value_t headers[] = {
        { .key = "X-The-Answer-To-The-Universal-Question", .value = "42" },
        { NULL, NULL }
    };
    response->headers = headers;
    response->mime_type = "text/plain";

    const char *name = lwan_request_get_query_param(request, "name");
    if (name)
        strbuf_printf(response->buffer, "Hello, %s!", name);
    else
        strbuf_set_static(response->buffer, "Hello, world!", sizeof("Hello, world!") -1);

    const char *dump_vars = lwan_request_get_query_param(request, "dump_vars");
    if (!dump_vars)
        goto end;

    if (request->cookies.base) {
        strbuf_append_str(response->buffer, "\n\nCookies\n", 0);
        strbuf_append_str(response->buffer, "-------\n\n", 0);

        lwan_key_value_t *qs = request->cookies.base;
        for (; qs->key; qs++)
            strbuf_append_printf(response->buffer,
                        "Key = \"%s\"; Value = \"%s\"\n", qs->key, qs->value);
    }

    strbuf_append_str(response->buffer, "\n\nQuery String Variables\n", 0);
    strbuf_append_str(response->buffer, "----------------------\n\n", 0);

    lwan_key_value_t *qs = request->query_params.base;
    for (; qs->key; qs++)
        strbuf_append_printf(response->buffer,
                    "Key = \"%s\"; Value = \"%s\"\n", qs->key, qs->value);

    if (!(request->flags & REQUEST_METHOD_POST))
        goto end;

    strbuf_append_str(response->buffer, "\n\nPOST data\n", 0);
    strbuf_append_str(response->buffer, "---------\n\n", 0);

    for (qs = request->post_data.base; qs->key; qs++)
        strbuf_append_printf(response->buffer,
                    "Key = \"%s\"; Value = \"%s\"\n", qs->key, qs->value);

end:
    return HTTP_OK;
}

/*
 * @功能		Web服务器数据入口函数；
 * @参数		request: 请求数据存储结构
 * 			response: 应答数据存储结构
 * @返回值	执行状态
 * */
lwan_http_status_t common_api(lwan_request_t *request,
            lwan_response_t *response,
            void *data __attribute__((unused)))
{
	if (NULL == request->header.body)
	{
		// @无数据，出错，返回
		return HTTP_BAD_REQUEST;
	}

	// @解析数据；
	// @数据库操作；
	// @传递数据buffer和长度。
	if (FALSE == json_parse_protocol(request->header.body->value, request->header.body->len))
	{
		// @无数据，出错，返回
		return HTTP_BAD_REQUEST;
	}
	printf("%s, %d: EXEC OK\n", __FUNCTION__, __LINE__);


	// TEST @准备response数据
	strbuf_set(response->buffer, "{\"vendor_name\":\"OK\",\"vendor_key\": \"OK\",\"vendor_description\":\"OK\"}",
			strlen("{\"vendor_name\":\"OK\",\"vendor_key\": \"OK\",\"vendor_description\":\"OK\"}"));//)77);
	response->mime_type = "application/json";


	return HTTP_OK;
}

/*
 * @功能		Web服务器启动入口函数；
 * @参数		void
 * @返回值	执行状态
 * */
int lwan_run()
{
    lwan_t l;
/*
    // @初始化数据库连接
    mysql = db_stat();

    if (NULL == mysql)
    	return FALSE;
*/
    // @加载配置文件

    // @初始化数据库、连接数据库
    //db_connect(mysql, host, user, passwd, database);
    if (FALSE == db_connect("127.0.0.1", "root", "111111", "cstack"))
    	return FALSE;

    // @lwan服务初始化
    lwan_init(&l);

    // @lwan进程运行
    lwan_main_loop(&l);

    // @停止进程运行
    lwan_shutdown(&l);

    // @断开数据库连接，释放mysql_conn空间。
    db_disconnect(mysql);

    return EXIT_SUCCESS;
}
