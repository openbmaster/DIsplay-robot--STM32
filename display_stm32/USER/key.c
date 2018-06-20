#include "stm32f10x.h"
#include "key.h" 

/*
 * 函数名：Key_Init
 * 描述  ：配置按键的I/O
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void Key_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;  

    GPIO_InitStructure.GPIO_Pin = K1_PIN; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//IO口速度为50MHz
    GPIO_Init(GPIO_KEY1, &GPIO_InitStructure);//根据设定参数初始化Key1

	  GPIO_InitStructure.GPIO_Pin = K2_PIN; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//IO口速度为50MHz
    GPIO_Init(GPIO_KEY2, &GPIO_InitStructure);//根据设定参数初始化Key1
}

/*
 * 函数名：Key_Exti_Init
 * 描述  ：配置按键中断
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void Key_Exti_Init(void)
{ 
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 按键端口初始化
	Key_Init();  
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//使能复用功能时钟
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line0;//外部中断线
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级0 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);   //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource13);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line13;//外部中断线
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级0 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);   //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

void EXTI15_10_IRQHandler(void)
{																											  
	 //进入处理函数 
    OSIntEnter();
	EXTI_ClearITPendingBit(EXTI_Line13);  //清除LINE13上的中断标志位 
    OSSemPost(g_tKeyExtiSem);  //发送信号量给任务函数
    OSIntExit();
    //在出中断的时候会引起系统调度，然后最高优先级的任务会先执行，保证系统的实时性 
}

void EXTI0_IRQHandler(void)
{																											  
	 //进入处理函数 
    OSIntEnter();
	EXTI_ClearITPendingBit(EXTI_Line0);  //清除LINE2上的中断标志位 
    OSSemPost(g_tKeyExtiSem);  //发送信号量给任务函数
    OSIntExit();
    //在出中断的时候会引起系统调度，然后最高优先级的任务会先执行，保证系统的实时性 
}
