#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
//#include<pthread.h>
#include<signal.h>
#include<fcntl.h>
#include<termios.h>
#include<time.h>
#include<sys/types.h>
#include<linux/input.h>

#define MAPLENGTH 16
#define MAPWIDTH 16
#define INITLENGTH 3

#define DEBUG 0

#define DEV_LED1616 "/dev/led1616"
#define DEV_ZLG7290 "/dev/zlg7290"
#define DEV_IR "/dev/input/event1"
#define DEV_BEEP "/sys/devices/platform/pwm/pwm.2"

#define LED_DISPLAY _IOW('L', 0, struct led1616Data)
#define SET_VAL _IO('Z', 0)

char snakeMove;//蛇移动方向全局变量
int snakeLevel;//蛇难度等级全局变量
int snakeLength;//蛇身长度全局变量，兼有计分功能

/*
 * main.c
 */
pthread_t id_infrared_control_thread;
pthread_t id_score_thread;
pthread_t id_snake_thread;

int dev_ir_fd;
int dev_led1616_fd;
int dev_zlg7290_fd;
int dev_beep_fd;

void ReleaseResource();


/*
 * infrared_control.c
 */
void infrared_control_thread();



/*
 * snake.c
 */
typedef struct SNAKE{  //蛇身节点结构体
	int x;
	int y;
	struct SNAKE *next;
}snake;
struct led1616Data{	//点阵数据包结构体
	unsigned char code[33];
};
snake *snakeHead,*snakeFood;//蛇链表头结点，食物指针
int snakeSpeed;//刷新速度
char snakeMove;//蛇移动的方向
char options;//操作方法
char snakeState;/*蛇状态
r:移动
s:停止
m:加速
*/
void snake_thread();
void snake_init();
void snake_food();
int snake_bite_self();
void snake_cross();
void snake_print();
void level_print();
void snake_move_sound();
void snake_eat_sound();

/*
 * score.c
 */
void score_thread();
void itoa(int num, char *str, int radix);

/*
 * startUI.c
 */
void game_end();
void start_interface1();
void start_interface2();
void start_interface3();
void end_interface();
