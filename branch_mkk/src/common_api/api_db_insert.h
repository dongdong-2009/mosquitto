/**********************************************************
 * 数据库插入功能接口定义。
 * 主要涉及数据库不同表的插入接口的实现。
 * */
#ifndef api_db_insert_h
#define api_db_insert_h

#include "db_common.h"

/*
 * @func: 	解析数据库插入功能数据
 * @param:
 * ＠return:
 */
int db_insert(char *buf_in, int len_in);

int rsp_insert();

#endif	//end of api_db_insert_h

