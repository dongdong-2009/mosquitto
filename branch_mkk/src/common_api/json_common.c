/**********************************************************
 * json字符串相关的功能实现。
 * 主要json字符串的解析、格式化的实现。
 * 本部分实现依赖于库cJSON(工程中改为cjson.c)。
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_common.h"
#include "def.h"
#include "debug_log.h"
#include "db_common.h"

extern MYSQL *mysql;

/*
 * @功能		协议内容解析
 * @参数		buf_in:json数据串
 * 			len_in:传入json串的长度
 * @返回值	执行状态
 * */
int32 json_parse_protocol(uint8 *buf_in, uint32 len_in)
{
	// TODO @日志接口嘞？
	char buf_log[200] = { 0 };
	sprintf(buf_log, "[%s: %d: %s]\n", __FILE__, __LINE__, __FUNCTION__);
	print(INFO, buf_log);

	// @入参判断
	if(NULL == buf_in || 0 == len_in)
	{
		sprintf(buf_log, "[%s: %d: %s] [Parameter ERR.]\n", __FILE__, __LINE__, __FUNCTION__);
		print(ERR, buf_log);

		return 0;
	}


	/************************************************************************
	 * 变量声明定义。
	 * */
	// @数据处理: 按结构体定义格式化输出字符串；
	// @buf大小按后续实际数据测试作调整。

	db_info_t *db_info = (db_info_t *)malloc(strlen(buf_in) + 1);

    // @创建解析数据存储结构
    // @先只创建头结点吧。
    linklist_t *head = (linklist_t *)malloc(sizeof(linklist_t));
    if (NULL == head)
    	return FALSE;

    //json_data_t *data = (json_data_t *)malloc(sizeof(json_data_t));
    //if (NULL == data)
    //	return FALSE;
    // @还是手动初始化一下吧。。。
    head->data = NULL;
    //head->obj_level = 0;
    head->next = NULL;
    printf("%s: %p, %p\n", __FUNCTION__, head, head->data);

    // @解析数据
    cJSON *root = cJSON_Parse(buf_in);
    if (NULL == root)
    {
		sprintf(buf_log, "[%s: %d: %s] [cJSON_Parse ERR/data ERR.]\n", __FILE__, __LINE__, __FUNCTION__);
		print(ERR, buf_log);
    	return FALSE;
    }
    json_parse(root, head, 0, DEFAULT);

#if 1
	// for test
    printf("-------------------------------start------------------------------\n");
	linklist_t *p = head->next;
	while(p != NULL)
	{
		printf("obj_type: %d\n", p->obj_type);

		json_data_t *q = p->data;
		while (q != NULL)
		{
			printf("field: %s, value: %s, val_type: %d\n", q->field, q->value, q->val_type);
			q = q->next;
		}
		p = p->next;
	}
	printf("----------------------------end---------------------------------\n");

/*
-------------------------------start------------------------------
obj_type: 0
field: (null), value: (null), val_type: 0
obj_type: 1
field: prot_info, value: (null), val_type: 0
field: req_type, value: 1, val_type: 3
field: db_name, value: 1, val_type: 3
field: tab_name, value: 1, val_type: 3
field: req_cnt, value: 2, val_type: 3
obj_type: 2
field: data_info, value: (null), val_type: 0
field: product_name, value: pdt_01, val_type: 4
field: product_description, value: 0101, val_type: 4
field: product_key, value: 123456789, val_type: 4
field: product_config, value: hkajsh, val_type: 4
obj_type: 2
field: data_info, value: (null), val_type: 0
field: product_name, value: pdt_02, val_type: 4
field: product_description, value: 0202, val_type: 4
field: product_key, value: 987654321, val_type: 4
field: product_config, value: KLSADJKAS, val_type: 4
----------------------------end---------------------------------
*/
#endif
	linklist2_t *buf_out = (linklist2_t *)malloc(sizeof(linklist2_t));
	json_parse_data(head, buf_out);
    return TRUE;
}

/*
 * @功能		数据内容解析
 * @参数
 * @返回值
 * */
