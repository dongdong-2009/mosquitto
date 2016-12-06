/**********************************************************
 * json字符串相关的功能实现。
 * 主要json字符串的解析、格式化的实现。
 * 1. 对于json串的解析,先解析出操作，然后根据操作再解析 数据部分，同时将解析的数据拼接为SQL 语句。
 * 本部分实现依赖于库cJSON(工程中改为cjson.c)。
 * */

#ifndef json_common_h
#define json_common_h

#include "cjson.h"
#include "def.h"
#include "mysql/mysql.h"

/*
 * @功能		协议内容解析
 * @参数		buf_in:json数据串
 * 			len_in:传入json串的长度
 * @返回值	执行状态
 * */
int32 json_parse_protocol(uint8 *buf_in, uint32 len_in);

/*
 * @功能		数据内容解析
 * @参数
 * @返回值
 * */
//int32 json_parse_data(db_info_t *db_info, linklist_t *head, linklist2_t *buf_out);
int32 json_parse_data(linklist_t *head, linklist2_t *buf_out);


/*
 * @功能		数据内容解析
 * @参数
 * @返回值
 * */
int32 json_parse(cJSON *sub, linklist_t *head, uint32 obj_level, obj_type_t obj_type);


/*
 * @功能		将解析的JSON数据值和item，组成SQL语句
 * @参数
 * @返回值
 * */
//int32 json_to_sql(db_info_t *db_info, uint8 *str_field, uint8 *str_val, uint8 *buf_out);
int32 json_to_sql(db_info_t *db_info, linklist_t *head, linklist2_t *buf_out);


/*
 * @功能		获取MYSQL指针
 * @参数
 * @返回值
 * */
//status db_stat();
MYSQL* db_stat();


/*e
 * @功能		将字符串buf中的最后一次出现的‘,’替换为空格
 * @参数
 * @返回值
 * */
int32 str_replace(uint8 *buf);


/*
 * @功能		格式化数据
 * @参数
 * @返回值
 * */
int32 json_packet_data();


/*
 * @功能		格式化协议格式
 * @参数
 * @返回值
 * */
int32 json_packet_protocol();

/*
 * @功能		格式化协议格式
 * @参数
 * @返回值
 * */
int32 db_operation(uint8 db_flag);

/*
 * @功能		将解析的JSON数据值和item，组成SQL语句
 * @参数		head: 链表头结点；
 * 			data: 要查入的节点。
 * @返回值	void
 * */

int32 linklist_insert(linklist_t *head, json_data_t *data);


#endif	//end of json_common_h





