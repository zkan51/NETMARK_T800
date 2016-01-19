#include "main.h"

static u8 rtc_cnt=0;
//static u8 lse_cnt=0;//等待LSE ready 计数
/*
 * 函数名：SoftReset
 * 描述  ：软件复位
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
void SoftReset(void)
{
 __set_FAULTMASK(1);      // ??????
 NVIC_SystemReset();// ??
}
/*
 * 函数名：NVIC_Configuration
 * 描述  ：配置RTC秒中断的主中断优先级为1，次优先级为0
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
static void RTC_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：RTC_Configuration
 * 描述  ：配置RTC
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
static void RTC_Configuration(void)
{
	rtc_cnt=0;
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

//   /* Enable LSE */
//   RCC_LSEConfig(RCC_LSE_ON);
//   /* Wait till LSE is ready */
//   while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
//   {
// 		lse_cnt++;
// // 		USART_SendData(USART1, (unsigned char) lse_cnt);
// // 			while (!(USART1->SR & USART_FLAG_TXE));
// 		Delay(100000);
// 		if(lse_cnt >= 250)
// 		{
// 			SoftReset();
// 		}
// 	}

  /* Select HSE/128 as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);  //62.5KHz

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Alarm */
  RTC_ITConfig(RTC_IT_ALR, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1/37.5 sec ;make sure that the time of one counter is a AIS slot*/
  RTC_SetPrescaler(1666); /* RTC period = RTCCLK/RTC_PR = (62.5 KHz)/(1666+1) */
	
	/* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
	
	/*Sets the RTC alarm value:Sets the RTC alarm value to 1800sec*/
	RTC_SetAlarm(SEND_INTERVAL);  // RTC时钟计数到的时隙数
	
	/* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
	
	/*Sets the RTC counter value:Sets the RTC counter value to 0*/
	RTC_SetCounter(0x0);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/*
 * 函数名：RTC_Init
 * 描述  ：初始化RTC
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void RTC_Init(void)
{
		/* 配置RTC秒中断优先级 */
	RTC_NVIC_Configuration();


    /* RTC Configuration */
    RTC_Configuration();

}

/**
  * @brief  Gets the RTC Low counter value.
  * @param  None
  * @retval : RTC counter value.
  */
uint16_t Get_RTC_Counter(void)
{
  uint16_t tmp = 0;
  tmp = RTC->CNTL;
  return (tmp) ;
}

/**
  * @brief  This function handles RTC global interrupt request.
  * @param  None
  * @retval : None
  */
void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_ALR);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
		
    /* Reset RTC Counter */
		RTC_SetCounter(0x0);
		/* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
		
		rtc_cnt++;

// 		if( (charging_flag==off) && rtc_cnt>=8)   //只有在非充电状态下才复位 //修改rtc_cnt的值以改变重启程序间隔
// 		{
// 			SoftReset();  //soft reset
// 		}
//     if(rtc_cnt>=9)
// 			rtc_cnt=0;
  }
}
