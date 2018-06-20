#include "stm32f10x.h"
#include "key.h" 

/*
 * ��������Key_Init
 * ����  �����ð�����I/O
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void Key_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;  

    GPIO_InitStructure.GPIO_Pin = K1_PIN; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIO_KEY1, &GPIO_InitStructure);//�����趨������ʼ��Key1

	  GPIO_InitStructure.GPIO_Pin = K2_PIN; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIO_KEY2, &GPIO_InitStructure);//�����趨������ʼ��Key1
}

/*
 * ��������Key_Exti_Init
 * ����  �����ð����ж�
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void Key_Exti_Init(void)
{ 
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// �����˿ڳ�ʼ��
	Key_Init();  
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//ʹ�ܸ��ù���ʱ��
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line0;//�ⲿ�ж���
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�0 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);   //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource13);
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line13;//�ⲿ�ж���
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�0 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);   //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

void EXTI15_10_IRQHandler(void)
{																											  
	 //���봦���� 
    OSIntEnter();
	EXTI_ClearITPendingBit(EXTI_Line13);  //���LINE13�ϵ��жϱ�־λ 
    OSSemPost(g_tKeyExtiSem);  //�����ź�����������
    OSIntExit();
    //�ڳ��жϵ�ʱ�������ϵͳ���ȣ�Ȼ��������ȼ����������ִ�У���֤ϵͳ��ʵʱ�� 
}

void EXTI0_IRQHandler(void)
{																											  
	 //���봦���� 
    OSIntEnter();
	EXTI_ClearITPendingBit(EXTI_Line0);  //���LINE2�ϵ��жϱ�־λ 
    OSSemPost(g_tKeyExtiSem);  //�����ź�����������
    OSIntExit();
    //�ڳ��жϵ�ʱ�������ϵͳ���ȣ�Ȼ��������ȼ����������ִ�У���֤ϵͳ��ʵʱ�� 
}
