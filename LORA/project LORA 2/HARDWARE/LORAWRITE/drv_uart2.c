#include "drv_uart2.h"


/**
  * @brief :?????
  * @param :
  *			@UartBaudRate:?????
  * @note  :?
  * @retval:?
  */
void drv_uart2_init( uint32_t UartBaudRate )
{
	GPIO_InitTypeDef	UartGpioInitStructer;
	USART_InitTypeDef	UartinitStructer;
	
	//??????,???TX RX????????,??????,?????
	//?????TX RX ?? 
	RCC_APB2PeriphClockCmd( UART2_TX_GPIO_CLK | UART2_RX_GPIO_CLK, ENABLE );	//??TX RX ????
	
	UartGpioInitStructer.GPIO_Mode = GPIO_Mode_AF_PP;
	UartGpioInitStructer.GPIO_Speed = GPIO_Speed_2MHz;
	//TX
	UartGpioInitStructer.GPIO_Pin = UART2_TX_GPIO_PIN;
	GPIO_Init( UART2_TX_GPIO_PORT, &UartGpioInitStructer );		//???TX??  ???????
	//RX
	UartGpioInitStructer.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	UartGpioInitStructer.GPIO_Pin = UART2_RX_GPIO_PIN;
	GPIO_Init( UART2_RX_GPIO_PORT, &UartGpioInitStructer );		//???RX??  ?????
	
	//??USART??
	USART_DeInit( UART2_PORT );		//????
	
	if( USART1 == UART2_PORT )		//??????
	{
		RCC_APB2PeriphClockCmd( UART2_PORT_CLK, ENABLE );			
	}																	//???USART????????APB???														
	else																//STM32F103?????USART1?APB2?,?????????????
	{
		RCC_APB1PeriphClockCmd( UART2_PORT_CLK, ENABLE );
	}
	
	UartinitStructer.USART_BaudRate = UartBaudRate;						//?????
	UartinitStructer.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//??????
	UartinitStructer.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;		//?????	
	UartinitStructer.USART_Parity = USART_Parity_No;					//????
	UartinitStructer.USART_StopBits = USART_StopBits_1;					//?????
	UartinitStructer.USART_WordLength = USART_WordLength_8b;			//8????
	
	USART_Cmd( UART2_PORT, DISABLE );									//????
	USART_Init( UART2_PORT, &UartinitStructer );							//?????
	USART_Cmd( UART2_PORT, ENABLE );										//????	
}

/**
  * @brief :??????
  * @param :
  *			@TxBuffer:???????
  *			@Length:????
  * @note  :?
  * @retval:?
  */
void drv_uart2_tx_bytes( uint8_t* TxBuffer, uint8_t Length )
{
	while( Length-- )
	{
		while( RESET == USART_GetFlagStatus( UART2_PORT, USART_FLAG_TXE ));
		UART2_PORT->DR = *TxBuffer;
		TxBuffer++;
	}
}

/**
  * @brief :??????
  * @param :
  *			@RxBuffer:???????
  * @note  :?
  * @retval:????????
  */
uint8_t drv_uart2_rx_bytes( uint8_t* RxBuffer )
{
	uint8_t l_RxLength = 0;
	uint16_t l_UartRxTimOut = 0x7FFF;
	
	while( l_UartRxTimOut-- )			//????????
	{
		if( RESET != USART_GetFlagStatus( UART2_PORT, USART_FLAG_RXNE ))
		{
			*RxBuffer = (uint8_t)UART2_PORT->DR;
			RxBuffer++;
			l_RxLength++;
			l_UartRxTimOut = 0x7FFF;	//???????,??????
		}
		if( 100 == l_RxLength )
		{
			break;		//????100???
		}
	}
	
	return l_RxLength;					//????,??????
}
