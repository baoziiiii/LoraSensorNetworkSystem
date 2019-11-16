#ifndef __AM2306_H__
#define __AM2306_H__

#include"stm32f10x.h"

#define RCC_AM2301				RCC_APB2Periph_GPIOG
#define GPIO_AM2301_PORT		GPIOG  
#define GPIO_AM2301_PIN			GPIO_Pin_11

#define AM2301_Write_1()     GPIO_SetBits(GPIO_AM2301_PORT ,GPIO_AM2301_PIN)  //–¥1
#define AM2301_Write_0()     GPIO_ResetBits(GPIO_AM2301_PORT ,GPIO_AM2301_PIN)//–¥0
#define AM2301_ReadBit()     GPIO_ReadInputDataBit(GPIO_AM2301_PORT ,GPIO_AM2301_PIN) //∂¡DQ…œ


#endif
