#include "main.h"
#include "IWDG.h"
static u8 A;
static u16 B;
static u16 num_bit =0;

u16 tim3_cnt;
int tim2_cnt; //定时器2计数
u8 swchflag;
static u8 time4flag=0;
static u8 finish;
/***********************************************************
 * 函数名: TIM3_NVIC_Configuration
 * 描述  ：配置定时器T3中断
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/***********************************************************
 * 函数名: TIM3_Configuration
 * 描述  ：配置定时器T3
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM3_Configuration(void)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
		TIM3_NVIC_Configuration();

		TIM_DeInit(TIM3);
		TIM_TimeBaseStructure.TIM_Period=99;		 								
		TIM_TimeBaseStructure.TIM_Prescaler= 9;			//48M/(9+1)*(99+1)=48KHz=9.6K*5的频率进入中断  
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

		TIM_ITConfig(TIM3, TIM_IT_Update , ENABLE);	
		TIM_Cmd(TIM3, ENABLE);	/* 开启时钟 */ 
}
/***********************************************************
 * 函数名: TIM3_OFF
 * 描述  ：关闭定时器T3
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM3_OFF(void)
{
	 TIM_ITConfig(TIM3, TIM_IT_Update , DISABLE);	
	 TIM_Cmd(TIM3, DISABLE);	
}
/***********************************************************
 * 函数名: TIM4_Configuration
 * 描述  ：配置定时器T4,外部时钟触发模式，dds方波作为触发
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM4_Configuration(void)
{
	  /*GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//配置TIM4外部触发IO
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOE, &GPIO_InitStructure);
	
    TIM_DeInit(TIM4);
	
    TIM_TimeBaseStructure.TIM_Period=0xffff;		 								
    TIM_TimeBaseStructure.TIM_Prescaler= 0;				    
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

		TIM_ETRClockMode2Config(TIM4,TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0);
		
		TIM_SetCounter( TIM4, 0x00);
		TIM_Cmd(TIM4, ENABLE);	 */
		
		///////////////////////////////////
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  NVIC_InitTypeDef         NVIC_InitStructure;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		
    TIM_TimeBaseStructure.TIM_Period=24999;		 								
    TIM_TimeBaseStructure.TIM_Prescaler= 1919;//1s				    
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
//		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	  TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断

		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //TIM4
    /*NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		//TIM_ITConfig(TIM4, TIM_IT_Update , DISABLE);
		TIM_Cmd(TIM4, ENABLE);
}

/***********************************************************
 * 函数名: TIM4_ON
 * 描述  ：开启闭定时器T4
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM4_ON(void)
{	
		TIM_Cmd(TIM4, ENABLE);
		//TIM_SetCounter( TIM4, 0x00);
}
/***********************************************************
 * 函数名: TIM4_OFF
 * 描述  ：关闭定时器T4
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM4_OFF(void)
{
	 TIM_Cmd(TIM4, DISABLE);
}
//=============================================
void TIM2_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void TIM2_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	 TIM2_NVIC_Configuration();
	
  TIM_DeInit(TIM2);
  TIM_TimeBaseStructure.TIM_Period=4999;		 					/* 自动重装载寄存器周期的值(计数值) */
  //TIM_TimeBaseStructure.TIM_Prescaler= (36000 - 1);				    /* 时钟预分频数   例如：时钟频率=72MHZ/(时钟预分频+1) */ //0.5ms
	 TIM_TimeBaseStructure.TIM_Prescaler= (8200 - 1);				      /* 时钟预分频数   例如：时钟频率=48MHZ/(时钟预分频+1) */ //0.5ms
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 			/* 采样分频 */
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		/* 向上计数模式 */
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    /* 清除溢出中断标志 */
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM2, ENABLE);											/* 开启时钟 */
}
//=============================================
/***********************************************************
 * 函数名: TIM3_IRQHandler
 * 描述  ：定时器3中断服务函数，按   频率进入中断，根据查找
           表写dds频率字
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
		 TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		}	
}


void TIM2_IRQHandler(void)  //GPS失效时间判断
{
	if( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{
			IWDG_Feed();
			TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
	}	
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );
	}				    	    
}





