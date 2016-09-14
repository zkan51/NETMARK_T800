#include "main.h"
void RedDelay(void)
{
	u8 i;
	for(i=0;i<90;i++)
			Delay(45000);
}

void LedFlash(void)  //ÉÁÒ»´Î
{
	LED_ON();
	delay_ms(1000);
	if(setting_flag==off)
		LED_OFF();	
}

