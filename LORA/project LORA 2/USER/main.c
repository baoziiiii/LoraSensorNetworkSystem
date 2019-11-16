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

//AM2301��������
extern void AM2301_Port_Config(void);
extern unsigned short Tem;
extern unsigned short RH;
extern void Get_AM2301_Data(void);
//��������

u8 ble_BUF[255];
int send_start=0;

uchar BUF[8];
u8 temperature;  	    
u8 humidity;   
int dis_data;
int mcy;
int fla=0,fla2,fla4=0;	
uint8_t flag[]={0};
uint8_t flag2[]={0}; //����������LORA����Ļ���
uchar BUF[8];
u8 blen,bcnt;
u8 w=0,s=1;
float wendu=0.1,shidu=0.2;

	int main( void )
{
  unsigned char Tem_Sign = 0;
	int t;
	
	//��ʼ��
	drv_uart_init( 9600 );
	uart_init( 9600 );
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
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
			blen=1; //�ڶ����ֽ�Ϊָ��������������ֽڲ��ã���4���ֽ�����Ϊָ��
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
		
		
    ASxx_rx_packet(flag); //�ȴ���������
		if(flag[0]=='1')
		{
		 fla=1;
		 TIM_Cmd(TIM3,ENABLE);  //ʹ��TIMx		
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
		if(fla4==1)  //���LORA�л��������
		{
			ASxx_rx_packet(flag2);
			fla4=0;
			Get_AM2301_Data();
		}
	
	}
}