#include "drv_uart.h"
#include "drv_AS62.h"
#include "led.h"
#include "delay.h"
#include "stm32f10x.h"
#include "AM2306.h"
#include "usart.h"
#include "delay.h"
#include "sys.h"
#include "exti.h"
#include "key.h"
#include "drv_uart2.h"
#include "dht11.h" 	
#include "timer.h"
#include "BLE.h"

#define uchar unsigned char 
#define uint  unsigned int

//AM2301函数声明
extern void AM2301_Port_Config(void);
extern unsigned short Tem;
extern unsigned short RH;
extern void Get_AM2301_Data(void);
//变量定义

u8 ble_BUF[255];
int send_start=0;

uchar BUF[8];
u8 temperature;  	    
u8 humidity;   
int dis_data;
int mcy;
int fla=0,fla2,fla4=0;	
uint8_t flag[]={0};
uint8_t flag2[]={0}; //这个数组清空LORA里面的缓存
uchar BUF[8];
u8 blen,bcnt;
u8 w=0,s=1;
float wendu=0.1,shidu=0.2;

	int main( void )
{
  unsigned char Tem_Sign = 0;
	int t;
	
	//初始化
	drv_uart_init( 9600 );
	uart_init( 9600 );
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
  SystemInit();
	delay_init();	    		  
	LED_Init();
  AM2301_Port_Config();
	
  TIM3_Int_Init(3999,14399); 
	while(1)
	{  
    if(send_start==1)
		{
			LED0=!LED0;
			blen=1; //第二个字节为指令个数，第三个字节不用，第4个字节往后为指令
			bcnt=ble_BUF[blen];	
			blen=blen+2;
			while(bcnt!=0)
			{
				switch(ble_BUF[blen]&0x07)
				{
					case 0: 
						SendFloat(w,wendu);
					  SendFloat(s,shidu);
					  break;
					case 1:
						break;
					case 2:
						break;
				}
				bcnt=bcnt-1;
				blen=blen+1;
			}
		send_start=0;
		Get_AM2301_Data();
		wendu=(float)Tem/10;
		shidu=(float)RH/10;
		}
		
		
    ASxx_rx_packet(flag); //等待接收数据
		if(flag[0]=='1')
		{
		 fla=1;
		 TIM_Cmd(TIM3,ENABLE);  //使能TIMx		
		 flag[0]=0;
		 LED0=!LED0;
		}
		while(fla==1)
		{   
		     BUF[0]=Tem;
		     BUF[1]=RH;
			   ASxx_tx_packet(0x12, 0x35, 0x17,BUF,2);
				 delay_ms(100);
		     LED1=!LED1;
		 }
		if(fla4==1)  //清除LORA中缓存的数据
		{
			ASxx_rx_packet(flag2);
			fla4=0;
			Get_AM2301_Data();
		}
	
	}
}