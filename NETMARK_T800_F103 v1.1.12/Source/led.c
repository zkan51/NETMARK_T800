#include "main.h"
void RedDelay(void)
{
	u8 i;
	for(i=0;i<90;i++)
			Delay(45000);
}

void LedFlash(void)  //��һ��
{
	LED_ON();
 RedDelay();
	if(setting_flag == off)	LED_OFF();	
}

void LedShutdown(void) //�ػ�����
{
	u16 i;
	
	for(i=0;i<10;i++)
	{
		LED_ON();
		Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
		Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
		Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);

		if(setting_flag != on)
		{
			LED_OFF();
			Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
			Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
			Delay(30000);Delay(30000);Delay(30000);Delay(30000);Delay(30000);
		}
	}
}
