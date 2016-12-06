/**********************************************************
 * 本文件为定义webserver部分的相关数据定义。
 * 数据内容主要包括数据库表结构和其它辅助数据结构。
 * */

#ifndef def_h
#define def_h

#include <string.h>
#include <stdbool.h>
#include "mysql/mysql.h"

// @结构体 1 字节对齐
#pragma pack(1)

#define BUF_NAME		40
#define BUF_CHAR		200
#define BUF_TEXT		1024
#define BUF_SHORT		20

// @ 协议版本号 v1.0.0
#define PROTOCOL_VER	100

// @ 相关配置信息预定义
#define MAX_DB_NUM		1
#define MAX_TAB_NUM		7
#define	MAX_FIELD		9
#define REQ_TYPE_NUM	4

// @定义存储数据buf长度
#define FIELD			40
#define VALUE			1024

/*****************************************
 * 数据类型定义
 * */
typedef unsigned int 	uint32;
typedef unsigned char	uint8;
typedef signed int		int32;
typedef signed char		int8;
typedef signed int		status;


/*****************************************
 * 相关数据结构定义
 * */

// @协议：操作动作定义
typedef enum
{
	DB_INSERT = 1,
	DB_DELETE,
	DB_ALTER,
	DB_SEARCH
} db_opt;

// @数据库名
typedef enum
{
	DB_CSTACK = 1
} db_name_t;

// @协议：数据库表定义
typedef enum
{
	TAB_VENDOR = 1,
	TAB_SERVER,
	TAB_DEVICE,
	TAB_APP,
	TAB_DEV_STATUS,
	TAB_USER,
	TAB_SUB
} db_tab_t;

// @返回值定义
typedef enum
{
	ERR_JSON_STRUCT = -100,		// @json结构不是协议规定结构；
	FALSE = -1,					// @参数错误；
	TRUE = 1					// @运行OK；
} return_t;

// @数据库返回状态值定义
typedef enum
{
	CONN_ERR = -5,		// @连接错误
	INSERT_ERR = -4,	// @数据记录插入错误
	DEL_ERR = -3,		// @数据记录删除错误
	ALTER_ERR = -2,		// @数据记录修改错误
	SEARCH_ERR = -1,	// @数据记录查询错误
	EXEC_OK = 1			// @操作执行OK
} db_status_t;

/*****************************************
 * 数据库表结构定义
 * */
// @表：vendor
typedef struct
{
	uint8 vendor_name[BUF_CHAR];
	uint8 vendor_key[BUF_CHAR];
	uint8 vendor_description[BUF_TEXT];
}tab_vendor_t;

// @表:server
typedef struct
{
	uint8 server_name[BUF_CHAR];
	uint8 server_key[BUF_CHAR];
	uint8 server_config[BUF_CHAR];
	uint8 server_description[BUF_TEXT];
}tab_server_t;

// @表: device
typedef struct
{
	uint8 device_name[BUF_CHAR];
	uint8 device_key[BUF_CHAR];
	uint8 device_identifier[BUF_CHAR];
	uint8 device_secret[BUF_CHAR];
	uint8 device_version[BUF_SHORT];
	uint8 device_description[BUF_TEXT];
}tab_device_t;

// @表: application
typedef struct
{
	uint8 app_name[BUF_CHAR];
	uint8 app_key[BUF_CHAR];
	uint8 app_token[BUF_CHAR];
	uint8 report_url[BUF_CHAR];
	uint8 app_domain[BUF_CHAR];
	uint8 app_description[BUF_TEXT];
}tab_app_t;

// @表: device status
typedef struct
{
	bool dev_status;
} tab_dev_status_t;

// @表: user
typedef struct
{
	uint8 user_name[BUF_SHORT];
	uint8 user_password[BUF_SHORT];
	int32 user_access_level[BUF_CHAR];
	uint8 email_addr[BUF_CHAR];
	int32 phone_number[11];
}tab_user_t;

typedef struct
{
	uint8 sub_event[BUF_CHAR];
	uint8 sub_action[BUF_CHAR];
} tab_sub_t;

