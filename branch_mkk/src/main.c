#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <log/zlog.h>
#include <common/json.h>
#include <tbtools/topsdk.h>

#include "ideahos_xgo_server.h"
#include <data_type.h>

/*********************************************************
 * 功能描述	：服务入口
 * 输入参数	：无
 * 输出参数	：待定
 * 返回值	：0
 * 作者		：mkk
 * 修改日期	：2016-11-25
 **********************************************************/
int main(void){
	int ret;

	system("clear");
    printf("\33[47m\33[32m***************************************************\33[0m\n");
    printf("\33[47m\33[32m*            浩思智慧XGO产品测试程序              *\33[0m\n");
    printf("\33[47m\33[32m*                 海豚实验室                      *\33[0m\n");
    printf("\33[47m\33[32m***************************************************\33[0m\n");

	//获取服务控制结构
	ret = ideahos_server_data_create(&haosi_server_ptr);
	if(ret == NO_MEMORY){
		perror("Create server_data fail");
		exit(1);
	}
	printf("\33[32m[%s:%d] Haosi server_data init success\33[0m\n",__FILE__,__LINE__);


	//1、日志系统初始化
	haosi_server_ptr->log_init_state = haosi_server_ptr->log_init(&haosi_server_ptr->zc);
	if(haosi_server_ptr->log_init_state == LOG_READ_CONFIG_ERROR){
		perror("Zlog read config faili()");
		ideahos_server_data_destory(haosi_server_ptr);
		exit(1);
	}
	else if(haosi_server_ptr->log_init_state == LOG_GET_CATEGORY_ERROR){
		perror("Zlog get category fail()");
		ideahos_server_data_destory(haosi_server_ptr);
		exit(1);
	}
	haosi_server_ptr->log_init_state = OK;
	zlog_info(haosi_server_ptr->zc,"XGO server main id %d",getpid());
	zlog_info(haosi_server_ptr->zc,"Zlog init success.");

#if 1
	//2、LWAN网关服务启动
	haosi_server_ptr->lwan_state = haosi_server_ptr->lwan_start();
	if(haosi_server_ptr->lwan_state == LWAN_THREAD_CREATE_ERROR){
		ideahos_server_data_destory(haosi_server_ptr);
	}
	haosi_server_ptr->lwan_state = OK;
	zlog_info(haosi_server_ptr->zc,"Lwan server id %d",haosi_server_ptr->lwan_pid);
	zlog_info(haosi_server_ptr->zc,"Lwan server run ok.");
#endif

#if 1
	//3、MQTT代理服务启动
	haosi_server_ptr->mqtt_agent_state = haosi_server_ptr->mqtt_agent_start();
	if(haosi_server_ptr->mqtt_agent_state == NO_MEMORY){
		zlog_info(haosi_server_ptr->zc,"Mqtt have no enough merrory to init.");
		ideahos_server_data_destory(haosi_server_ptr);
		exit(1);
	}else if(haosi_server_ptr->mqtt_agent_state == MQTT_AGIENT_INIT_ERROR){
		zlog_info(haosi_server_ptr->zc,"Mqtt init network argement fail.");
		ideahos_server_data_destory(haosi_server_ptr);
		exit(1);
	}else if(haosi_server_ptr->mqtt_agent_state == MQTT_AGIENT_LISTEN_ERROR){
		zlog_info(haosi_server_ptr->zc,"Mqtt listen error.");
		ideahos_server_data_destory(haosi_server_ptr);
		exit(1);
	}else if(haosi_server_ptr->mqtt_agent_state == MQTT_AGINET_PROGRESS_ERROR){
		zlog_info(haosi_server_ptr->zc,"Mqtt create progress fail.");
		ideahos_server_data_destory(haosi_server_ptr);
		exit(1);
	}
	zlog_info(haosi_server_ptr->zc,"Mqtt agient pid is %d",haosi_server_ptr->mqtt_agent_pid);
	zlog_info(haosi_server_ptr->zc,"Mqtt agient run ok.");
#endif

//下面测试进程收尸,对所有创建的服务应该另外起一个进程，\
	对其他进程进行监视，如果异常终止，进行资源回收
	haosi_server_ptr->xgo_monitor_pid = fork();
	if(haosi_server_ptr->xgo_monitor_pid < 0)
	{
		zlog_info(haosi_server_ptr->zc,"xgo monitor creater falied");
		exit(1);
	}
	else if(haosi_server_ptr->xgo_monitor_pid == 0)
	{
		//启动监视服务
		haosi_server_ptr->xgo_monitor();
		return 0;
	}

	exit(0);
#if 0
	int ret;
    char randomNum[6];
    STMESSAGE sendMSG;
    char *cmdSend;
    char ID2[18];
    char name[20];
    char getName;
    char date[100];
<<<<<<< .mine
	sendMSG.rec_num = "18600260684";
    //sendMSG.rec_num = "18600458483";
=======

    sendMSG.rec_num = "18600260684";
>>>>>>> .r55
    generatNum(randomNum, 6);
    printf("randomNum is %s\n",randomNum);

    cJSON *root;
    root = cJSON_CreateObject();  
    cJSON_AddItemToObject(root, "json_display", cJSON_CreateString("This json string is test json api"));
    cJSON_AddItemToObject(root, "number", cJSON_CreateString(randomNum));
    sendMSG.sms_param = cJSON_Print(root);
    printf("%s\n",sendMSG.sms_param);
<<<<<<< .mine
=======
//   testMessageSend(&sendMSG);
>>>>>>> .r55

	
	//testMessageSend(&sendMSG);
	//testHttps();
	//testTrans("I am xiaokunge");
    int rc;
	zlog_category_t *zc;

	rc = zlog_init("/mnt/hgfs/share/sdk-cloudStack/src/config/cslog.conf");
	if (rc) {
		printf("init failed\n");
		return -1;
	}

	zc = zlog_get_category("my_cat");
	if (!zc) {
		printf("get cat fail\n");
		zlog_fini();
		return -2;
	}

	zlog_info(zc, "hello, cstack");

	zlog_fini();	//关闭zlog功能	

	system("clear");
    printf("                               \33[47m\33[32m***************************************************\33[0m\n");
    printf("                               \33[47m\33[32m*            浩思智慧XGO产品测试程序              *\33[0m\n");
    printf("                               \33[47m\33[32m*                 海豚实验室                      *\33[0m\n");
    printf("                               \33[47m\33[32m***************************************************\33[0m\n");

    printf("请输入17位设备id2...\n");
    gets(ID2);
LOOP1:
    printf("请选择事件...\n");
    printf("1 setcustomaction(语音事件)\n");
    printf("2 nodeDataEvent(数据事件)\n");
    printf("q EXIT(退出)\n");
    scanf("%s",&getName);
    getchar();
    switch(getName)
    {
        case('1'): strcpy(name, "setcustomaction"); break;
        case('2'): strcpy(name, "nodeDataEvent"); break;
        case('q'): return 0;
        default:printf("请输入正确的格式!\n");goto LOOP1;break;
    }
    printf("请输入下发的数据...\n");
    gets(date);

    cJSON *cmdJson;
    cJSON *sonCmdJson;
    char *sonCmdSend;
    cmdJson = cJSON_CreateObject();
    cJSON_AddItemToObject(cmdJson, "id2", cJSON_CreateString(ID2));
    cJSON_AddItemToObject(cmdJson, "service_id", cJSON_CreateString("HAOSI_23383927_new"));
    cJSON_AddItemToObject(cmdJson, "type", cJSON_CreateString("action"));
    cJSON_AddItemToObject(cmdJson, "name", cJSON_CreateString(name));
    sonCmdJson = cJSON_CreateObject();
    cJSON_AddItemToObject(sonCmdJson, "value", cJSON_CreateString(date));

    cJSON_AddItemToObject(cmdJson, "args", sonCmdJson);
    cmdSend = cJSON_Print(cmdJson);
    printf("%s\n",cmdSend);
    testHttps();
/*    testMessageSend(&sendMSG);*/
    testTrans(cmdSend);
    goto LOOP1;

    return 0;
#endif
}

