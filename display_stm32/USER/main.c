#include "stm32f10x.h"
#include "ucos_ii.h"
#include "app_cfg.h"
#include "tim_pwm.h"
#include "led.h"
#include "key.h"
#include "uart.h"
#include "open.h"

const uint32_t SystemFrequency         = 72000000;      /*!< System Clock Frequency (Core Clock) */

OS_EVENT *g_tKeyExtiSem;//�����ж��ź���
INT8U g_ucErr;//��������ȫ�ֱ���

int motor_flag = 0;
int leddly_ms = 25;

/************����ջ��С����λΪ OS_STK ��************/
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
	   
OS_STK start_task_stk[START_TASK_STK_SIZE];		//����ջ
OS_STK task_led_stk[TASK_LED_STK_SIZE];		  	//����ջ
OS_STK task_key_stk[TASK_KEY_STK_SIZE];		  	//����ջ
OS_STK task_uart1_rx_stk[TASK_UART1_RX_STK_SIZE];		  	//����ջ
OS_STK task_uart1_tx_stk[TASK_UART1_TX_STK_SIZE];		  	//����ջ

/************��������*************/
void Task_LED(void *arg);  //������˸
void Task_KEY(void *arg);  //��������PWM�����ڲ��Զ��

int main(void)
 {
	 
	 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	SystemInit();
	SysTick_Config(SystemFrequency/OS_TICKS_PER_SEC);
	
	Key_Exti_Init();
	TIM3_PWM_Init();
	RELAY1_Init();		//�̵��� 1��ʼ��
	
	RCC_APB2PeriphClockCmd(RCC_GPIO_LED, ENABLE);           /*ʹ��LED��ʹ�õ�GPIOʱ��*/
  	GPIO_InitStructure.GPIO_Pin = DS1_PIN|DS2_PIN|DS3_PIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIO_LED, &GPIO_InitStructure);               /*LED����ص�GPIO�ڳ�ʼ��*/
  	GPIO_SetBits(GPIO_LED,DS1_PIN|DS2_PIN); 				/* Ϩ�����е�LEDָʾ��*/

	uart1_init(9600);
	
	 
	 /*****�̵���1��ʼ��****/
	 
	 
	 
	 /*****�̵���1��ʼ�����****/
	 
	 /*****�̵���2��ʼ��****/
	 
	 
	 
	 /*****�̵���2��ʼ�����****/
	 

	/******** UCOSII ����ϵͳ��ʼ����ʼ �� ***********************************************************/
	OSInit();	/* UCOSII ����ϵͳ��ʼ�� */

	/* UCOSII�������� */
	OSTaskCreate(Task_LED,(void *)0,&task_led_stk[TASK_LED_STK_SIZE-1], TASK_LED_PRIO);
	OSTaskCreate(Task_KEY,(void *)0,&task_key_stk[TASK_KEY_STK_SIZE-1], TASK_KEY_PRIO);	
	OSTaskCreate(Task_Uart1_Recv,(void *)0,&task_uart1_rx_stk[TASK_UART1_RX_STK_SIZE-1], TASK_UART1_RX_PRIO);		
	OSTaskCreate(Task_Uart1_Send,(void *)0,&task_uart1_tx_stk[TASK_UART1_TX_STK_SIZE-1], TASK_UART1_TX_PRIO);		

	OSStart();	 	/* ����UCOSII����ϵͳ */
	/******** UCOSII ����ϵͳ��ʼ������ �� ***********************************************************/  
}

void Task_LED(void *arg)	  //״̬LED��LED��˸���������������
{
    (void)arg;                		// 'arg' ��û���õ�����ֹ��������ʾ����
    while (1)
    {
		GPIO_ResetBits(GPIO_LED,DS2_PIN); 	//����LED1
		OSTimeDlyHMSM(0,0,0,leddly_ms);			//��ʱ
		GPIO_SetBits(GPIO_LED,DS2_PIN);	  	//Ϩ��LED1
		OSTimeDlyHMSM(0,0,0,leddly_ms);			//��ʱ
    }
}

void Task_KEY(void *arg)
{
	(void)arg;
    g_tKeyExtiSem = OSSemCreate( 0 );  //���崦��Ϊ0���ź���
    
	while(1)
    {
			//GPIO_SetBits(GPIOC,GPIO_Pin_6);
        OSSemPend(g_tKeyExtiSem,0,&g_ucErr);//�ȴ����������ж��ź�
        OSTimeDlyHMSM(0,0,0,10);
		//K1_PIN	GPIO_Pin_2	   ����LED2����
		//K2_PIN	GPIO_Pin_3	   ����4��PWMƵ�ʺ�LED1��˸Ƶ��
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
				case 0:TIM_SetCompare1(TIM3, 1000); leddly_ms = 25; break;	 //90��
				case 1:TIM_SetCompare1(TIM3, 1500); leddly_ms = 50; break;	 //180��
				case 2:TIM_SetCompare1(TIM3, 2000); leddly_ms = 100; break;	 //270��
				case 3:TIM_SetCompare1(TIM3, 1500); leddly_ms = 50; break;	 //180��
				default:break;
			}
		}
		
    }
}
