#ifndef __DRV_UART2_H__
#define __DRV_UART2_H__


#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"


//??????
#define UART2_PORT					USART3
#define UART2_PORT_CLK				RCC_APB1Periph_USART3
#define UART2_PORT_AF				GPIO_Remap_USART3

//??????
#define UART2_TX_GPIO_PORT			GPIOB
#define UART2_TX_GPIO_CLK			RCC_APB2Periph_GPIOA
#define UART2_TX_GPIO_PIN			GPIO_Pin_10

#define UART2_RX_GPIO_PORT			GPIOA
#define UART2_RX_GPIO_CLK			RCC_APB2Periph_GPIOA
#define UART2_RX_GPIO_PIN			GPIO_Pin_11


void drv_uart2_init( uint32_t UartBaudRate );
void drv_uart2_tx_bytes( uint8_t* TxBuffer, uint8_t Length );
uint8_t drv_uart2_rx_bytes( uint8_t* RxBuffer );



#endif
