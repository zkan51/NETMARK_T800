#include "main.h"
#include "stm32f10x_adc.h"			

void BackupRegisterInit(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);  //for 低功耗模式 必开
	PWR_BackupAccessCmd(ENABLE);
}
	
void Enter_Setting(void)
{
	USART_Cmd(USART2, DISABLE);
	LED_ON();
	LED_RED_ON();
}


