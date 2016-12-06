/**********************************************************
 * 本文件主要为封装调试日志信息的输出控制。
 * 日志等级分为[INFO] [WARNING] [ERR]三级。
 * */

#include <stdio.h>
#include "debug_log.h"
#include "def.h"

void print(log_type_t flag, char *log)
{
#ifdef LOG_INFO
	if (INFO == flag)
	{
		printf("%s", log);
	}
#endif
#ifdef LOG_WARNING
	if (WARNING == flag)
	{
		printf("%s", log);
	}
#endif
#ifdef LOG_ERROR
	if (ERR == flag)
	{
		printf("%s", log);
	}
#endif
}
