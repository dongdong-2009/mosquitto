/*** ***************************************
** Author		 :  周利民
** Last Modified :  2016-09-21 16:54
** Filename		 :  openssl_email.c
** Description   :  本文件主要实现通过登陆SMTP.qq.com邮箱服务器发送邮件
					需要使用readconfig.c文件里读取配置文件参数的函数接口 GetConfigfileString()
** Other		 :
** Function List :	SetSMTPServer()  函数功能是创建一个socket连接发送邮件服务器
					SendEmail()		 函数功能是发送一个邮件到指定的用户邮箱
** History		 :
** ****************************************/
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//openssl应用需要包含的头文件
//#include "openssl/crypto.h"
//#include "openssl/x509.h"
//#include "openssl/rand.h"
//#include "openssl/pem.h"
//#include "openssl/ssl.h"
//#include "openssl/err.h"

#define SENDER ("test@haieros.com")	//发件人
#define SMTPSERV ("14.17.57.217")	//qq邮箱的SMTP发送服务器地址

#define EMAILLEN 1024

//设置SMTP服务器
int SetSMTPServer(int *sockfd, struct sockaddr_in *servaddr)
{
	int res = -1;
	int iconn = -2;
	ssize_t retConnect = -2;
	char buf[255] = "";

	*sockfd = socket(AF_INET,SOCK_STREAM,0);//以scoket方式和邮件服务器通讯
	if(*sockfd>0)
	{
		bzero(servaddr, sizeof(struct sockaddr_in));
		
		servaddr->sin_family = AF_INET;
		servaddr->sin_port = htons(587);	//绑定SMTP服务器端口

		inet_pton(AF_INET, SMTPSERV, &servaddr->sin_addr);

		iconn = connect(*sockfd, (struct sockaddr *)servaddr,
						sizeof(struct sockaddr));
		if(iconn==0) 
		{
			//获取服务器响应
			retConnect = recv(*sockfd, buf, sizeof(buf), 0);	
			if(retConnect==-1) 
			{
				printf("Failed to receive msg from smtp port");
				res = -1;
			} 
			else 
			{
				printf("\nServer:%s\n",buf);
				res = 0;
			}
		} 
		else 
		{
			printf("connect server failed!");
			sleep(1);
			close(*sockfd);
			res = -1;
		}
	} 
	else 
	{
		printf("open socket failed!");
		res = -1;
	}

	return 0;
}
//发送邮件
int SendEmail(const char *RECIVER, const char *fileContent, const char *subject)
{
	int sockfd = -1;
	int iconn = -2;
	int retconfig = -2;
	ssize_t retConnect = -2;
	struct sockaddr_in servaddr;
	char sentmsg[2048] = "";
	char buf[255] = "";

	iconn = SetSMTPServer(&sockfd, &servaddr);	//设置SMTP服务器
	if(iconn < 0) 
	{
		//连接错误
		return -1;
	}
	//连接SMTP服务器
	memset(sentmsg, '\0', sizeof(sentmsg));
	strcpy(sentmsg, "ehlo qq.com\r\n");
	retConnect = send(sockfd, sentmsg, strlen(sentmsg), 0);
	if(retConnect == -1)
	{
		printf("\nFailed to send meg to smtp port in step 1.\n");
		exit(1);
	} 
	else 
	{
		printf("send msg:%s", sentmsg);
	}
	memset(buf, '\0', sizeof(buf));
	retConnect=recv(sockfd, buf, sizeof(buf), 0);
	if(retConnect == -1) 
	{
		printf("\nFailed to recive meg from smtp port in step 1.\n");
		exit(1);
	} 
	else 
	{
		printf("%s\n", buf);
	}
	
	//发送starttls 指令
	memset(sentmsg, '\0', sizeof(sentmsg));
	strcpy(sentmsg, "starttls smtp\r\n");
	retConnect = send(sockfd, sentmsg, strlen(sentmsg), 0);
	if(retConnect == -1) 
	{
		printf("\nFailed to send meg to smtp port in step 2.\n");
		exit(1);
	} 
	else 
	{
		printf("send msg:%s", sentmsg);
	}
	memset(buf, '\0', sizeof(buf));
	retConnect=recv(sockfd, buf, sizeof(buf), 0);
	if(retConnect == -1) 
	{
		printf("\nFailed to recive meg from smtp port in step 2.\n");
		exit(1);
	} 
	else 
	{
		printf("%s\n", buf);
		//sleep(10);
	}
	//用户准备登录
	memset(sentmsg, '\0', sizeof(sentmsg));
	strcpy(sentmsg, "auth login \r\n");
	retConnect = send(sockfd, sentmsg, strlen(sentmsg), 0);
	if(retConnect > 0) 
	{
		printf("%s", sentmsg);
	}
	memset(buf, '\0', sizeof(buf));
	retConnect = recv(sockfd, buf, sizeof(buf), 0);
	if(retConnect > 0) 
	{
		printf("%s\n", buf);
	}
	
	//用户登录包括二步
	//1、发送用户名
	//2、发送用户密码
	//用户名和密码必须用BASE64进行编码
	//发送用户名
	memset(sentmsg, '\0', sizeof(sentmsg));
	//strcpy(sentmsg, "dGVzdEBoYWllcm9zLmNvbQ==\r\n");
	GetConfigfileString("./../config/proj.conf","smtp_server","base64_send_exmail",sentmsg);
	strcat(sentmsg, "\r\n");

	retConnect = send(sockfd, sentmsg, strlen(sentmsg), 0);
	if(retConnect > 0) 
	{
		printf("%s", sentmsg);
	}
	memset(buf, '\0', sizeof(buf));
	retConnect = recv(sockfd, buf, sizeof(buf), 0);
	if(retConnect > 0) 
	{
		printf("%s\n", buf);
	}

	//发送密码
	memset(sentmsg, '\0', sizeof(sentmsg));
	//strcpy(sentmsg, "RG9vcm9zMTcwOA==\r\n");
	GetConfigfileString("./../config/proj.conf","smtp_server","base64_exmial_secret",sentmsg);
	strcat(sentmsg, "\r\n");

	retConnect = send(sockfd, sentmsg, strlen(sentmsg), 0);
	if(retConnect > 0) 
	{
		printf("%s", sentmsg);
	}
	memset(buf, '\0', sizeof(buf));
	retConnect = recv(sockfd, buf, sizeof(buf), 0);
	if(retConnect > 0) 
	{
		printf("%s\n", buf);
	}

	//发送发件人
	memset(sentmsg, '\0', sizeof(sentmsg));
	strcpy(sentmsg, "MAIL FROM:<");
	strcat(sentmsg, SENDER);
	strcat(sentmsg, ">\r\n");
	retConnect = send(sockfd, sentmsg, strlen(sentmsg), 0);
	if(retConnect > 0) 
	{
		printf("%s", sentmsg);
	}
	memset(buf, '\0', sizeof(buf));
	retConnect = recv(sockfd, buf, sizeof(buf), 0);
	if(retConnect > 0) 
	{
		printf("%s\n", buf);
	}

	//发送收件人
	memset(sentmsg, '\0', sizeof(sentmsg));
	//收件人   以<>括起来
	strcpy(sentmsg, "RCPT TO: <");
	strcat(sentmsg, RECIVER);
	strcat(sentmsg, ">\r\n");
	retConnect = send(sockfd, sentmsg, strlen(sentmsg), 0);
	if(retConnect > 0) 
	{
		printf("%s", sentmsg);
	}
	memset(buf, '\0', sizeof(buf));
	retConnect = recv(sockfd, buf, sizeof(buf), 0);
	if(retConnect > 0) 
	{
		printf("%s\n", buf);
	}

	//准备发送邮件内容
	memset(sentmsg, '\0', sizeof(sentmsg));
	strcpy(sentmsg, "DATA\r\n");
	retConnect = send(sockfd, sentmsg, strlen(sentmsg), 0);
	if(retConnect > 0) 
	{
		printf("%s", sentmsg);
	}
	memset(buf, '\0', sizeof(buf));
	retConnect = recv(sockfd, buf, sizeof(buf), 0);
	if(retConnect > 0) 
	{
		printf("%s\n", buf);
	}

	//发送邮件内容
	//包括邮件头:发件人，收件人，主题
	//和邮件正文
	memset(sentmsg, '\0', sizeof(sentmsg));
	//发件人
	strcpy(sentmsg, "From: ");
	strcat(sentmsg, SENDER);
	strcat(sentmsg, "\r\n");
	//收件人
	strcat(sentmsg, "To: <");
	strcat(sentmsg, RECIVER);
	strcat(sentmsg, ">\r\n");
	//主题
	strcat(sentmsg, "Subject: ");
	strcat(sentmsg, subject);
	strcat(sentmsg, "\r\n\r\n");
	//邮件内容
	strcat(sentmsg, fileContent);
	strcat(sentmsg, "\r\n");
	//邮件内容结束 以“\r\n.\r\n”结尾
	strcat(sentmsg, "\r\n.\r\n");
	printf("%s\n", sentmsg);
	retConnect = send(sockfd, sentmsg, strlen(sentmsg), 0);

	//发送邮件结束
	memset(sentmsg, '\0', sizeof(sentmsg));
	strcpy(sentmsg, "QUIT \r\n");
	retConnect = send(sockfd, sentmsg, strlen(sentmsg), 0);
	if(retConnect > 0)
		printf("%s", sentmsg);
	memset(buf, '\0', sizeof(buf));
	retConnect = recv(sockfd, buf, sizeof(buf), 0);
	if(retConnect > 0)
		printf("%s\n", buf);

	//发送邮件结束，关闭Socket
	close(sockfd);
	return 0;
}
 
int main(int argc,char *argv[])
{
	char *reciver = "DAZHOU0503@163.com";
	char *subject = "短信验证码";
	char content[EMAILLEN] = "短信验证码是：23435。打死也不要告诉别人！\n";
	
	SendEmail(reciver, content, subject);
	return 0;
}
