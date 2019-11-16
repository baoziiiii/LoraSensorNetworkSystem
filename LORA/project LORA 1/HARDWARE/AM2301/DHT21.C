
#include "delay.h"
#include "sys.h"
#include "stm32f10x.h"
#include "DHT21.h"

#define RCC_AM2301				RCC_APB2Periph_GPIOB
#define GPIO_AM2301_PORT		GPIOB  
#define GPIO_AM2301_PIN			GPIO_Pin_1

#define AM2301_Write_1()     GPIO_SetBits(GPIO_AM2301_PORT ,GPIO_AM2301_PIN)  //写1
#define AM2301_Write_0()     GPIO_ResetBits(GPIO_AM2301_PORT ,GPIO_AM2301_PIN)//写0
#define AM2301_ReadBit()     GPIO_ReadInputDataBit(GPIO_AM2301_PORT ,GPIO_AM2301_PIN) //读DQ上
unsigned short Tem_Past,RH_Past,Tem,RH;
void AM2301_Port_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure ;

	RCC_APB2PeriphClockCmd(RCC_AM2301, ENABLE);  //RCC_APB2Periph_AFIO	
	GPIO_InitStructure.GPIO_Pin = GPIO_AM2301_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //双向IO
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
	unsigned char Tem_Sign = 0;
	unsigned char count;
	unsigned char  temp_check,check_num;
	unsigned char  temp_W_H,temp_W_L,temp_S_H,temp_S_L;//上面的临时数
	unsigned char Send_Buffer[10];
	unsigned char i;

	AM2301_Write_1(); 						//想将AM2321拉高，准备通信开始
	delay_us(10);
	AM2301_Write_0();						//主机发送复位脉冲，推荐值为1ms
	delay_us(1000);
	AM2301_Write_1();						//主机将总线拉高，准备接收应答信号
	delay_us(50);							//主机延时50us之后读取总线电平，判断是否收到了应答信号
	if(!AM2301_ReadBit())					//如果收到了应答信号说明通信成功
	{
		count++;							//count的目的是防止通信过程中器件故障，导致while无法跳出，造成死机
		while(!AM2301_ReadBit()&&(count++))	//等待应答脉冲结束
		{
			delay_us(1);
		}
		while(AM2301_ReadBit()&&(count++))	//等待从机发出的高电平结束，准备进入数据接收状态
		{
			delay_us(1);
		}
//数据接收	
		temp_S_H=Get_dat();                 
        temp_S_L=Get_dat();
        temp_W_H=Get_dat();
        temp_W_L=Get_dat();
        check_num=Get_dat();
		AM2301_Write_1();									//释放总线
		temp_check=(temp_W_H+temp_W_L+temp_S_H+temp_S_L);
		if((temp_check==check_num)&&(temp_check!=0))     	//判断 从机发送的校验和与主机接收后算出的校验和 是否相等
        {
            RH = temp_S_H;
            RH = (RH<<8) | temp_S_L;
            Tem = temp_W_H;
            Tem = (Tem<<8) | temp_W_L;
			if(Tem&0x8000)
			{
				Tem &= 0x7fff;
				Tem_Sign = 1;  //看温度是否为负
			}
			else
			{
				Tem_Sign = 0;
			}

        }
		else 									//如果校验不正确，说明通信出错了，用湿度为0和温度为100标定
		{
			RH =  0;
			Tem = 100;
		}
	}
	else
	{
		Tem = 100;
		RH = 0xff;		
	}
	for(i=0;i<12;i++)
	{
		Send_Buffer[0] = 0x55;
		if(Tem_Sign == 1)
		{
			Send_Buffer[1] = 0x31;
		}
		else
		{
			Send_Buffer[1] = 0x30;
		}
		Send_Buffer[2] = (Tem/100)|0x30;
		Send_Buffer[3] = (Tem%100/10)|0x30;
		Send_Buffer[4] = (Tem%10)|0x30;
		Send_Buffer[5] = 0x30;
		Send_Buffer[6] = (RH/100)|0x30;
		Send_Buffer[7] = (RH%100/10)|0x30;
		Send_Buffer[8] = (RH%10)|0x30;
		Send_Buffer[9] = 0xaa;

	}
}
