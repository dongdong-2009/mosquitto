#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "http.h"


#define IP		"120.24.38.104"
//#define IP		"61.147.124.120"
//#define 127.0.0.1
#define PORT 	8888
int main(int argc, char **argv)
//int login(int argc,char **argv)
{
	printf("[info] [test start] [%s].\n", __FUNCTION__);

	int socked = -1;

	// TODO :if (!argv[1] || !argv[2])
	if (!argv[1] || !argv[2])
	{
		printf("Enter param: [IP] [port].\n");
		return 0;
	}
	printf("ip-port: %s-%s.\n", argv[1], argv[2]);
	int port = atoi(argv[2]);
	printf("port = %d.\n", port);
	//if ((socked = net_connect(IP, PORT)) == -1)
	if ((socked = net_connect(argv[1], port)) == -1)
	{
		return -1;
	}

	char buf_send[BUFSIZE] = { 0 };
	char buf_recv[BUFSIZE] = { 0 };
	
	char buf_tmp[100] = { 0 };

	char buf_cont[600] = { 0 };

#if 0
	memset(buf_tmp, 0, sizeof(buf_tmp));
	strcat(buf_send, "GET /hello HTTP/1.1\n");
	strcat(buf_send, "Host: 192.168.207.148:8080\n");
	strcat(buf_send, "Accept: application/x-ms-application, */*\n");
	strcat(buf_send, "Connction: close\n");
	strcat(buf_send, "Content-Type: application/json;charset=utf-8\n");
	strcat(buf_send, "Content-Length: 27\n");
	strcat(buf_send, "{\"title\":\"test\",\"sub\":\"OK\"}\n\n");
#endif

#if 1

	memset(buf_tmp, 0, sizeof(buf_tmp));
	//strcat(buf_send, "POST /api/server/insert HTTP/1.1\r\n");
	strcat(buf_send, "POST /api/login HTTP/1.1\r\n");
	strcat(buf_send, "Host: 192.168.207.148:8080\r\n");
	strcat(buf_send, "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.1.4) Gecko/20091027 Fedora/3.5.4-1.fc12 Firefox/3.5.4\r\n");
	//strcat(buf_send, "Accept: text/html,application/xhtml+xml,application/xml\r\n");
	strcat(buf_send, "Accept: application/json\r\n");
	strcat(buf_send, "Accept-Language: en-us,en\r\n");
	strcat(buf_send, "Accept-Encoding: gzip,deflate\r\n");
	strcat(buf_send, "Accept-Charset: ISO-8859-1,utf-8\r\n");
	strcat(buf_send, "Keep-Alive: 300\r\n");
	strcat(buf_send, "Connection: keep-alive\r\n");
	strcat(buf_send, "Content-Type: application/x-www-form-urlencoded\r\n");
	strcat(buf_send, "Content-Length: ");

	//int len = sprintf(buf_cont, "{\"product_key\":\"2222\",\"device_code\":\"0001\",\"device_type\":\"1\",\"device_module\":\"Linux\",\"version\":\"0.0.1\"}");
	//int len = sprintf(buf_cont, "{\"vendor_name\":\"test001\",\"vendor_key\": \"0001\",\"vendor_description\":\"test001\"}");
	//int len = sprintf(buf_cont, "{\"vendor_name\":\"test01\",\"vendor_key\":\"101010\",\"vendor_description\":\"test01\"}");
	//int len = sprintf(buf_cont, "{\"req_type\":1,\"db_name\":0,\"tab_name\":1,\"req_cnt\":1,\"req_data1\":{\"product_name\":\"pdt_01\",\"product_description\":\"0101\",\"product_key\":\"123456789\",\"product_config\":\"hkajsh\"}}");
	//int len = sprintf(buf_cont, "{\"prot_info\":{\"req_type\":1,\"db_name\":1,\"tab_name\":1,\"req_cnt\":2},\"data_info\":{\"product_name\":\"pdt_01\",\"product_description\":\"0101\",\"product_key\":\"123456789\",\"product_config\":\"hkajsh\"},\"data_info\":{\"product_name\":\"pdt_02\",\"product_description\":\"0202\",\"product_key\":\"987654321\",\"product_config\":\"KLSADJKAS\"}}");
	//int len = sprintf(buf_cont, "{\"prot_info\":{\"req_type\":4,\"db_name\":1,\"tab_name\":1,\"req_cnt\":2},\"data_info\":{\"server_vendor_id\": 1, \"server_name\":\"pdt_01\",\"server_descripition\":\"0101\",\"server_key\":\"123456789\",\"server_config\":\"hkajsh\"},\"data_info\":{\"server_vendor_id\": 1,\"server_name\":\"pdt_02\",\"server_description\":\"0202\",\"server_key\":\"987654321\",\"server_config\":\"KLSADJKAS\"}}");
	//int len = sprintf(buf_cont, "{\"data_info\":{\"server_vendor_id\": 1, \"server_name\":\"pdt_01\",\"server_description\":\"0101\",\"server_key\":\"123456789\",\"server_config\":\"hkajsh\"},\"data_info\":{\"server_vendor_id\": 1,\"server_name\":\"pdt_02\",\"server_description\":\"0202\",\"server_key\":\"987654321\",\"server_config\":\"KLSADJKAS\"}}");
	//int len = sprintf(buf_cont, "{\"prot_info\":{\"req_type\":4,\"db_name\":1,\"tab_name\":1,\"req_cnt\":2},\"data_info\":{\"server_id\":1, \"server_description\":\"jlasjdl\", \"server_key\":\"123456789\"},\"data_info\":{\"server_id\":3, \"server_description:\"0202\", \"server_key\":\"987654321\"}}");
	int len = sprintf(buf_cont, "{\"msg\":{\"F1\": \"V1\", \"F2\":\"V2\"},\"server_id\": \"hos_server_01\",\"device_id\": \"dev_01\"}}");
	memset(buf_tmp, 0, sizeof(buf_tmp));
	sprintf(buf_tmp, "%d\r\n\r\n", len);
	strcat(buf_send, buf_tmp);
	
	strcat(buf_send, buf_cont);

#endif


#if 0
	memset(buf_tmp, 0, sizeof(buf_tmp));
	strcat(buf_send, "POST /post HTTP/1.1\r\n");

	//strcat(buf_send, "Content-Type: application/json, application/x-ms-application, image/jpeg, application/xaml+xml, image/gif, image/pjpeg, application/x-ms-xbap, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*\r\n");
	strcat(buf_send, "text/html");
	strcat(buf_send, "Accept-Language: zh-CN\r\n");
	strcat(buf_send, "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.1; WOW64; Trident/7.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; InfoPath.3; .NET4.0C; .NET4.0E)\r\n");
	strcat(buf_send, "Accept-Encoding: gzip, deflate\r\n");
	strcat(buf_send, "Host: 192.168.207.148:8080\r\n");
	strcat(buf_send, "Connection: close\r\n");
	
	strcat(buf_send, "Content-Length: ");

	int len = sprintf(buf_cont, "{\"product_key\":\"2222\",\"device_code\":\"0001\",\"device_type\":\"1\",\"device_module\":\"Linux\",\"version\":\"0.0.1\"}");

	memset(buf_tmp, 0, sizeof(buf_tmp));
	sprintf(buf_tmp, "%d\r\n\r\n", len);
	strcat(buf_send, buf_tmp);
	//strcat(buf_send, "\r\n\r\n");

	strcat(buf_send, buf_cont);
	strcat(buf_send, "\r\n\r\n");
#endif
#if 0
	memset(buf_tmp, 0, sizeof(buf_tmp));
	strcat(buf_send, "POST hello HTTP/1.1\r\n");
	sprintf(buf_tmp, "Host: %s:%s\r\n", argv[1], argv[2]);
	strcat(buf_send, buf_tmp);
	strcat(buf_send, "Connection: close\r\n");
	strcat(buf_send, "User-Agent: Linux\r\n");
	strcat(buf_send, "Content-Type: application/json\r\n");
	strcat(buf_send, "Content-Length: 0\r\n");
	//memset(buf_tmp, 0, sizeof(buf_tmp));
	//sprintf(buf_tmp, "");
#endif
#if 0
    memset(buf_tmp, 0, sizeof(buf_tmp));
	strcat(buf_send, "GET /post HTTP/1.1\r\n");
	sprintf(buf_tmp, "Host: %s:%s\r\n", argv[1], argv[2]);
	strcat(buf_send, buf_tmp);

	strcat(buf_send, "Connection: close\n");
	strcat(buf_send, "User-Agent: firefox4.0.1-Windows\r\n");
	strcat(buf_send, "\r\n");
	strcat(buf_send, "Content-Type: text/html\n");
	strcat(buf_send, "\r\n");
	strcat(buf_send, "Content-Length: 0\n");
#endif
	
	printf("%s\n", buf_send);

	int ret = -1;
	if ((ret = net_send(socked, buf_send, strlen(buf_send))) < 0)
	{
		return -1;
	}

	printf("ret = %d.\n", ret);

	fd_set t_set;

	FD_ZERO(&t_set);
	FD_SET(socked, &t_set);

	int loop = 0, h = 0;
	struct timeval tv;

	while (1)
	{
		sleep(2);

		tv.tv_sec = 0;
		tv.tv_usec = 0;

		h = 0;
		printf("-->1\n");
		h = select(socked + 1, &t_set, NULL, NULL, &tv);
		printf("-->2\n");

		if (h < 0)
		{
			close(socked);
			printf("select ERR.\n");
			return -1;
		}

		if (h > 0)
		{
			loop = read(socked, buf_recv, sizeof(buf_recv));
			if (loop == 0)
			{
				close(socked);
				printf("Server closed.\n");

				return -1;
			}
			printf("recv_len: %d.\n", (int)strlen(buf_recv));
			printf("recv: %s.\n", buf_recv);
		}
	}

	close(socked);

	return 1;
}



