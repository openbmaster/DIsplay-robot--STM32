#ifndef __LED_H
#define __LED_H

/*LED����ض���*/
#define RCC_GPIO_LED                    RCC_APB2Periph_GPIOB    /*LEDʹ�õ�GPIOʱ��*/
#define LEDn                            8                       /*LED����*/
#define GPIO_LED                        GPIOB                   /*LED��ʹ�õ�GPIO��*/
#define DS1_PIN                         GPIO_Pin_5              /*DS1ʹ�õ�GPIO�ܽ�*/
#define DS2_PIN                         GPIO_Pin_0				/*DS2ʹ�õ�GPIO�ܽ�*/
#define DS3_PIN                         GPIO_Pin_1             
		


#define LED1_VALUE GPIO_ReadInputDataBit(GPIO_LED, DS1_PIN)
#define LED2_VALUE GPIO_ReadInputDataBit(GPIO_LED, DS2_PIN)
#define LED3_VALUE GPIO_ReadInputDataBit(GPIO_LED, DS3_PIN)

	

#endif
