#include "main.h"
#include "stm32f10x_adc.h"			

void BackupRegisterInit(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);  //for �͹���ģʽ �ؿ�
	PWR_BackupAccessCmd(ENABLE);
}
	
void Enter_Setting(void)
{
	TIM3_OFF();
 PA_OFF();
	PLL_OFF();
	GPS_OFF();
	LED_ON();
	LED_RED_ON();
	
	task_flag2=off; //����������
	
}

//����ֵ�������ȼ�
u8 GetBatteryPower(void) 
{
	u16 batteryValue; //����ADֵ
	
	batteryValue = Get_Adc_Average(ADC_Channel_9,5);
	
	if(batteryValue > 3722)	battery = 9;  //>8v
	else if((batteryValue > 3629) && (batteryValue <= 3722 ))	battery = 8;  //7.8v-8v
	else if((batteryValue > 3542) && (batteryValue <= 3629))	battery = 7;  //7.6v-7.8v
	else if((batteryValue > 3447) && (batteryValue <= 3542))	battery = 6;  //7.4v-7.6v
	else if((batteryValue > 3352) && (batteryValue <= 3447))	battery = 5;  //7.2v-7.4v
	else if((batteryValue > 3261) && (batteryValue <= 3352))	battery = 4;  //7.0v-7.2v
	else if((batteryValue > 3168) && (batteryValue <= 3261))	battery = 3;  //6.8v-7.0v
	else if((batteryValue > 3076) && (batteryValue <= 3168))	battery = 2;  //~6.6v-6.8v
	else 	battery = 1;
	
	return battery;
}

