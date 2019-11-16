
#ifndef __BLE_H
#define __BLE_H

#include "sys.h"	

#define BLE_TX_PIN			   GPIO_Pin_10
#define BLE_RX_PIN		   	 GPIO_Pin_11 

#define BLE_USARTx UART4



void BLE_Init(u32 bound);
void BLE_Enable(void);
void BLE_Disable(void);
void SendInt(const u8 ID, const u32 a);
void SendFloat(const u8 ID,const float a);


#endif