int32 json_parse(cJSON *cjson, linklist_t *head, uint32 obj_level, obj_type_t obj_type)
{
	// TODO @日志接口嘞？
	char buf_log[200] = { 0 };
	sprintf(buf_log, "[%s: %d: %s]\n", __FILE__, __LINE__, __FUNCTION__);
	print(INFO, buf_log);

	if (NULL == cjson)
	{
		sprintf(buf_log, "[%s: %d: %s] [Parameter ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		print(ERR, buf_log);

		return FALSE;
	}

	switch (cjson->type)
	{
		case cJSON_False:
		{
	    	sprintf(buf_log, "[%s: %d: %s] [case cJSON_False...]\n", __FILE__, __LINE__, __FUNCTION__);
	    	print(INFO, buf_log);

	    	// @创建节点
	    	json_data_t *node= (json_data_t *)malloc(sizeof(json_data_t));
	    	if (NULL == node)
	    		return FALSE;

	    	node->field = (uint8 *)malloc(strlen(cjson->string) + 1);
	    	if (node->field == NULL)
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [malloc ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);
		    	return FALSE;
	    	}
	    	node->value = (uint8 *)malloc(2);
	    	if (node->value == NULL)
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [malloc ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);
		    	return FALSE;
	    	}

	    	// @赋值
	    	strcpy(node->field, cjson->string);
	    	sprintf(node->value, "%d", cjson->valueint);
	    	node->val_type = VAL_FALSE;

	    	printf("%d: %s, %s; %d.\n", __LINE__, node->field, node->value, node->val_type);

	    	if (FALSE == linklist_insert(head, node))
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [linklist_insert ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);

		    	return FALSE;
	    	}
			break;
		}
		case cJSON_True:
		{
	    	sprintf(buf_log, "[%s: %d: %s] [case cJSON_True...]\n", __FILE__, __LINE__, __FUNCTION__);
	    	print(INFO, buf_log);

	    	// @创建节点
	    	json_data_t *node= (json_data_t *)malloc(sizeof(json_data_t));
	    	if (NULL == node)
	    		return FALSE;

	    	node->field = (uint8 *)malloc(strlen(cjson->string) + 1);
	    	if (node->field == NULL)
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [malloc ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);
		    	return FALSE;
	    	}
	    	node->value = (uint8 *)malloc(2);
	    	if (node->value == NULL)
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [malloc ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);
		    	return FALSE;
	    	}

	    	// @赋值
	    	strcpy(node->field, cjson->string);
	    	sprintf(node->value, "%d", cjson->valueint);
	    	node->val_type = VAL_TRUE;

	    	printf("%d: %s, %s; %d.\n", __LINE__, node->field, node->value, node->val_type);

	    	if (FALSE == linklist_insert(head, node))
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [linklist_insert ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);

		    	return FALSE;
	    	}
			break;
		}
		case cJSON_NULL:
		{
	    	sprintf(buf_log, "[%s: %d: %s] [case cJSON_Number...]\n", __FILE__, __LINE__, __FUNCTION__);
	    	print(INFO, buf_log);

	    	// @创建节点
	    	json_data_t *node= (json_data_t *)malloc(sizeof(json_data_t));
	    	if (NULL == node)
	    		return FALSE;

	    	node->field = (uint8 *)malloc(strlen(cjson->string) + 1);
	    	if (node->field == NULL)
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [malloc ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);
		    	return FALSE;
	    	}
	    	node->value = NULL;

	    	// @赋值
	    	strcpy(node->field, cjson->string);
	    	sprintf(node->value, "%d", cjson->valueint);
	    	node->val_type = VAL_NULL;

	    	printf("%d: %s, %s; %d; %d.\n", __LINE__, node->field, node->value, atoi(node->value), node->val_type);

	    	if (FALSE == linklist_insert(head, node))
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [linklist_insert ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);

		    	return FALSE;
	    	}
			break;
		}
		case cJSON_Number:
		{
	    	sprintf(buf_log, "[%s: %d: %s] [case cJSON_Number...]\n", __FILE__, __LINE__, __FUNCTION__);
	    	print(INFO, buf_log);

	    	// @创建节点
	    	json_data_t *node= (json_data_t *)malloc(sizeof(json_data_t));
	    	if (NULL == node)
	    		return FALSE;

	    	node->field = (uint8 *)malloc(strlen(cjson->string) + 1);
	    	if (node->field == NULL)
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [malloc ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);
		    	return FALSE;
	    	}
	    	node->value = (uint8 *)malloc(11);
	    	if (node->value == NULL)
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [malloc ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);
		    	return FALSE;
	    	}

	    	// @赋值
	    	strcpy(node->field, cjson->string);
	    	sprintf(node->value, "%d", cjson->valueint);
	    	node->val_type = VAL_NUM;

	    	printf("%d: %s, %s; %d; %d.\n", __LINE__, node->field, node->value, atoi(node->value), node->val_type);

	    	if (FALSE == linklist_insert(head, node))
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [linklist_insert ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);

		    	return FALSE;
	    	}

			break;
		}
		case cJSON_String:
		{
	    	sprintf(buf_log, "[%s: %d: %s] [case cJSON_String...]\n", __FILE__, __LINE__, __FUNCTION__);
	    	print(INFO, buf_log);

	    	// @创建节点
	    	json_data_t *node= (json_data_t *)malloc(sizeof(json_data_t));
	    	if (NULL == node)
	    		return FALSE;

	    	node->field = (uint8 *)malloc(strlen(cjson->string) + 1);
	    	if (node->field == NULL)
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [malloc ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);
		    	return FALSE;
	    	}
	    	node->value = (uint8 *)malloc(strlen(cjson->valuestring) + 1);
	    	if (node->value == NULL)
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [malloc ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);
		    	return FALSE;
	    	}

	    	// @赋值
	    	strcpy(node->field, cjson->string);
	    	strcpy(node->value, cjson->valuestring);
	    	node->val_type = VAL_STR;

	    	printf("%d: %s, %s; %d.\n", __LINE__, node->field, node->value, node->val_type);

	    	if (FALSE == linklist_insert(head, node))
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [linklist_insert ERR]\n", __FILE__, __LINE__, __FUNCTION__);
		    	print(ERR, buf_log);

		    	return FALSE;
	    	}
			break;
		}
		case cJSON_Array:
		case cJSON_Object:
		{
			printf("----------------------------------------------------------------------------------------------\n");
	    	sprintf(buf_log, "[%s: %d: %s] [case cJSON_Object...]\n", __FILE__, __LINE__, __FUNCTION__);
	    	print(INFO, buf_log);

	    	uint32 size = cJSON_GetArraySize(cjson);
	    	printf("%d: size = %d\n", __LINE__, size);

	    	// @防止json最外层string为空导致出现空节点的情况
	    	if (NULL != cjson->string)
	    	{
				// @创建数据链
				// @每个object,占用一个主链表的节点，该节点下创建数据链，存储object的item，即field和value.
				// @json串最外层大括号的string为空，是否创建节点？默认每个obj的string属性都不为空的，若为空，则是最外层？？
				linklist_t *main_node = (linklist_t *)malloc(sizeof(linklist_t));
				if (NULL == main_node)
					return FALSE;
				json_data_t *json_data = (json_data_t*)malloc(sizeof(json_data_t));
				if (NULL == json_data)
					return FALSE;

				// @串连节点，并赋值
				main_node->data = json_data;

				// @赋值obj_type
				if (NULL == cjson->string)
				{
					obj_type = DEFAULT;
				}
				else if (NULL != cjson->string && !strcmp(cjson->string, "prot_info"))
				{
					obj_type = PROT_INFO;
				}
				else if (NULL != cjson->string && !strcmp(cjson->string, "data_info"))
				{
					obj_type = DATA_INFO;
				}
				else
				{
					// TODO @作为数据错误来处理？
					//return FALSE;
				}

				// @赋值当前obj类型信息
				main_node->obj_type = obj_type;

				// @拷贝obj名称
				if (NULL != cjson->string)
				{
					json_data->field = (uint8 *)malloc(11);
					if (NULL == json_data->field)
						return FALSE;
					strcpy(json_data->field, cjson->string);
				}

				// @设置当前object的深度
				// TODO @之后调整 obj_level 的赋值
				obj_level++;

				// @将节点插入主链，并且采用尾插法插入。
				linklist_t *p = head;
				// @找到链表的尾
				while (NULL != p->next)
					p = p->next;
				// @插入节点
				p->next = main_node;
				main_node->next = NULL;
	    	}

	    	// @根据size大小，遍历json结构。
	    	int loop = 0;
	    	for(loop = 0; loop < size; loop++)
	    	{
		    	sprintf(buf_log, "[%s: %d: %s] [size : %d]\n", __FILE__, __LINE__, __FUNCTION__, loop);
		    	print(INFO, buf_log);

		    	// @解析一个item
	    		cJSON *sub = cJSON_GetArrayItem(cjson, loop);
	    		if (sub == NULL)
	    		{
	    	    	sprintf(buf_log, "[%s: %d: %s] [cJSON_GetArrayItem ERR]\n", __FILE__, __LINE__, __FUNCTION__);
	    	    	print(ERR, buf_log);
	    		}
	    		printf("cjson->string: %s\n", cjson->string);

	    		// @函数自调用，继续解析
	    		json_parse(sub, head, obj_level, obj_type);
	    	}

			break;
		}
		default:
			break;
	}
	return TRUE;
}