// @数据库信息结构定义：协议结构定义
typedef struct
{
	uint32 req_type;					// @请求类型，参见结构 db_opt
	uint32 db_name;						// @数据库名称，参见结构 db_name_t
	uint32 tab_name;					// @数据库表名称，参见结构 db_tab_t
	uint32 req_cnt;						// @数据部分中，请求记录的条数，只计数二级项目
/*	uint8  *data;						// @数据内容
	union								// @数据内容
	{
		tab_vendor_t *tab_vendor;
		tab_server_t *tab_server;
		tab_device_t *tab_device;
		tab_app_t	 *tab_app;
		tab_dev_status_t *tab_status;
		tab_user_t	 *tab_user;
		tab_sub_t	 *tab_sub;
	} tab_info_t;
*/
} db_info_t;

/*************************************************************************
 * 对于数据包的数据结构定义。
 * 用多条链表来维护json数据的结构：
 * 	1.用一条链来维护一次数据包中的对象，每个节点为一个对象链表的头节点；
 * 	2.每个对象链表头结点下，为一条数据链，对应存储每个对象的属性和值，以及值的类型等；
 * 	3.如此，概括地讲，即为：一条主链，每个节点为一条数据链表的头结点，每条数据链表存储json串中的对象的属性和值等。
 * */
// @存储字段类型
typedef enum
{
	VAL_FALSE = 0,
	VAL_TRUE = 1,
	VAL_NULL = 2,
	VAL_NUM = 3,
	VAL_STR = 4,
} val_type_t;

typedef enum
{
	DEFAULT = 0,
	PROT_INFO = 1,
	DATA_INFO = 2
} obj_type_t;

// @存放数据
typedef struct json_data
{
	uint8 *field;						// @存储字段
	uint8 *value;						// @存储字段值
	uint32 val_type;					// @存储字段类型: val_type_t

	struct json_data *next;
} json_data_t;
/*
// @存放数据
typedef struct json_data
{
	uint8 field[FIELD];					// @存储字段
	uint8 value[VALUE];					// @存储字段值
	uint8 val_type;						// @存储字段类型: val_type_t

	struct json_data *next;
} json_data_t;
 */

typedef struct linklist
{
	json_data_t *data;					// @存储 json_data_t 链的链表首地址
	obj_type_t obj_type;				// @存储数据是协议格式数据还是数据库字段信息数据？
										// @通过该标志信息，为SQL语句的组合作判断。
	struct linklist *next;
} linklist_t;


// @数据库连接指针
static MYSQL *mysql = NULL;


/******************************************************************************
 * 普通字符串链表;
 * 准备存放SQL语句。
 * */
typedef struct linklist2
{
	uint8 data[BUF_TEXT * 4];
	struct linklist2 *next;
} linklist2_t;

/******************************************************************************
 * 定义SQL查询结构；
 * 如有变动，修改此处定义和json解析过程中的数组定义以及下标。
 */
static uint8 *protocol_info[] = {"req_type", "db_name", "tab_name", "req_cnt"};
// @存放表名
static uint8 *db_tab[7] = {"vendor", "server", "device", "application", "dev_status", "user", "subscribe"};

// @存放各个表中的字段
static uint8 *field_vendor[6] = {"vendor_id", "vendor_name", "vendor_key", "vendor_description", "created_at", "updated_at"};
static uint8 *field_server[7] = {"server_id", "server_name", "server_key", "server_config", "server_description", "created_at",
								"updated_at"};
static uint8 *field_device[9] = {"device_id", "device_name", "device_key", "device_identifier", "device_secret", "device_version",
								"device_description", "created_at", "updated_at"};
static uint8 *field_app[9] = {"app_id", "app_name", "app_key", "app_token", "report_url", "app_domain", "vendor_description",
							 "created_at", "updated_at"};
static uint8 *field_status[2] = {"id", "dev_status"};
static uint8 *field_user[8] = {"user_id", "user_name", "user_password", "user_access_level", "email_addr", "phone_number",
							  "created_at", "updated_at"};
static uint8 *field_sub[5] = {"sub_id", "sub_event", "sub_action", "created_at", "updated_at"};

#endif	// end of def_h


/*
{
	"status": 1,
	"message":
	{
		"f1": "v1",
		"f2": "v2"
	},
	"server_id": "******",
	"content": "****"
};
*/








