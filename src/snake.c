#include"global_variable.h"

extern snake *snakeHead;
extern char snakeMove;
extern char snakeState;
extern char options;
extern int snakeLevel;
extern int snakeLength;

/*
 * 功能  ：
 * 参数  ：
 * 返回值：
 */
void snake_thread()
{	
	printf("--我们的小可爱已经饥渴难耐了！\n");
	
	snakeLength = INITLENGTH;
	snakeSpeed = 0;
	snakeLevel = 1;
	snakeState = 's';

	dev_led1616_fd = open(DEV_LED1616,O_RDWR);//|O_NOCTTY|O_NDELAY);
	dev_beep_fd = open(DEV_BEEP,O_RDWR);//|O_NOCTTY|O_NDELAY);

	snake_init();//蛇身初始化
	snake_print();

	while(1)//等待输入难度
	{
		if(options >= '1' && options <= '9')
		{
			snakeLevel = options - '0';
			snakeSpeed = (10 - snakeLevel) * 100000;
			break;
		}
	}

	start_interface2();
	while(options != 'O');//等待确认键被点击
	options = 't';
	snakeState = 'r';
	start_interface3();
	
	snakeMove = 'L';//初始化蛇向左运动

	if(dev_led1616_fd < 0 || dev_beep_fd < 0)
	{
		printf("open %s error\n",DEV_LED1616);
		return;
	}

	while(1)
	{
		snake *snakeNextHead;
		snakeNextHead = (snake*)malloc(sizeof(snake));

		if(snakeState != 's')
		{
			switch(snakeMove)
			{
				case 'U':
					snakeNextHead->x = snakeHead->x;
					snakeNextHead->y = snakeHead->y - 1;
					break;
				case 'D':
					snakeNextHead->x = snakeHead->x;
					snakeNextHead->y = snakeHead->y + 1;
					break;
				case 'L':
					snakeNextHead->x = snakeHead->x + 1;
					snakeNextHead->y = snakeHead->y;
					break;
				case 'R':
					snakeNextHead->x = snakeHead->x - 1;
					snakeNextHead->y = snakeHead->y;
					break;
				default:
					break;
			}

			if (snakeHead->x == snakeFood->x && snakeHead->y == snakeFood->y)//如果蛇头触碰到了食物
			{
				snakeNextHead->next = snakeHead;
				snakeHead = snakeNextHead;
				snakeLength += 1;
				if(snakeLength > 9999)
					snakeLength = 9999;
				if((snakeLength - INITLENGTH) % 10 == 0)//每逢吃十个食物，增加一级难度
					snakeLevel += 1;
				if(snakeLevel > 9)
					snakeLevel = 9;
				snakeSpeed = (10 - snakeLevel) * 100000;//重新调整速度
				snake_food();
			}
			else   //如果没有食物//
			{
				snakeNextHead->next = snakeHead;
				snakeHead = snakeNextHead;
				while (snakeNextHead->next->next != NULL)
				{
					snakeNextHead = snakeNextHead->next;
				}
				free(snakeNextHead->next);
				snakeNextHead->next = NULL;
			}
		}

		switch(options)
		{
			//切换加速和普通模式
			case 'm':
			case 'r':
				snakeState = 'r';
				snakeSpeed = (10 - snakeLevel) * 100000;
				options = 't';
				break;
			//暂停模式
			case 'O':
				snakeState = snakeState == 's' ? 'r' : 's';
				options = 't';
				break;
			default:
				break;
		}
		//蛇静止时改变速度
		if (options <= '9' && optinos > '0') {
			snakeLevel = options - '0';
			snakeSpeed = (10 - snakeLevel) * 100000;
		}


		
		snake_cross();//判断是否穿墙

		snake_bite_self();//判断是否咬到了自己

		snake_print();//打印蛇

		usleep(snakeSpeed);
	}
}

/*
 * 功能  ：初始化蛇身
 * 参数  ：无
 * 返回值：无
 */
void snake_init()
{
	snake *snakeTail;
	int i;
	snakeTail = (snake*)malloc(sizeof(snake));//从蛇尾开始，用头插法设置初始位置

	snakeTail->x = 0;
	snakeTail->y = 0;
	snakeTail->next = NULL;

	for(i = 1 ;i < snakeLength ;i++)
	{
		snakeHead = (snake*)malloc(sizeof(snake));
		snakeHead->next = snakeTail;
		snakeHead->x = snakeTail->x + 1;
		snakeHead->y = snakeTail->y;
		snakeTail = snakeHead;
	}

	snake_food();
}

