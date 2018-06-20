#include"stm32f10x.h" 
#include <stdlib.h>
#include "string.h"
#include "led.h"
#include "uart.h"
#include "open.h"


//void delaysimple(u32 i)	  
//{
//	while(i--);
//}
  
OS_EVENT *Recv_Sem;		  //接收数据信号量
OS_EVENT *Send_Sem;		  //发送数据信号量

INT16U	Recv_Size;

INT8U Uart_Recv_Buffer[BUFFER_SIZE];	 //DMA接收缓存
INT8U Uart_Deal_Buffer[BUFFER_SIZE];	 //DMA处理缓存
INT8U Uart_Send_Buffer[BUFFER_SIZE];	 //DMA发送缓存

INT8U Deal_State; 	   //数据处理状态	0处理完成 1正在处理
INT8U Flag_Uart_Send;   //数据发送状态	0发送完成 1正在发送

void uart1_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
 
	DMA_DeInit(DMA1_Channel4);												//DMA1通道4配置      
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);			//外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Send_Buffer; 		//内存地址      
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//dma传输方向单向  
	DMA_InitStructure.DMA_BufferSize = 100;    								//设置DMA在传输时缓冲区的长度  	  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    	//设置DMA的外设递增模式，一个外设  	    
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;     			//设置DMA的内存递增模式  					  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据字长  			   
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;     //内存数据字长  				  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  	       					//设置DMA的传输模式  							  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;      				//设置DMA的优先级别  							  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  		      				//设置DMA的2个memory中的变量互相访问  		 
	DMA_Init(DMA1_Channel4,&DMA_InitStructure);  
	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);							//DMA发送完成中断
//	DMA_Cmd(DMA1_Channel4, ENABLE);											//使能通道4

	DMA_DeInit(DMA1_Channel5);  											//DMA1通道5配置
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  		//外设地址  			   
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Recv_Buffer;  	//内存地址  				
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  			       	//dma传输方向单向  			
    DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;  						//设置DMA在传输时缓冲区的长度 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	    //设置DMA的外设递增模式  	
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  				//设置DMA的内存递增模式  		
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据字长  				     
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  	    //内存数据字长  				  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  					    	//设置DMA的传输模式  				  
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  	     		//设置DMA的优先级别  					 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  				      		//设置DMA的2个memory中的变量互相访问  
    DMA_Init(DMA1_Channel5,&DMA_InitStructure);    
    DMA_Cmd(DMA1_Channel5,ENABLE);  										//使能通道5
  
   //USART 初始化设置 
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
 
//	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);  	//接收中断
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE); 	//接收空闲中断

	//配置UART1中断    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;               //通道设置为串口1中断    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;       //中断占先等级0    
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;              //中断响应优先级0    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //打开中断    
    NVIC_Init(&NVIC_InitStructure);

	//DMA发送中断设置  
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure); 

	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  //采用DMA方式发送
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);  //采用DMA方式接收 
    USART_Cmd(USART1, ENABLE);                    //使能串口 
}

