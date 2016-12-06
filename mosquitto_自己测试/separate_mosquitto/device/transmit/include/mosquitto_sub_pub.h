#ifndef __MOSQUITTO_SUB_PUB_H
#define __MOSQUITTO_SUB_PUB_H

typedef struct pub_data{
	char *ip;			//要连接的broker的ip地址
	char *port;			//要连接的broker的端口
	char *topic;		//发布到的主题
	char *msg;			//发布的消息
	char *qos;			//指定服务质量等级
}Pub_Data;

typedef struct sub_data{
	char *ip;
	char *port;
	char *topic;
	char *qos;
}Sub_Data;

//int client_pub(int argc,char **argv);

/*订阅消息*/

/*
 * 功能：初始化订阅客户端配置信息，包括主题、要连接的broker等
 * 作者：穆昆空
 * 时间：2016-10-11
 * 返回值
 *		0：成功
 *		1：参数非法
 *		2：其它
 */
int mosquitto_sub_init(Sub_Data *sub_s);

/*
 * 功能：生成与broker连接的句柄
 * 作者：穆昆空
 * 时间：2016-10-11
 * 返回值
 *		0：成功
 *		1：失败
 */
int mosquitto_sub_create(void);

/*
 * 功能：连接broker
 * 作者：穆昆空
 * 时间：2016-10-11
 * 返回值
 *		0：连接成功
 *		1：连接失败
 */
int mosquitto_sub_connect(void);

/*
 * 功能：订阅主题，循环等待服务器的消息推送
 * 作者：穆昆空
 * 时间：2016-10-11
 * 返回值：void
 */
void mosquitto_sub_start(int (*publish_callback)(char *str));

/*
 * 功能：订阅异常结束，资源回收
 * 作者：穆昆空
 * 时间：2016-10-11
 * 返回值：void
 */
void mosquitto_sub_destory(void);




/*发布消息*/

/*
 * 功能：初始化发布用户配置信息，包括发布的主题、内容等
 * 作者：穆昆空
 * 时间：2016-10-10
 * 返回值
 *		0：成功
 *		1：参数非法
 *		2：生成客户端id失败
 */
int mosquitto_pub_init(Pub_Data *pub_s);

/*
 * 功能：生成连接broker的句柄
 * 作者：穆昆空
 * 时间：2016-10-10
 * 返回值
 *		0：成功
 *		1：内存不足
 *		-1：其它
 */
int mosquitto_pub_create(void);

/*
 * 功能：连接broker
 * 作者：穆昆空
 * 时间：2016-10-10
 * 返回值
 *		0：成功
 *		1：失败
 */
int mosquitto_pub_connect(void);

/*
 * 功能：发布消息至broker
 * 作者：穆昆空
 * 时间：2016-10-10
 * 返回值
 *		0：成功
 *		1：失败
 */
int mosquitto_pub_publish(void);

/*
 * 功能：资源回收
 */
void mosquitto_pub_destory(void);

#endif
