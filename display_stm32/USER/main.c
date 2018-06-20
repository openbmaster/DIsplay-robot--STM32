#include "stm32f10x.h"
#include "ucos_ii.h"
#include "app_cfg.h"
#include "tim_pwm.h"
#include "led.h"
#include "key.h"
#include "uart.h"
#include "open.h"

const uint32_t SystemFrequency         = 72000000;      /*!< System Clock Frequency (Core Clock) */

OS_EVENT *g_tKeyExtiSem;//按键中断信号量
INT8U g_ucErr;//错误类型全局变量

int motor_flag = 0;
int leddly_ms = 25;

/************设置栈大小（单位为 OS_STK ）************/
#define START_TASK_STK_SIZE   		100
#define	TASK_LED_STK_SIZE			100
#define	TASK_KEY_STK_SIZE			100
#define	TASK_UART1_RX_STK_SIZE		100
#define	TASK_UART1_TX_STK_SIZE		100
#define START_TASK_PRIO     	4
#define	TASK_LED_PRIO			5
#define	TASK_KEY_PRIO			6
#define	TASK_UART1_RX_PRIO		7
#define	TASK_UART1_TX_PRIO		8
	   
OS_STK start_task_stk[START_TASK_STK_SIZE];		//定义栈
OS_STK task_led_stk[TASK_LED_STK_SIZE];		  	//定义栈
OS_STK task_key_stk[TASK_KEY_STK_SIZE];		  	//定义栈
OS_STK task_uart1_rx_stk[TASK_UART1_RX_STK_SIZE];		  	//定义栈
OS_STK task_uart1_tx_stk[TASK_UART1_TX_STK_SIZE];		  	//定义栈

/************函数声明*************/
void Task_LED(void *arg);  //持续闪烁
void Task_KEY(void *arg);  //按键控制PWM，用于测试舵机

int main(void)
 {
	 
	 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	SystemInit();
	SysTick_Config(SystemFrequency/OS_TICKS_PER_SEC);
	
	Key_Exti_Init();
	TIM3_PWM_Init();
	RELAY1_Init();		//继电器 1初始化
	
	RCC_APB2PeriphClockCmd(RCC_GPIO_LED, ENABLE);           /*使能LED灯使用的GPIO时钟*/
  	GPIO_InitStructure.GPIO_Pin = DS1_PIN|DS2_PIN|DS3_PIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIO_LED, &GPIO_InitStructure);               /*LED灯相关的GPIO口初始化*/
  	GPIO_SetBits(GPIO_LED,DS1_PIN|DS2_PIN); 				/* 熄灭所有的LED指示灯*/

	uart1_init(9600);
	
	 
	 /*****继电器1初始化****/
	 
	 
	 
	 /*****继电器1初始化完毕****/
	 
	 /*****继电器2初始化****/
	 
	 
	 
	 /*****继电器2初始化完毕****/
	 

	/******** UCOSII 操作系统初始化开始 ｛ ***********************************************************/
	OSInit();	/* UCOSII 操作系统初始化 */

	/* UCOSII创建任务 */
	OSTaskCreate(Task_LED,(void *)0,&task_led_stk[TASK_LED_STK_SIZE-1], TASK_LED_PRIO);
	OSTaskCreate(Task_KEY,(void *)0,&task_key_stk[TASK_KEY_STK_SIZE-1], TASK_KEY_PRIO);	
	OSTaskCreate(Task_Uart1_Recv,(void *)0,&task_uart1_rx_stk[TASK_UART1_RX_STK_SIZE-1], TASK_UART1_RX_PRIO);		
	OSTaskCreate(Task_Uart1_Send,(void *)0,&task_uart1_tx_stk[TASK_UART1_TX_STK_SIZE-1], TASK_UART1_TX_PRIO);		

	OSStart();	 	/* 启动UCOSII操作系统 */
	/******** UCOSII 操作系统初始化结束 ｝ ***********************************************************/  
}

void Task_LED(void *arg)	  //状态LED，LED闪烁代表程序运行正常
{
    (void)arg;                		// 'arg' 并没有用到，防止编译器提示警告
    while (1)
    {
		GPIO_ResetBits(GPIO_LED,DS2_PIN); 	//点亮LED1
		OSTimeDlyHMSM(0,0,0,leddly_ms);			//延时
		GPIO_SetBits(GPIO_LED,DS2_PIN);	  	//熄灭LED1
		OSTimeDlyHMSM(0,0,0,leddly_ms);			//延时
    }
}

void Task_KEY(void *arg)
{
	(void)arg;
    g_tKeyExtiSem = OSSemCreate( 0 );  //定义处置为0的信号量
    
	while(1)
    {
			//GPIO_SetBits(GPIOC,GPIO_Pin_6);
        OSSemPend(g_tKeyExtiSem,0,&g_ucErr);//等待按键触发中断信号
        OSTimeDlyHMSM(0,0,0,10);
		//K1_PIN	GPIO_Pin_2	   控制LED2亮灭
		//K2_PIN	GPIO_Pin_3	   控制4档PWM频率和LED1闪烁频率
        if (KEY1_VALUE == 1)
        {
			if(LED3_VALUE == 0)
			{
				GPIO_SetBits(GPIO_LED,DS3_PIN);
			  GPIO_SetBits(GPIOA,GPIO_Pin_5);
				delaysimple(6000000);
			}
				else
				GPIO_ResetBits(GPIO_LED,DS3_PIN);
        }
		else if(KEY2_VALUE == 1)
		{
			switch(++motor_flag%4)
			{
				case 0:TIM_SetCompare1(TIM3, 1000); leddly_ms = 25; break;	 //90度
				case 1:TIM_SetCompare1(TIM3, 1500); leddly_ms = 50; break;	 //180度
				case 2:TIM_SetCompare1(TIM3, 2000); leddly_ms = 100; break;	 //270度
				case 3:TIM_SetCompare1(TIM3, 1500); leddly_ms = 50; break;	 //180度
				default:break;
			}
		}
		
    }
}
