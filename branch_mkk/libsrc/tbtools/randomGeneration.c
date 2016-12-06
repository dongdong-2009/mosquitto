#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "randomGeneration.h"
/*********************************************************
功能描述：获取随机数验证码
输入参数：ptr:存储地址指针；len:验证码长度
输出参数：验证码
返回值：0：输入参数错误；1:函数正常运行
修改日期：20160824
作者：Loya
*/
int generatNum(char *ptr, char len)
{
    unsigned char i, getNum = 0;
    char *input;
    if(ptr == NULL)
    {
        printf("[ERROR]:The para input wrong!\n");
        return 0;
    }

    input = ptr;
    srand(time(NULL));
    for(i=0; i<len; i++)
    {
        getNum = rand()%10;
        *input = getNum + 48;
        input++;
    }
    return 1;
}
