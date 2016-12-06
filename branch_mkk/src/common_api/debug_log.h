/**********************************************************
 * 本文件主要为封装调试日志信息的输出控制。
 * 日志等级分为[INFO] [WARNING] [ERR]三级。
 * */

#ifndef debug_log_h
#define debug_log_h

#define		LOG_INFO
#define		LOG_WARNING
#define		LOG_ERROR

typedef enum
{
	INFO = 1,
	WARNING,
	ERR
} log_type_t;

/*
 * @功能		输出日志信息
 * @参数		flag: 输出类型
 * 			log: 输出内容
 * @返回值	void
 * */
void print(log_type_t flag, char *log);

#endif	// @end of debug_log_h
