#ifndef __LED_H
#define __LED_H

/*LED灯相关定义*/
#define RCC_GPIO_LED                    RCC_APB2Periph_GPIOB    /*LED使用的GPIO时钟*/
#define LEDn                            8                       /*LED数量*/
#define GPIO_LED                        GPIOB                   /*LED灯使用的GPIO组*/
#define DS1_PIN                         GPIO_Pin_5              /*DS1使用的GPIO管脚*/
#define DS2_PIN                         GPIO_Pin_0				/*DS2使用的GPIO管脚*/
#define DS3_PIN                         GPIO_Pin_1             
		


#define LED1_VALUE GPIO_ReadInputDataBit(GPIO_LED, DS1_PIN)
#define LED2_VALUE GPIO_ReadInputDataBit(GPIO_LED, DS2_PIN)
#define LED3_VALUE GPIO_ReadInputDataBit(GPIO_LED, DS3_PIN)

	

#endif
