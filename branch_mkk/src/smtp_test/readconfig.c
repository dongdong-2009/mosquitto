/*** ***************************************
** Author		 :  周利民
** Last Modified :  2016-09-27 16:54
** Filename		 :  readconfig.c
** Description   :  本文件主要实现从配置文件读取指定keyname的参数值
			
** Other		 :
** Function List :	l_trim()  函数功能是删除字符串中左边的空格
					r_trim()  函数功能是删除字符串中右边的空格
					a_trim()  函数功能是删除字符串中两边的空格
					GetConfigfileString()		 函数功能是获取配置文件中指定的参数值
** History		 :
** ****************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
//#include "readconfig.h"
#define KEYVALLEN 100	//每行最多100个字符
 
/*   删除左边的空格   */
char * l_trim(char * szOutput, const char *szInput)
{
	assert(szInput != NULL);
	assert(szOutput != NULL);
	assert(szOutput != szInput);
	for(NULL; *szInput != '\0' && isspace(*szInput); ++szInput)
	{
		;
	}
	return strcpy(szOutput, szInput);
}
 
/*   删除右边的空格   */
char *r_trim(char *szOutput, const char *szInput)
{
	char *p = NULL;
	assert(szInput != NULL);
	assert(szOutput != NULL);
	assert(szOutput != szInput);
	strcpy(szOutput, szInput);
	for(p = szOutput + strlen(szOutput) - 1; p >= szOutput && isspace(*p); --p)
	{
		;
	}
	*(++p) = '\0';
	return szOutput;
}
 
/*   删除两边的空格   */
char * a_trim(char * szOutput, const char * szInput)
{
	char *p = NULL;
	assert(szInput != NULL);
	assert(szOutput != NULL);
	l_trim(szOutput, szInput);
	for(p = szOutput + strlen(szOutput) - 1;p >= szOutput && isspace(*p); --p)
	{
		;
	}
	*(++p) = '\0';
	return szOutput;
}
 
 
int GetConfigfileString(char *profile, char *AppName, char *KeyName, char *KeyVal )
{
	char appname[32],keyname[32];
	char *buf,*c;
	char buf_i[KEYVALLEN], buf_o[KEYVALLEN];
	FILE *fp;
	int found=0; /* 1 AppName 2 KeyName */
	//printf("[zhou limin] [000]this code running ok!!!!!!!\n");
	if((fp=fopen(profile,"r")) == NULL)
	{
		printf( "openfile [%s] error\n",profile);
		return(-1);
	}
	fseek(fp, 0, SEEK_SET); //设置指针在文件开头，而不是尾
	memset(appname, 0, sizeof(appname));
	sprintf(appname,"[%s]", AppName );
	while(!feof(fp) && fgets(buf_i, KEYVALLEN, fp)!=NULL)	//每次读取配置文件中一行数据保存到buf_i所指向的空间，且保证是本文件内容
	{
		//printf("[zhou limin] [001]this code running ok!!!!!!!\n");
		l_trim(buf_o, buf_i);
		if(strlen(buf_o) <= 0 )
			continue;
		buf = NULL;
		buf = buf_o;
		 
		if(found == 0)
		{
			//printf("[zhou limin] [002]this code running ok!!!!!!!\n");
			if(buf[0] != '[') 
			{
				continue;
			} 
			else if (strncmp(buf,appname,strlen(appname))==0)
			{
				found = 1;
				continue;
			}
		} 

		if(found == 1)
		{
			//printf("[zhou limin] [003]this code running ok!!!!!!!\n");
			if(buf[0] == '#') //如果本行是注释行，结束本次循环开始检查下一行
			{
				continue;
			} 
			else if(buf[0] == '[') //如果本行是AppName行，结束本次循环开始检查下一行
			{
				//出现另一个appname，说明本appname的内容已经结束
				printf("[empty]this appname in config file is empty!!!!!!!\n");
				break;
			} 
			else 
			{
				//printf("[zhou limin] [004]this code running ok!!!!!!!\n");
				if((c = (char*)strchr(buf, '=')) == NULL)
				{
					//本行是空行，结束本次循环开始检测下一行
					continue;	
				}
				memset(keyname, 0, sizeof(keyname));
				sscanf(buf, "%[^=|^ |^\t]",keyname);	////???????????????????
				if(strcmp(keyname, KeyName) == 0 )
				{
					printf("check key_name is sucess,keyname is %s!!!\n",keyname);
					sscanf(++c, "%[^\n]", KeyVal);
					char *KeyVal_o = (char *)malloc(strlen(KeyVal) + 1);
					if(KeyVal_o != NULL)
					{
						memset(KeyVal_o, 0, sizeof(KeyVal_o));
						a_trim(KeyVal_o, KeyVal);
						if(KeyVal_o && strlen(KeyVal_o) > 0)
							strcpy(KeyVal, KeyVal_o);
						free(KeyVal_o);
						KeyVal_o = NULL;
					}
					found = 2;
					break;
				} 
				else 
				{
					continue;
				}
			}
		}
	}
	fclose(fp);
	if(found == 2)
		return(0);
	else
		return(-1);
}

#if 0
void main()
{
	char ip[32] = {0};
	GetConfigfileString("./config/proj.conf", "smtp_server", "server_ip", ip);
	printf("%s\n",ip);
}
#endif