/*
 * 功能  ：随机食物产生
 * 参数  ：无
 * 返回值：无
 */
void snake_food()
{
	snake *foodTemp;
	snake *snakeTemp;
	srand((unsigned)time(NULL));//为了防止每次产生的随机数相同，种子设置为time
	foodTemp = (snake*)malloc(sizeof(snake));
	foodTemp->x = rand() % MAPWIDTH;
	foodTemp->y = rand() % MAPLENGTH;
	snakeTemp = snakeHead;

	while (snakeTemp->next != NULL)
	{
		if (snakeTemp->x == foodTemp->x && snakeTemp->y == foodTemp->y) //判断蛇身是否与食物重合
		{
			free(foodTemp);
			snake_food();
			//return;
		}
		snakeTemp = snakeTemp->next;
	}
	snakeFood = foodTemp;
}

/*
 * 功能  ：判断是否咬到了自己
 * 参数  ：无
 * 返回值：1咬到自己，0没有咬到自己
 */
int snake_bite_self()
{
	snake *snakeSelf;
	snakeSelf = snakeHead->next;
	while (snakeSelf != NULL)
	{
		if (snakeSelf->x == snakeHead->x && snakeSelf->y == snakeHead->y)
		{
			snake_eat_sound();
			game_end();
		}
		snakeSelf = snakeSelf->next;
	}
	return 0;
}

/*
 * 功能  ：穿墙判断
 * 参数  ：
 * 返回值：
 */
void snake_cross()
{
	if (snakeHead->x < 0)
		snakeHead->x = MAPWIDTH - 1;

	if (snakeHead->y < 0)
		snakeHead->y = MAPLENGTH - 1;

	if (snakeHead->x >= MAPWIDTH)
		snakeHead->x = 0;

	if (snakeHead->y >= MAPLENGTH)
		snakeHead->y = 0;
}

/*
 * 功能  ：向点阵传输信息
 * 参数  ：无
 * 返回值：无
 */
void snake_print()
{
	int n;
	snake *snakePrint;
	snakePrint = snakeHead;
	struct led1616Data snakeData;
	
	memset(snakeData.code,0,sizeof(snakeData.code));//置0防止数据被污染
	
	while (snakePrint != NULL)
	{
		n = snakePrint->x/8 + 2*snakePrint->y;//算出需要点亮的点在点阵数据数组中的位置
		snakeData.code[n] = snakeData.code[n] | (1 << snakePrint->x % 8);//算出需要点亮的点所在的数据位并置1

#if DEBUG	
		int i;
		printf("-(%d,%d)-",snakePrint->x,snakePrint->y);
		for(i=0;i<sizeof(snakeData.code);i++)
			printf(" 0x%02x",snakeData.code[i]);
#endif		
		
		snakePrint = snakePrint->next;
	}

	n = snakeFood->x/8 + 2*snakeFood->y;
	snakeData.code[n] = snakeData.code[n] | (1 << snakeFood->x % 8);

#if DEBUG	
	int i;
	printf("\nfood:(%d,%d)\n",snakeFood->x,snakeFood->y);
	for(i=0;i<sizeof(snakeData.code);i++)
		printf(" 0x%02x",snakeData.code[i]);
	printf("\n");
#endif
	
	ioctl(dev_led1616_fd,LED_DISPLAY, &snakeData);//发送组好的数据
	
	snake_move_sound();
}

/*
 * 功能  ：蛇移动时的声音
 * 参数  ：无
 * 返回值：无
 */
void snake_move_sound()
{
	int i = 0;
	char beep_on[10] = "2000,50";//频率，占空比
	char beep_off[10] = "0,0";
	if(snakeState == 's')	write(dev_beep_fd,beep_off,sizeof(beep_off));
	else	write(dev_beep_fd,beep_on,sizeof(beep_on));
	usleep(50000);
	write(dev_beep_fd,beep_off,sizeof(beep_off));

}

/*
 * 功能  ：蛇吃到自己时的声音
 * 参数  ：无
 * 返回值：无
 */
void snake_eat_sound()
{
	char beep_on[10] = "100,20";//频率，占空比
	char beep_off[10] = "0,0";

	write(dev_beep_fd,beep_on,sizeof(beep_on));
	usleep(1000000);
	write(dev_beep_fd,beep_off,sizeof(beep_off));
}
