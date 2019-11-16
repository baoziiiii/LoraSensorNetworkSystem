#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "LED.h"
#include "AM2306.h"


unsigned short Tem,RH;


/******************************************
�������ƣ�GPIO_DQ_Out_Mode
��    �ܣ�����DQ����Ϊ��©���ģʽ
��    ������
����ֵ  ����
*******************************************/
void AM2301_Port_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure ;

	RCC_APB2PeriphClockCmd(RCC_AM2301, ENABLE);  //RCC_APB2Periph_AFIO	
	GPIO_InitStructure.GPIO_Pin = GPIO_AM2301_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIO_AM2301_PORT, &GPIO_InitStructure);
}

unsigned char Get_dat()
{
	unsigned char i,count,dat,Rev_Bit;
	for(i=0;i<8;i++)
	{
		count=1;
		while(AM2301_ReadBit()&&(count++))
		{
			delay_us(1);		
		}
		count=1;
		while(!AM2301_ReadBit()&&(count++))
		{
			delay_us(1);
		}
	delay_us(40);
		if(AM2301_ReadBit())
		{	
			Rev_Bit = 1;
		}
		else
		{
			Rev_Bit = 0;
		}
		dat <<= 1;
		dat |= Rev_Bit;
	}
	return (dat);
}

void Get_AM2301_Data()
{	
	
	unsigned char count;
	unsigned char  temp_check,check_num;
	unsigned char  temp_W_H,temp_W_L,temp_S_H,temp_S_L;//�������ʱ��
	unsigned char Send_Buffer[10];
	unsigned char i;

	AM2301_Write_1(); 						//�뽫AM2321���ߣ�׼��ͨ�ſ�ʼ
  delay_us(10);
	AM2301_Write_0();						//�������͸�λ���壬�Ƽ�ֵΪ1ms
	delay_us(1000);
	AM2301_Write_1();						//�������������ߣ�׼������Ӧ���ź�
	delay_us(50);							//������ʱ50us֮���ȡ���ߵ�ƽ���ж��Ƿ��յ���Ӧ���ź�
	if(!AM2301_ReadBit())					//����յ���Ӧ���ź�˵��ͨ�ųɹ�
	{
		
		LED1=0;
		count++;							//count��Ŀ���Ƿ�ֹͨ�Ź������������ϣ�����while�޷��������������
		while(!AM2301_ReadBit()&&(count++))	//�ȴ�Ӧ���������
		{
			delay_us(1);
		}
		while(AM2301_ReadBit()&&(count++))	//�ȴ��ӻ������ĸߵ�ƽ������׼���������ݽ���״̬
		{
			delay_us(1);
		}
//���ݽ���	
		temp_S_H=Get_dat();                 
        temp_S_L=Get_dat();
        temp_W_H=Get_dat();
        temp_W_L=Get_dat();
        check_num=Get_dat();
		AM2301_Write_1();									//�ͷ�����
		temp_check=(temp_W_H+temp_W_L+temp_S_H+temp_S_L);
		if((temp_check==check_num)&&(temp_check!=0))     	//�ж� �ӻ����͵�У������������պ������У��� �Ƿ����
        {
            RH = temp_S_H;
            RH = (RH<<8) | temp_S_L;
            Tem = temp_W_H;
            Tem = (Tem<<8) | temp_W_L;
			

        }
		else 									//���У�鲻��ȷ��˵��ͨ�ų����ˣ���ʪ��Ϊ0���¶�Ϊ100�궨
		{
			RH =  0;
			Tem = 100;
		}
	}
	else   //���ȶ�״̬ ��ʪ��255���¶�100�궨
	{
		Tem = 100;
		RH = 0xff;		
	}
	
}
