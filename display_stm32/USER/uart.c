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
  
OS_EVENT *Recv_Sem;		  //���������ź���
OS_EVENT *Send_Sem;		  //���������ź���

INT16U	Recv_Size;

INT8U Uart_Recv_Buffer[BUFFER_SIZE];	 //DMA���ջ���
INT8U Uart_Deal_Buffer[BUFFER_SIZE];	 //DMA������
INT8U Uart_Send_Buffer[BUFFER_SIZE];	 //DMA���ͻ���

INT8U Deal_State; 	   //���ݴ���״̬	0������� 1���ڴ���
INT8U Flag_Uart_Send;   //���ݷ���״̬	0������� 1���ڷ���

void uart1_init(u32 bound)
{
    //GPIO�˿�����
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
 
	DMA_DeInit(DMA1_Channel4);												//DMA1ͨ��4����      
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);			//�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Send_Buffer; 		//�ڴ��ַ      
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//dma���䷽����  
	DMA_InitStructure.DMA_BufferSize = 100;    								//����DMA�ڴ���ʱ�������ĳ���  	  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    	//����DMA���������ģʽ��һ������  	    
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;     			//����DMA���ڴ����ģʽ  					  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //���������ֳ�  			   
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;     //�ڴ������ֳ�  				  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  	       					//����DMA�Ĵ���ģʽ  							  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;      				//����DMA�����ȼ���  							  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  		      				//����DMA��2��memory�еı����������  		 
	DMA_Init(DMA1_Channel4,&DMA_InitStructure);  
	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);							//DMA��������ж�
//	DMA_Cmd(DMA1_Channel4, ENABLE);											//ʹ��ͨ��4

	DMA_DeInit(DMA1_Channel5);  											//DMA1ͨ��5����
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  		//�����ַ  			   
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Recv_Buffer;  	//�ڴ��ַ  				
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  			       	//dma���䷽����  			
    DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;  						//����DMA�ڴ���ʱ�������ĳ��� 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	    //����DMA���������ģʽ  	
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  				//����DMA���ڴ����ģʽ  		
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //���������ֳ�  				     
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  	    //�ڴ������ֳ�  				  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  					    	//����DMA�Ĵ���ģʽ  				  
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  	     		//����DMA�����ȼ���  					 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  				      		//����DMA��2��memory�еı����������  
    DMA_Init(DMA1_Channel5,&DMA_InitStructure);    
    DMA_Cmd(DMA1_Channel5,ENABLE);  										//ʹ��ͨ��5
  
   //USART ��ʼ������ 
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
 
//	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);  	//�����ж�
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE); 	//���տ����ж�

	//����UART1�ж�    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;               //ͨ������Ϊ����1�ж�    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;       //�ж�ռ�ȵȼ�0    
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;              //�ж���Ӧ���ȼ�0    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //���ж�    
    NVIC_Init(&NVIC_InitStructure);

	//DMA�����ж�����  
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure); 

	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  //����DMA��ʽ����
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);  //����DMA��ʽ���� 
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
}

void Task_Uart1_Recv(void *arg)
{
	int Target_x = 0;
    int set_Compare = 0;

	INT8U err;
	(void)* arg;

	Recv_Sem = OSSemCreate( 0 );
	memset(Uart_Recv_Buffer,0,BUFFER_SIZE);	 //��ջ�����
	memset(Uart_Deal_Buffer,0,BUFFER_SIZE);
	while(1)
	{
		Deal_State = 0;
		OSSemPend(Recv_Sem,0,&err);//�ȴ����ݵ���
		if(Deal_State == 1)
		{
			/*��ʼ�������ݣ��������ʱ��ϳ��������ٿ�һ���߳�
			 *���������ݷ���   Uart_Deal_Buffer	  ������ 
			 *�����յ�0x50 0x49 0x01 0x02 0x03 0x00  6λ����
			 * Uart_Deal_Buffer	����ǰ6λ��ᱣ��0x50 0x49 0x01 0x02 0x03 0x00
			 * ͨ���Աȵ�һλ����0x50�����һλ����0x00ȷ�����ݽ�����ȷ
			 * ͨ���Աȵڶ�λ����0x49�ж�Ҫ����ʲô����
			 * ͨ����3~5λ����ȷ��  ���ؿ���/�رա�ͷ��ת��λ��
			 */
			/**************�����Ǵ������***************/
			//�ⲿ������Ҫ�޸�����
			if((Uart_Deal_Buffer[0]==0x50)&&(Uart_Deal_Buffer[5]==0x00))//��ʼλֹͣλ��ȷ
			{
				switch(Uart_Deal_Buffer[1])
				{
					case 0x49:	//����λ0x49Ϊ����ͷ������
						//GPIO_SetBits(GPIOC,GPIO_Pin_7);
					  //delaysimple(6000000);
						Target_x = Uart_Deal_Buffer[2]*256+Uart_Deal_Buffer[3];
            			set_Compare = Target_x + 1180;
            			if((set_Compare > 1000)&&(set_Compare < 2000))
                		TIM_SetCompare1(TIM3,set_Compare);
					case 0x48:	//����λ0x48Ϊ����ͷ������				
						//GPIO_SetBits(GPIOC,GPIO_Pin_7);
					  //delaysimple(6000000);
					Target_x = Uart_Deal_Buffer[2]*256+Uart_Deal_Buffer[3];
            			set_Compare = Target_x + 1180;
            			if((set_Compare > 1000)&&(set_Compare < 2000))
                		TIM_SetCompare1(TIM3,set_Compare);
					default :	//���ݴ���ʧ�ܣ�LED4�ı�״̬
						if(LED1_VALUE == 0)
							GPIO_SetBits(GPIO_LED,DS1_PIN);
						else
							GPIO_ResetBits(GPIO_LED,DS1_PIN);		  
				}
			}
			/***************����������****************/
 
			//LED3�ı�״̬
			if(LED3_VALUE == 0)
				GPIO_SetBits(GPIO_LED,DS3_PIN);
			else
				GPIO_ResetBits(GPIO_LED,DS3_PIN);
//			OSSemPost(Send_Sem);//�������ݷ�������
			Deal_State = 0;	//�������ݽ���
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
	memset(Uart_Send_Buffer,0,BUFFER_SIZE);	 //��ջ�����
	while(1)
	{
		OSSemPend(Send_Sem,0,&err);//�ȴ������ź���
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

void USART1_IRQHandler(void)              	//����1�жϷ������
{
	OSIntEnter();      
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
    {  
        DMA_Cmd(DMA1_Channel5,DISABLE); 
        USART1->SR;  
        USART1->DR; //��USART_IT_IDLE��־  

		Recv_Size = DMA_GetCurrDataCounter(DMA1_Channel5);
		Recv_Size = BUFFER_SIZE-Recv_Size;  
   		if(Recv_Size > 0 && Deal_State == 0)
		{
			memcpy(Uart_Deal_Buffer,Uart_Recv_Buffer,Recv_Size);	   //��������
			Deal_State = 1;
			OSSemPost(Recv_Sem);//�������ݴ������� 
		}
		DMA_SetCurrDataCounter(DMA1_Channel5,BUFFER_SIZE);
		DMA_Cmd(DMA1_Channel5,ENABLE);
    }
	OSIntExit(); 
}

void DMA1_Channel4_IRQHandler(void)  
{   
	OSIntEnter();          
	DMA_ClearFlag(DMA1_FLAG_TC4); 	//�����־λ  	  
	DMA_Cmd(DMA1_Channel4,DISABLE); //�ر�DMA  
	Flag_Uart_Send = 0;  
	OSIntExit();                  
}

