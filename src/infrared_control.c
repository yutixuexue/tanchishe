#include"global_variable.h"

extern char snakeMove;
extern char snakeState;
extern char options;
extern int snakeLevel;

/*红外遥控键值结构体*/
typedef struct key {
		unsigned char key;
		char key_desc;
}key1_t;

/*红外遥控键值表*/
key1_t table[] = {
	{KEY_1, '1'},
	{KEY_2, '2'},
	{KEY_3, '3'},
	{KEY_4, '4'},
	{KEY_5, '5'},
	{KEY_6, '6'},
	{KEY_7, '7'},
	{KEY_8, '8'},
	{KEY_9, '9'},
	{KEY_UP, 'U'},
	{KEY_DOWN, 'D'},
	{KEY_LEFT, 'L'},
	{KEY_RIGHT, 'R'},
	{KEY_ENTER, 'O'},
};


/*
 * 功能  ：红外遥控器指令接收及处理
 * 参数  ：无
 * 返回值：无
 */
void infrared_control_thread()
{
	printf("--控制台接入完毕！\n");
	
	int i = 0;
	char key_old = 'L';//记录上一个移动方向
	struct input_event ev;
	
	dev_ir_fd = open(DEV_IR,O_RDONLY|O_NOCTTY|O_NDELAY);
	if (dev_ir_fd < 0)
	{
		printf("open %s error", DEV_IR);
		return;
	}
	
	int oldLevel, oldState = 0;
	while(1)
	{
		read(dev_ir_fd, &ev, sizeof(ev));
		switch(ev.type)
		{

#if DEBUG
			printf("----%.2x\n",ev.code);
#endif

			case EV_KEY:
				printf("%d %c %d %c %c %d\n",ev.code,options,ev.value,snakeState,snakeMove,snakeLevel);
				for(i=0 ; i < sizeof(table) / sizeof(table[0]) ; i++)
				{
					if(table[i].key == ev.code && ev.value == 1)
					{
						
						//移动时禁止其它按键
						//if(snakeState && (table[i].key <= KEY_9 && table[i].key >= KEY_1 || table[i].key_desc == 'O'))	break;
						if(snakeState == 'r' && table[i].key_desc != 'O' && (table[i].key_desc > '9' || table[i].key_desc < '1')) {
							if(table[i].key_desc != key_old)//无视蛇同方向的移动操作
							{
								//禁止蛇回头
								if(table[i].key_desc == 'U' && key_old == 'D')	break;
								if(table[i].key_desc == 'D' && key_old == 'U')	break;
								if(table[i].key_desc == 'L' && key_old == 'R')	break;
								if(table[i].key_desc == 'R' && key_old == 'L')	break;

								key_old = table[i].key_desc;
								snakeMove = table[i].key_desc;
							}
							else	oldState = 1;
						}	
						else 	options = table[i].key_desc;
				
						break;
					}
					else if(table[i].key == ev.code && table[i].key_desc == key_old && ev.value == 2 && (oldState == 1 || oldState == 0)) {
						options = 'm';
						oldLevel = snakeLevel;
						snakeLevel = 9;
						oldState = 2;
					}
					else if(table[i].key == ev.code && table[i].key_desc == key_old && ev.value == 0 && oldState == 2) {
						options = 'r';
						oldState = 0;
						snakeLevel = oldLevel;
					}
				}
			default:
				break;
		}
	}
}