void Task_Uart1_Recv(void *arg)
{
	int Target_x = 0;
    int set_Compare = 0;

	INT8U err;
	(void)* arg;

	Recv_Sem = OSSemCreate( 0 );
	memset(Uart_Recv_Buffer,0,BUFFER_SIZE);	 //清空缓冲区
	memset(Uart_Deal_Buffer,0,BUFFER_SIZE);
	while(1)
	{
		Deal_State = 0;
		OSSemPend(Recv_Sem,0,&err);//等待数据到来
		if(Deal_State == 1)
		{
			/*开始处理数据，如果处理时间较长，可以再开一个线程
			 *待处理数据放在   Uart_Deal_Buffer	  数组中 
			 *比如收到0x50 0x49 0x01 0x02 0x03 0x00  6位数据
			 * Uart_Deal_Buffer	数组前6位则会保存0x50 0x49 0x01 0x02 0x03 0x00
			 * 通过对比第一位比如0x50和最后一位比如0x00确认数据接收正确
			 * 通过对比第二位比如0x49判断要处理什么任务
			 * 通过第3~5位数据确定  开关开启/关闭、头部转动位置
			 */
			/**************以下是处理程序***************/
			//这部分是主要修改内容
			if((Uart_Deal_Buffer[0]==0x50)&&(Uart_Deal_Buffer[5]==0x00))//起始位停止位正确
			{
				switch(Uart_Deal_Buffer[1])
				{
					case 0x49:	//功能位0x49为控制头部命令
						//GPIO_SetBits(GPIOC,GPIO_Pin_7);
					  //delaysimple(6000000);
						Target_x = Uart_Deal_Buffer[2]*256+Uart_Deal_Buffer[3];
            			set_Compare = Target_x + 1180;
            			if((set_Compare > 1000)&&(set_Compare < 2000))
                		TIM_SetCompare1(TIM3,set_Compare);
					case 0x48:	//功能位0x48为控制头部命令				
						//GPIO_SetBits(GPIOC,GPIO_Pin_7);
					  //delaysimple(6000000);
					Target_x = Uart_Deal_Buffer[2]*256+Uart_Deal_Buffer[3];
            			set_Compare = Target_x + 1180;
            			if((set_Compare > 1000)&&(set_Compare < 2000))
                		TIM_SetCompare1(TIM3,set_Compare);
					default :	//数据处理失败，LED4改变状态
						if(LED1_VALUE == 0)
							GPIO_SetBits(GPIO_LED,DS1_PIN);
						else
							GPIO_ResetBits(GPIO_LED,DS1_PIN);		  
				}
			}
			/***************处理程序结束****************/
 
			//LED3改变状态
			if(LED3_VALUE == 0)
				GPIO_SetBits(GPIO_LED,DS3_PIN);
			else
				GPIO_ResetBits(GPIO_LED,DS3_PIN);
//			OSSemPost(Send_Sem);//唤醒数据发送任务
			Deal_State = 0;	//处理数据结束
		}	
//		memset(Uart_Recv_Buffer,0,Recv_Size);
//		memset(Uart_Deal_Buffer,0,Recv_Size);
	}
}

void Task_Uart1_Send(void *arg)
{
	INT8U err;
	(void)* arg;
	Send_Sem = OSSemCreate( 0 );
	memset(Uart_Send_Buffer,0,BUFFER_SIZE);	 //清空缓冲区
	while(1)
	{
		OSSemPend(Send_Sem,0,&err);//等待发送信号量
		if(Flag_Uart_Send == 0)
		{
			DMA_SetCurrDataCounter(DMA1_Channel4,Recv_Size);
			memcpy(Uart_Send_Buffer,Uart_Deal_Buffer,Recv_Size);
			Flag_Uart_Send = 1;
			DMA_Cmd(DMA1_Channel4,ENABLE);
		}
//		memset(Uart_Send_Buffer,0,Recv_Size);
	}
}

void USART1_IRQHandler(void)              	//串口1中断服务程序
{
	OSIntEnter();      
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
    {  
        DMA_Cmd(DMA1_Channel5,DISABLE); 
        USART1->SR;  
        USART1->DR; //清USART_IT_IDLE标志  

		Recv_Size = DMA_GetCurrDataCounter(DMA1_Channel5);
		Recv_Size = BUFFER_SIZE-Recv_Size;  
   		if(Recv_Size > 0 && Deal_State == 0)
		{
			memcpy(Uart_Deal_Buffer,Uart_Recv_Buffer,Recv_Size);	   //拷贝数据
			Deal_State = 1;
			OSSemPost(Recv_Sem);//唤醒数据处理任务 
		}
		DMA_SetCurrDataCounter(DMA1_Channel5,BUFFER_SIZE);
		DMA_Cmd(DMA1_Channel5,ENABLE);
    }
	OSIntExit(); 
}

void DMA1_Channel4_IRQHandler(void)  
{   
	OSIntEnter();          
	DMA_ClearFlag(DMA1_FLAG_TC4); 	//清除标志位  	  
	DMA_Cmd(DMA1_Channel4,DISABLE); //关闭DMA  
	Flag_Uart_Send = 0;  
	OSIntExit();                  
}