/*
-------------------------------start------------------------------
obj_type: 0
field: (null), value: (null), val_type: 0
obj_type: 1
field: prot_info, value: (null), val_type: 0
field: req_type, value: 1, val_type: 3
field: db_name, value: 1, val_type: 3
field: tab_name, value: 1, val_type: 3
field: req_cnt, value: 2, val_type: 3
obj_type: 2
field: data_info, value: (null), val_type: 0
field: product_name, value: pdt_01, val_type: 4
field: product_description, value: 0101, val_type: 4
field: product_key, value: 123456789, val_type: 4
field: product_config, value: hkajsh, val_type: 4
obj_type: 2
field: data_info, value: (null), val_type: 0
field: product_name, value: pdt_02, val_type: 4
field: product_description, value: 0202, val_type: 4
field: product_key, value: 987654321, val_type: 4
field: product_config, value: KLSADJKAS, val_type: 4
----------------------------end---------------------------------

 */
/*
 * @功能		数据内容解析;
 * @参数
 * @返回值
 * */
int32 json_parse_data(linklist_t *head, linklist2_t *buf_out)
{
	// TODO @日志接口嘞？
	char buf_log[200] = { 0 };
	sprintf(buf_log, "[%s: %d: %s]\n", __FILE__, __LINE__, __FUNCTION__);
	print(INFO, buf_log);

	// @遍历链表信息，开始整合SQL语句
	db_info_t db_info;

	linklist_t *head_tmp = head->next;
	while(head_tmp != NULL)
	{
		printf("obj_type: %d\n", head_tmp->obj_type);
		// @如果是协议头,则存储备用
		if (PROT_INFO == head_tmp->obj_type)
		{
			// @先初始化db_info，防止多条数据时的意外事故发生吧。
			memset(&db_info, 0, sizeof(db_info_t));
			json_data_t *data_tmp = head_tmp->data;
			// @计数，同时作为协议存储的顺序；
			uint32 node_cnt = 0;
			while (data_tmp != NULL)
			{
				// @节点数超出，出错返回
				if (node_cnt > 5)
				{
					sprintf(buf_log, "[%s: %d: %s] [Json Data form ERR]\n", __FILE__, __LINE__, __FUNCTION__);
					print(ERR, buf_log);
					return FALSE;
				}
				// @首个节点存储来obj的属性，不是数据信息
				if (node_cnt == 0)
				{
					data_tmp = data_tmp->next;
					node_cnt++;
					continue;
				}

				// @协议解析
				// @协议头：请求类型
				if (node_cnt == 1 && !strcmp(data_tmp->field, "req_type") && data_tmp->val_type == cJSON_Number)
				{
					db_info.req_type = atoi(data_tmp->value);
					node_cnt++;
				}
				else if (node_cnt == 2 && !strcmp(data_tmp->field, "db_name") && data_tmp->val_type == cJSON_Number)
				{
					db_info.db_name = atoi(data_tmp->value);
					node_cnt++;
				}
				else if (node_cnt == 3 && !strcmp(data_tmp->field, "tab_name") && data_tmp->val_type == cJSON_Number)
				{
					db_info.tab_name = atoi(data_tmp->value);
					node_cnt++;
				}
				else if (node_cnt == 4 && !strcmp(data_tmp->field, "req_cnt") && data_tmp->val_type == cJSON_Number)
				{
					db_info.req_cnt = atoi(data_tmp->value);
					node_cnt++;
				}
				else
				{
					// @数据错误，返回
					sprintf(buf_log, "[%s: %d: %s] [Json Data Item ERR]\n", __FILE__, __LINE__, __FUNCTION__);
					print(ERR, buf_log);

					return FALSE;
				}

				//printf("value: %d\n",atoi(data_tmp->value));

				// @下一个节点
				data_tmp = data_tmp->next;
			}

			//printf("%d: node_cnt: %d\n", __LINE__, node_cnt);
		}
		else if (DATA_INFO == head_tmp->obj_type)
		{
			//printf("[%s: %d: %s]\n", __FILE__, __LINE__, __FUNCTION__);
			// @如果是数据链表，则进入解析
			// @判断数据合法性,未按照协议封装数据，则返回错误
			if ( 0 == db_info.req_type || 0 == db_info.db_name || 0 == db_info.tab_name || 0 == db_info.req_cnt)
			{
				sprintf(buf_log, "[%s: %d: %s] [Json Data contents ERR]\n", __FILE__, __LINE__, __FUNCTION__);
				print(ERR, buf_log);
				return FALSE;
			}
			// @调用函数，拼接SQL语句;遍历链表，直到再遇到“PROT_INFO == head_tmp->obj_type”时返回，作为一条SQL语句的结束。
			//printf("%d, OK!\n", __LINE__);
			json_to_sql(&db_info, head_tmp, buf_out);
		}
		else
		{
			// @主链表的第一个数据节点应该为空，可跳过。
			printf("[%s: %d: %s] [NULL....]\n", __FILE__, __LINE__, __FUNCTION__);
		}

		head_tmp = head_tmp->next;
	}
	return TRUE;
}


