#include"global_variable.h"

/* 声明线程 */
extern pthread_t id_infrared_control_thread;
extern pthread_t id_snake_thread;
extern pthread_t id_score_thread;

/*
 *功能：主函数
 */
int main()
{
	start_interface1();

	/*创建红外遥控命令接收线程*/
	if(pthread_create(&id_infrared_control_thread, 0, infrared_control_thread, NULL) !=0 )
	{
		printf("infrared_control_thread creation failed");
		return -1;
	}
	/*创建计分线程*/
	if(pthread_create(&id_score_thread, 0, score_thread, NULL) !=0 )
	{
		printf("score_thread creation failed");
		return -1;
	}
	/*创建贪吃蛇游戏线程*/
	if(pthread_create(&id_snake_thread, 0, snake_thread, NULL) !=0 )
	{
		printf("snake_thread creation failed");
		return -1;
	}

	/* 等待各线程退出 */
	pthread_join (id_infrared_control_thread,NULL);
	pthread_join (id_snake_thread,NULL);
	pthread_join (id_score_thread,NULL);


	/*信号处理函数。SIGINT：要进行处理的信号，此处信号SIGINT由Interrupt Key产生，通常是CTRL+C或者DELETE*/
	signal (SIGINT, ReleaseResource);
	
	return 0;
}

/*
 * 功能  ：释放线程资源，取消线程
 * 参数  ：无
 * 返回值：无
 */
void ReleaseResource()
{
	/* 取消线程函数 */
	pthread_cancel (id_infrared_control_thread);
	pthread_cancel (id_snake_thread);
	pthread_cancel (id_score_thread);

	/* 关闭设备句柄 */
	close (dev_led1616_fd);
	close (dev_zlg7290_fd);
	close (dev_ir_fd);

	printf ("All quit!\n");
}
