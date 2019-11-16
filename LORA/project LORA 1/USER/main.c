#include "drv_uart.h"
#include "drv_AS62.h"
#include "led.h"
#include "delay.h"
#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "sys.h"
#include "exti.h"
#include "key.h"
#include "drv_uart2.h"
#include "timer.h"
#include "BLE.h"


u8 ble_BUF[255],PC_BUF[255];
int send_start=0,pc_start=0;

uint8_t g_As62_rx_buffer[ 100 ] = { 0 };
uint8_t g_RxLength = 0;
uint8_t flag[]={'1'};
uint8_t flag2[]={0};
unsigned char BUF[8];
u8 temperature;  	    
u8 humidity;   
int dis_data;
int mcy;
int fla=1,fla2,fla3,fla4=0;	


float wendu=26,guangzhao=42,shidu=44,fengsu=0,fengxiang=0;
int ff=0;
u8 w=0,s=1,gz=2,fs=3,fx=4;
int z0=0,z1=0,z2=0,z3=0,z4=0;
u8 blen,bcnt;
int ii=0,aa=0,bb=0,uu=0,vv=0;
int main( void )
{
  float temp;
	int t;
	drv_uart_init( 9600 );
	uart_init( 115200 );
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
  SystemInit();
	delay_init();	    		  
	LED_Init();
	BLE_Init(9600);
  TIM3_Int_Init(9999,14399); //5K频率  技术到1W 2S
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
					  SendFloat(gz,guangzhao);
					  if(ff%2==0)
						{ SendFloat(fs,fengsu);
						
						}
							ff++;
							if(ff>100)
							{ff=0;}
					  SendFloat(fx,fengxiang);
				
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
		TIM_Cmd(TIM3, ENABLE);
		}
		
	
		if(pc_start==1&&send_start==0)
		{
			LED1=!LED1;
			switch(PC_BUF[1])
			{
				case 0x00:
				  break;
				case 0x01:
				printf("<SinkNodeLS:0>");
				 break;
				case 0x02:
				printf("<SensorLS:<Address:<0:%d>><Name:TEMPERATURE>>",w);
				delay_ms(1);
				printf("<SensorLS:<Address:<0:%d>><Name:HUMIDITY>>",s);
				delay_ms(1);
				printf("<SensorLS:<Address:<0:%d>><Name:LIGHT>>",gz);
				delay_ms(1);
				printf("<SensorLS:<Address:<0:%d>><Name:fengsu>>",fs);
				delay_ms(1);
				printf("<SensorLS:<Address:<0:%d>><Name:fengxiang>>",fx);
				delay_ms(1);
				break;
				case 0x03:
				printf("<SensorRSD:<Address:<0:%d>><Data:%f>>",w,wendu);
				delay_ms(1);
				printf("<SensorRSD:<Address:<0:%d>><Data:%f>>",s,shidu);
				delay_ms(1);
				printf("<SensorRSD:<Address:<0:%d>><Data:%f>>",gz,guangzhao);
				delay_ms(1);
				printf("<SensorRSD:<Address:<0:%d>><Data:%f>>",fs,fengsu);
				delay_ms(1);
				printf("<SensorRSD:<Address:<0:%d>><Data:%f>>",fx,fengxiang);
				delay_ms(1);
				break;
				
				default : printf("错误指令"); break;
			}
			pc_start=0;
			TIM_Cmd(TIM3, ENABLE);
			delay_ms(10);
		}
	 
 
 

		if(fla==0)
	 { 
		 BLE_Disable();
		for(t=0;t<2;t++)
		{
		 ASxx_tx_packet(0x12, 0x34, 0x17,flag,1);
		 LED1=!LED1;
		 delay_ms(100);
		}
		TIM_Cmd(TIM3, ENABLE);
		fla3=0;
		while(fla3==0)
		{
		g_RxLength = ASxx_rx_packet( BUF );
		 if( 0 != g_RxLength )
		 {
			LED1=0;
			LED0=0;
			wendu=(float)BUF[1]/10;
			shidu=(float)BUF[0]/10;
		 }
		}
		for(t=0;t<2;t++)
		{
		 ASxx_tx_packet(0x12, 0x35, 0x17,flag,1);
		 LED1=!LED1;
		 delay_ms(100);
		}
		TIM_Cmd(TIM3, ENABLE); 
		fla3=0;
		while(fla3==0)
		{
		g_RxLength = ASxx_rx_packet( BUF );
		 if( 0 != g_RxLength )
		 {
			LED1=0;
			LED0=0;
			fengxiang=BUF[1];
			fengsu=BUF[0];
		 }
		}

  for(t=0;t<2;t++)  //向节点3请求数据
		{
		 ASxx_tx_packet(0x12, 0x36, 0x17,flag,1);
		 LED1=!LED1;
		 delay_ms(100);
		}
		TIM_Cmd(TIM3, ENABLE);
		fla3=0;
		while(fla3==0)  //等待节点3数据
		{
		g_RxLength = ASxx_rx_packet( BUF );
		 if( 0 != g_RxLength )
		 {
			LED1=0;
			LED0=0;
			dis_data=BUF[0];
		  dis_data=BUF[1]+(dis_data<<8);
		  guangzhao=(float)dis_data/1.2;
      //printf("%flx\r\n",guangzhao); 
		 }
		}		
		
    fla=1;
		BLE_Enable();
	 }
 }
}
