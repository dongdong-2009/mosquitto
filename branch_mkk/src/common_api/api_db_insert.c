
/**********************************************************
 * 数据库插入功能接口定义。
 * 主要涉及数据库不同表的插入接口的实现。
 * */
#include <stdio.h>
#include <stdlib.h>
#include "api_db_insert.h"
#include "debug_log.h"


/*
 * @func: 	解析数据库插入功能数据
 * @param:
 * ＠return:
 */
int db_insert(char *buf_in, int len_in)
{
	if (NULL == mysql)
	{
		printf("[ERR] [%s : %d : %s] [MYSQL sock is nul.]\n", __FILE__, __LINE__, __FUNCTION__);
		return CONN_ERR;
	}
	// TODO @解析 "request->header.body->value" JSON格式数据
	printf("[INFO] [%s : %d : %s] [Data: %d:%s]\n", __FILE__, __LINE__, __FUNCTION__, len_in, buf_in);

	// @设置查询编码
	mysql_query(mysql, "set names utf8");

	// @执行SQL语句
    int res = mysql_query(mysql, buf_in);

    // @返回值非零则出错
    if (res)
    {
        printf("[ERR] [%s : %d : %s] [Insert error %d: %s.]\n", __FILE__, __LINE__,__FUNCTION__,
                (mysql_errno(mysql)), mysql_error(mysql));
        return INSERT_ERR;
    }
    printf("[info] [%s : %d : %s] [Inserted %lu rows.]\n", __FILE__, __LINE__, __FUNCTION__,
           (unsigned long)mysql_affected_rows(mysql));

    // TODO @准备response

	return EXEC_OK;
}

/*
 * @func: 	返回值封包
 * @param:
 * ＠return:
 */
int rsp_insert()
{
	return TRUE;
}








