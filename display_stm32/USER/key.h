#ifndef _KEY_H
#define _KEY_H

#include "ucos_ii.h"

//#define RCC_GPIO_KEY RCC_APB2Periph_GPIOB
#define GPIO_KEY1 GPIOA
#define GPIO_KEY2 GPIOC
#define K1_PIN	GPIO_Pin_0
#define K2_PIN	GPIO_Pin_13
//#define K3_PIN	GPIO_Pin_4
#define KEY1_VALUE GPIO_ReadInputDataBit(GPIO_KEY1, K1_PIN)
#define KEY2_VALUE GPIO_ReadInputDataBit(GPIO_KEY2, K2_PIN)
//#define KEY3_VALUE GPIO_ReadInputDataBit(GPIO_KEY, K3_PIN)
													 
extern OS_EVENT *g_tKeyExtiSem;

static void Key_Init(void);
void Key_Exti_Init(void);

#endif //_KEY_H
