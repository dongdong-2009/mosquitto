/**********************************************************
 * 数据库通用操作。
 * 主要涉及数据库登录退出等的实现。
 * */
#include <stdio.h>
#include "db_common.h"
#include "def.h"
#include "mysql/mysql.h"

extern MYSQL *mysql;

/*
 * @func: 	连接数据库
 * @param:	主机地址；用户名；密码；(要链接的)数据库
 * @return:	数据库描述符
 */
uint32 db_connect(const int8 *host, const int8 *user, const int8 *passwd, const int8 *database)
{
	printf("[INFO] [%s : %d : %s]\n", __FILE__, __LINE__, __FUNCTION__);

    // @初始化数据库连接
    mysql = db_stat();

    if (NULL == mysql)
    	return FALSE;

	// 初始化
    mysql_init(mysql);
	printf("[INFO] [%s : %d : %s] [mysql_init....]\n", __FILE__, __LINE__, __FUNCTION__);
    // 连接
    if (NULL == mysql_real_connect(mysql, host, user, passwd, database, 0, NULL, 0))
    {
    	printf("[ERR] [%s : %d : %s] [Connection failed.]\n", __FILE__, __LINE__, __FUNCTION__);
        if (mysql_error(mysql))
        {
            printf("[ERR] [%s : %d : %s] [Connection error %d: %s]\n", __FILE__, __LINE__, __FUNCTION__,
                    mysql_errno(mysql), mysql_error(mysql));
        }
        return FALSE;
    }
    printf("[INFO] [%s : %d : %s] [MYSQL connect OK.]\n", __FILE__, __LINE__, __FUNCTION__);

	return TRUE;
}

/*
 * @func: 	断开数据库连接
 * @param:	数据库描述符
 * @return:	操作状态
 */
uint32 db_disconnect()
{
	if (NULL == mysql)
	{
		printf("[ERR] [%s : %d : %s] [MYSQL sock is null.]\n", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	mysql_close(mysql);
	free(mysql);
	mysql = NULL;

	return TRUE;
}

/*
 * @func: 	数据库接口
 * @param:	opt: 操作量；
 * 			sql: SQL语句集
 * @return:	执行状态
 */
uint32 db_common(db_opt opt, linklist2_t *sql)
{
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;
	MYSQL_FIELD *fd;

	if (NULL == mysql)
	{
		// TODO @若为空，重连数据库？
		printf("[ERR] [%s : %d : %s] [MYSQL sock is nul.]\n", __FILE__, __LINE__, __FUNCTION__);
		return CONN_ERR;
	}

	if (NULL == sql)
		return FALSE;

	// @执行SQL语句
	linklist2_t *tmp = sql;
	while (NULL != tmp)
	{
		printf("[INFO] [%s : %d : %s] [Data: %s]\n", __FILE__, __LINE__, __FUNCTION__, tmp->data);

		// @设置查询编码
		mysql_query(mysql, "set names utf8");

		// @执行SQL语句
		int res = mysql_query(mysql, tmp->data);

		// @返回值非零则出错
		if (res)
		{
			printf("[ERR] [%s : %d : %s] [Insert error %d: %s.]\n", __FILE__, __LINE__,__FUNCTION__,
					(mysql_errno(mysql)), mysql_error(mysql));
			return INSERT_ERR;
		}
		//printf("[info] [%s : %d : %s] [Affected %lu rows.]\n", __FILE__, __LINE__, __FUNCTION__,
		//	   (unsigned long)mysql_affected_rows(mysql));

		/////////////////////////////////////////////////////////////////////////////////////////////
		// TODO @准备response
		printf("opt = %d\n", opt);
		switch (opt)
		{
			case DB_INSERT:
			case DB_DELETE:
			case DB_ALTER:
			{
				break;
			}
			case DB_SEARCH:
			{
				// @读取出上次操作的结果集
				res_ptr = mysql_store_result(mysql);

				if (NULL != res_ptr)
				{
					// @获取查询结果集中的field的值
					int32 field_num = mysql_num_fields(res_ptr);
					printf("[info] [%s : %d : %s] [Affected %d rows. resnum = %d]\n", __FILE__, __LINE__, __FUNCTION__,
							/*(unsigned long)*/mysql_affected_rows(res_ptr), field_num);
					// @逐行获取结果
					while ((sqlrow = mysql_fetch_row(res_ptr)))
					{
						int loop;
						for (loop = 0; loop < field_num; loop++)
						{
							printf("%s\t", sqlrow[loop]);
						}
						printf("\n");
					}
				}
				// @释放结果集
				mysql_free_result(res_ptr);

				break;
			}
			default:
			{
				printf("[ERR] [%s: %d: %s]\n", __FILE__, __FUNCTION__, __LINE__);
				break;
			}
		}

		// @指向下一个节点
		tmp = tmp->next;
	}

	return EXEC_OK;
}








