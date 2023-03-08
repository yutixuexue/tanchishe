#include"global_variable.h"

extern int snakeLevel;
extern int snakeLength;

/*
 * 功能  ：计分系统线程入口
 * 参数  ：无
 * 返回值：无
 */
void score_thread()
{
	printf("--计分系统载入成功！\n");
	
	char scoreBuff[8] = "0";

	dev_zlg7290_fd = open(DEV_ZLG7290, O_RDWR);
	if(dev_zlg7290_fd < 0)
	{
		printf("open %s error\n", DEV_ZLG7290);
		return;
	}

	while(1)
	{
		itoa(snakeLength-INITLENGTH,scoreBuff,10);
		itoa(snakeLevel,scoreBuff + 4,10);
		ioctl(dev_zlg7290_fd, SET_VAL, scoreBuff);
		usleep(10000);
	}
}

/*
 * 功能  ：int型转char型
 * 参数  ：需要转制的int型数据;接收转换后字符;进制选择
 * 返回值：无
 */
void itoa(int num, char *str, int radix)
{
	char index[]="0123456789ABCDEF";
	unsigned unum;//中间变量
	int i=0,j=0,k=0;
	
	/*确定unum的值*/
	if(radix==10&&num<0)//十进制负数
	{
		unum=(unsigned)-num;
		str[i++]='-';
	}
	else unum=(unsigned)num;//其他情况
	
	/*转换*/
	do{
		str[i++]=index[unum%(unsigned)radix];
		unum/=radix;
	}
	while(unum);
	str[i]='\0';
	
	/*逆序*/
	if(str[0]=='-')
		k=1;//十进制负数
	char temp;
	for(j=k;j<=(i-1)/2;j++)
	{
		temp=str[j];
		str[j]=str[i-1+k-j];
		str[i-1+k-j]=temp;
	}
}
