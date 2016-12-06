/**********************************************************
 * 数据库通用操作。
 * 主要涉及数据库登录退出等的实现。
 * */
#ifndef db_common_h
#define db_common_h

#include "mysql/mysql.h"
#include "def.h"

/**************************************
 * Data structure
 */

/**************************************
 * Function
 */

/*
 * @func: 	连接数据库
 * @param:	主机地址；用户名；密码；(要链接的)数据库
 * @return:	数据库描述符
 */
uint32 db_connect(const int8 *host, const int8 *user, const int8 *passwd, const int8 *database);

/*
 * @func: 	断开数据库连接
 * @param:	数据库描述符
 * @return:	操作状态
 */
uint32 db_disconnect();


/*
 * @func: 	数据库接口
 * @param:	opt: 操作量；
 * 			sql: SQL语句集
 * @return:	执行状态
 */
uint32 db_common(db_opt opt, linklist2_t *sql);


#endif	//end of db_common_h