/*
 * @功能		将解析的JSON数据值和item，组成SQL语句
 * @参数
 * @返回值
 * */
#if 1
int32 json_to_sql(db_info_t *db_info, linklist_t *head, linklist2_t *buf_out)
{
	// TODO @日志接口嘞？
	char buf_log[200] = { 0 };
	sprintf(buf_log, "[%s: %d: %s]\n", __FILE__, __LINE__, __FUNCTION__);
	print(INFO, buf_log);

	// @入参判断
	if (NULL == db_info || NULL == head || NULL == buf_out)
		return FALSE;
	// @
	switch (db_info->req_type)
	{
		case DB_INSERT:
		{
			sprintf(buf_log, "[%s: %d: %s] [case DB_INSERT...]\n", __FILE__, __LINE__, __FUNCTION__);
			print(INFO, buf_log);

			if (NULL == head->data)
				return FALSE;

			json_data_t *data_tmp = head->data;

			if (strcmp(data_tmp->field, "data_info"))
			{
				printf("field: %s\n", data_tmp->field);
				sprintf(buf_log, "[%s: %d: %s] [Data ERR]\n", __FILE__, __LINE__, __FUNCTION__);
				print(ERR, buf_log);

				return FALSE;
			}

			linklist2_t *sql = (linklist2_t *)malloc(sizeof(linklist2_t));
			if (NULL == sql)
				return FALSE;

			uint8 field[BUF_TEXT] = { 0 };
			uint8 val[BUF_TEXT * 2] = { 0 };

			while (NULL != data_tmp)
			{
				uint8 ch_tmp[BUF_TEXT] = { 0 };
				if (!strcmp(data_tmp->field, "data_info"))
				{
					data_tmp = data_tmp->next;
					continue;
				}
				sprintf(ch_tmp, "%s", data_tmp->field);
				strcat(field, ch_tmp);
				if (VAL_NUM == data_tmp->val_type)
				{
					sprintf(ch_tmp, "%d", atoi(data_tmp->value));
					strcat(val, ch_tmp);
				}
				else if (VAL_STR == data_tmp->val_type)
				{
					sprintf(ch_tmp, "'%s'", data_tmp->value);
					strcat(val, ch_tmp);
				}

				if (NULL != data_tmp->next)
				{
					strcat(field, ",");
					strcat(val, ",");
				}
				else
				{
					strcat(field, ", created_at");
					strcat(val, ", NULL");
				}
				data_tmp = data_tmp->next;
			}
			sprintf(sql->data, "INSERT INTO %s(%s) VALUES(%s)", db_tab[db_info->tab_name], field, val);
			printf("sql: %s\n", sql->data);
#if 0
			linklist2_t *node = buf_out;
			while (node != NULL)
				node = node->next;
			node->next = sql;
			sql->next = NULL;
#endif

			printf("-%s:%d------1--------\n", __FUNCTION__, __LINE__);

			// @调用数据库插入接口，执行SQL语句
			db_common(db_info->req_type, sql);

			break;
		}
		case DB_DELETE:
		{
			// @DELETE FORM [tab] WHERE [field] = [val]
			// @DELETE FORM [tab] WHERE [field] IN(val1, val2, val3...)
			sprintf(buf_log, "[%s: %d: %s] [case DB_DELETE...]\n", __FILE__, __LINE__, __FUNCTION__);
			print(INFO, buf_log);
#if 0
			while (head_tmp != NULL)
			{
				json_data_t *p_tmp = head_tmp->data;

				uint8 val[BUF_TEXT] = { 0 };

				uint32 val_cnt = 0;
				// @获取节点个数
				while (p_tmp != NULL && p_tmp->value != NULL)
				{
					val_cnt++;
					p_tmp = p_tmp->next;
				}
				p_tmp = head_tmp->data;

				// @<! 需要删除的数据的 value 为一个的时候：
				if (val_cnt == 1)
				{
					if (p_tmp->val_type == VAL_NUM)
					{
						sprintf(buf_sql, "DELETE FROM %s WHERE %s = %d", db_tab[db_info->tab_name], p_tmp->field, atoi(p_tmp->value));
						print(INFO, buf_sql);
					}
					else if (p_tmp->val_type == VAL_STR)
					{
						sprintf(buf_sql, "DELETE FROM %s WHERE %s = \"%s\"", db_tab[db_info->tab_name], p_tmp->field, p_tmp->value);
						print(INFO, buf_sql);
					}
					else if (p_tmp->val_type == NULL)
					{
						// TODO @此处是否要返回错误？？
					}
				}

				// @<! 需要删除的数据的 value 为多个的时候：
				if (val_cnt > 1)
				{
					// @整合 字段部分
					sprintf(val, "%s IN(", p_tmp->field);
					// @整合 字段值部分
					while (p_tmp != NULL)
					{
						uint8 tmp[BUF_TEXT] = { 0 };
						if (p_tmp->val_type == VAL_NUM)
						{
							sprintf(tmp, "%d", atoi(p_tmp->value));
						}
						else if (p_tmp->val_type == VAL_STR)
						{
							sprintf(tmp, "\"%s\"", p_tmp->value);
						}
						else if (p_tmp->val_type == NULL)
						{
							// TODO @此处是否要返回错误？？
						}

						strcat(val, tmp);

						if (p_tmp->next != NULL)
						{
							strcat(val, ", ");
						}
						// @指向下一个节点，继续整合数据
						p_tmp = p_tmp->next;
					}
					// @拼接最后的括号
					strcat(val, ")");
					// @拼SQL语句
					// @重写 多结果查询删除的
					sprintf(buf_sql, "DELETE FROM %s WHERE %s", db_tab[db_info->tab_name], val);

					print(INFO, buf_sql);
				}
				// @删除节点
				head_tmp->data = p_tmp->next;
				free(p_tmp);

				// @指向下一个节点，继续整合数据
				head_tmp = head_tmp->next;
			}
#endif
			break;
		}
		case DB_ALTER:
		{
			// @<! UPDATE [TAB] SET [FIELD] = [VALUE1] WHERE [FIELD] = [VALUE2]
			sprintf(buf_log, "[%s: %d: %s] [case DB_ALTER...]\n", __FILE__, __LINE__, __FUNCTION__);
			print(INFO, buf_log);
#if 0
			while (head_tmp != NULL)
			{
				json_data_t *p_tmp = head_tmp->data;
				if (p_tmp->val_type == VAL_NUM)
				{
					sprintf(buf_sql, "UPDATE %s SET %s = %d", db_tab[db_info->tab_name], p_tmp->field, atoi(p_tmp->value));
					print(INFO, buf_sql);
				}
				else if (p_tmp->val_type == VAL_STR)
				{
					sprintf(buf_sql, "DELETE FROM %s WHERE %s = \"%s\"", db_tab[db_info->tab_name], p_tmp->field, p_tmp->value);
					print(INFO, buf_sql);
				}
				else if (p_tmp->val_type == NULL)
				{
					// TODO @此处是否要返回错误？？
				}

				// @指向下一个节点，继续整合数据
				head_tmp = head_tmp->next;
			}
#endif
			break;
		}
		case DB_SEARCH:
		{
			// @分为一般查询某个字段+值的形式、某几个字段+值、联合查询，以及“*”查询等
			// TODO @程序需要分条件处理，而通信协议组成影响解析，先确定协议吧。

			sprintf(buf_log, "[%s: %d: %s] [case DB_SEARCH...]\n", __FILE__, __LINE__, __FUNCTION__);
			print(INFO, buf_log);

			if (NULL == head->data)
				return FALSE;

			json_data_t *data_tmp = head->data;

			if (strcmp(data_tmp->field, "data_info"))
			{
				printf("field: %s\n", data_tmp->field);
				sprintf(buf_log, "[%s: %d: %s] [Data ERR]\n", __FILE__, __LINE__, __FUNCTION__);
				print(ERR, buf_log);

				return FALSE;
			}

			linklist2_t *sql = (linklist2_t *)malloc(sizeof(linklist2_t));
			if (NULL == sql)
				return FALSE;

			uint8 field[BUF_TEXT] = { 0 };
			uint8 val[BUF_TEXT * 2] = { 0 };

			while (NULL != data_tmp)
			{
				uint8 ch_tmp[BUF_TEXT] = { 0 };
				if (!strcmp(data_tmp->field, "data_info"))
				{
					data_tmp = data_tmp->next;
					continue;
				}
				sprintf(ch_tmp, "%s", data_tmp->field);
				strcat(field, ch_tmp);
				if (VAL_NUM == data_tmp->val_type)
				{
					sprintf(ch_tmp, "%d", atoi(data_tmp->value));
					strcat(val, ch_tmp);
				}
				else if (VAL_STR == data_tmp->val_type)
				{
					sprintf(ch_tmp, "'%s'", data_tmp->value);
					strcat(val, ch_tmp);
				}

				if (NULL != data_tmp->next)
				{
					strcat(field, ",");
					strcat(val, ",");
				}
				data_tmp = data_tmp->next;
			}

			sprintf(sql->data, "SELECT %s FROM %s", field, db_tab[db_info->tab_name]);
			printf("sql: %s\n", sql->data);
#if 0
			linklist2_t *node = buf_out;
			while (node != NULL)
				node = node->next;
			node->next = sql;
			sql->next = NULL;
#endif

			printf("-%s:%d------1--------\n", __FUNCTION__, __LINE__);

			// @调用数据库插入接口，执行SQL语句
			db_common(db_info->req_type, sql);
			break;
		}
		default:
		{
			// @ERROR
			return FALSE;
		}
	}
	return TRUE;
}
#endif

// @链表操作

/*
 * @功能		将解析的JSON数据值和item，组成SQL语句
 * @参数		head: 链表头结点；
 * 			data: 要查入的节点。
 * @返回值	void
 * */

int32 linklist_insert(linklist_t *head, json_data_t *data)
{
	// @还是先做个判断吧，比较好判断返回值。
	if (NULL == head->next)
		return FALSE;

	linklist_t *tmp = head->next;
	while (NULL != tmp->next)
		tmp = tmp->next;

	json_data_t *data_tmp = tmp->data;
	while (NULL != data_tmp && NULL != data_tmp->next)
		data_tmp = data_tmp->next;

	data_tmp->next = data;
	data->next = NULL;

	printf("%s: end.\n", __FUNCTION__);

	return TRUE;
}





/*
 * @功能		获取MYSQL指针
 * @参数
 * @返回值
 * */
MYSQL* db_stat()
{
	if (mysql != NULL)
		return mysql;

	mysql = (MYSQL *)malloc(sizeof(MYSQL));
	if (NULL == mysql)
	{
		return NULL;
	}
	return mysql;
}















