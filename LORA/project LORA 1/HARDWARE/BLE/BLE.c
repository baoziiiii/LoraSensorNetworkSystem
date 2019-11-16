#include "BLE.h"
#include "sys.h"	
#include "delay.h"

extern u8 ble_BUF[255];
extern int send_start;
int cnt=0;

void BLE_Init(u32 bound)
{  
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//PC10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);  

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4,ENABLE);//¸´Î»´®¿Ú2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4,DISABLE);//Í£Ö¹¸´Î»
 

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8Î»Êý¾Ý³¤¶È
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//Ò»¸öÍ£Ö¹Î»
	USART_InitStructure.USART_Parity = USART_Parity_No;///ÆæÅ¼Ð£ÑéÎ»
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎÞÓ²¼þÊý¾ÝÁ÷¿ØÖÆ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//ÊÕ·¢Ä£Ê½
  USART_Init(UART4, &USART_InitStructure); ; //³õÊ¼»¯´®¿Ú
  
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //Ê¹ÄÜ´®¿Ú2ÖÐ¶Ï
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //ÏÈÕ¼ÓÅÏÈ¼¶2¼¶
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //´ÓÓÅÏÈ¼¶2¼¶
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //Ê¹ÄÜÍâ²¿ÖÐ¶ÏÍ¨µÀ
	NVIC_Init(&NVIC_InitStructure); //¸ù¾ÝNVIC_InitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ÷
 
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//¿ªÆôÖÐ¶Ï
  USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); 
  USART_Cmd(UART4,ENABLE);                    //Ê¹ÄÜ´®¿Ú 
}
void UART4_IRQHandler(void)
{
	u8 res;	    
 
 	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) 
	{	 	 
		ble_BUF[cnt++] =USART_ReceiveData(UART4); 	
		
	
	}  
	else if(USART_GetITStatus(UART4,USART_IT_IDLE)!=RESET)
	{
		if(cnt>1&&ble_BUF[0]==cnt)
		{
			send_start=1;
		}
		UART4->SR;
		UART4->DR;
	  cnt=0;
	}
	USART_ClearFlag(UART4, USART_IT_RXNE);
	USART_ClearFlag(UART4, USART_IT_IDLE);
} 

void BLE_Disable()
{
	USART_Cmd(BLE_USARTx, DISABLE);
}

void BLE_Enable()
{
	USART_Cmd(BLE_USARTx, ENABLE);
}

void SendInt(const u8 ID, const u32 a) 
	{
	u8 data[4];
	int n;
	USART_SendData(UART4, 0x00 + ID);
	while (USART_GetFlagStatus(UART4, USART_FLAG_TC) != SET);//TC=1,???????

	data[0] = (a >> 24) & 0xFF;
	data[1] = (a >> 16) & 0xFF;
	data[2] = (a >> 8) & 0xFF;
	data[3] = (a) & 0xFF;
	for (n = 0; n<4; n++) {
		USART_SendData(UART4, data[n]);
		delay_ms(1); 
	}
	while (USART_GetFlagStatus(UART4, USART_FLAG_TC) != SET);//TC=1,???????
}

void SendFloat(const u8 ID,const float a) 
{
	union val {
		uint32_t i;
		float f;
	}v;

	u8 data[4];
	int n;
	v.f = a;

	USART_SendData(UART4, 0x40 + ID);
	while (USART_GetFlagStatus(UART4, USART_FLAG_TC) != SET);//TC=1,???????

	data[0] = (v.i >> 24) & 0xFF;
	data[1] = (v.i >> 16) & 0xFF;
	data[2] = (v.i >> 8) & 0xFF;
	data[3] = (v.i) & 0xFF;
	for (n = 0; n<4; n++) {
		USART_SendData(UART4, data[n]);
		delay_ms(1); 
	}
	while (USART_GetFlagStatus(UART4, USART_FLAG_TC) != SET);//TC=1,???????
}

