#include "main.h"
#include <stdarg.h>
#include "stm32f10x_dma.h"

//static u8 com3_start;
//static u8 com3_stop;
//static u8 com3_num;

#define UART_RX1_LEN     100 
static u8 com1_rxbuf[UART_RX1_LEN];  //串口1接收缓存
u8 tx1buf[31]={0}; 	   //串口1发送缓冲
extern u8 swchflag;

 /***********************************************************
 * 函数名: USART1_NVIC_Configuration
 * 描述  ：配置串口1中断
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
static void USART1_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void UART1_DMA_Configuration(void)
{
	  DMA_InitTypeDef DMA_InitStructure;
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //启动DMA时钟  

			DMA_DeInit(DMA1_Channel5); 	//USART1 DMA1通道5配置   
			DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);//外设地址  
			DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)com1_rxbuf;     //内存地址  
			DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;            //dma传输方向从外设到内存 
			DMA_InitStructure.DMA_BufferSize = UART_RX1_LEN;               //设置DMA在传输时缓冲区的长度  
			DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//设置DMA的外设地址不变 
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //设置DMA的内存递增模式  
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据字长 8位
			DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //内存数据字长8位   
			DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                //设置DMA的传输模式   
			DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;      //设置DMA的优先级别 最高
			DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                 //设置DMA的2个memory中的变量互相访问  
			DMA_Init(DMA1_Channel5,&DMA_InitStructure);
	
			DMA_Cmd(DMA1_Channel5,ENABLE);
}

 /***********************************************************
 * 函数名: USART1_Config
 * 描述  ：配置串口1，9600波特率
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	UART1_DMA_Configuration();
	USART1_NVIC_Configuration();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);    
  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx| USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1,USART_IT_TC,DISABLE);    //禁止 
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);  //禁止 
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);   //开启

	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);	//采用DMA方式接收 
 USART_Cmd(USART1, ENABLE);
}

/************************************************************
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 ************************************************************/
//------ modified by Wangsi------
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}

/*************************************************************
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART1_printf()调用
 ************************************************************/
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/*********************************************************************
 * 函数名：USART1_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口1，即USART1
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART1_printf( USART1, "\r\n this is a demo \r\n" );
 *            		 USART1_printf( USART1, "\r\n %d \r\n", i );
 *            		 USART1_printf( USART1, "\r\n %s \r\n", j );
 ***********************************************************************/
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}

void USART1_IRQHandler(void)
{   
	u8 i;
	uint32_t Length = 0;//,i=0;
	u16 crcdata,scrData; //校验生成的数据, 源校验的数据
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
	{  
		DMA_Cmd(DMA1_Channel5,DISABLE); 
		Length = USART1->SR;  
		Length = USART1->DR; //清USART_IT_IDLE标志
 
		if(com1_rxbuf[1]==0x31|com1_rxbuf[1]==0x06|com1_rxbuf[1]==0x32)	
		{
			scrData = (com1_rxbuf[16]<<8)+com1_rxbuf[17];
			crcdata = msg_crc(com1_rxbuf,16);		
			if(scrData==crcdata)
				Usart1GetCommand();
			for(i=0;i<18;i++)
				com1_rxbuf[i] = 0;
		}
		else
		{
			Usart1GetCommand();
			for(i=0;i<18;i++)
				com1_rxbuf[i] = 0;
		}
		
		DMA1_Channel5->CNDTR = UART_RX1_LEN;//重装填,并让接收地址偏址从0开始
		DMA_Cmd(DMA1_Channel5, ENABLE);//处理完,重开DMA   
		
		//USART_ClearITPendingBit(USART1,USART_IT_IDLE);
	}
	__nop();   	
	
}

//判断t800当前插入在T81上的位置
void positionSel()
{
	u8 i;
		//T88写码插入的位置与设置的是否一致，上位机读取时com1_rxbuf[7]为0
		if(com1_rxbuf[7] == 1)//插入的位置为左舷
		{
			if(offset_len2 >= 0) //之前设置的值为右舷
			{
				offset_len2 = -50; // 设置默认值50
				WriteTuoWangInfo();
			}
			for(i=0;i<4;i++)
				tx1buf[i+8] = MMSI >> (24 - i*8);
		}
		else if(com1_rxbuf[7] == 2)//网尾
		{
			offset_len2 = 0;
			WriteTuoWangInfo();
			for(i=0;i<4;i++)
				tx1buf[i+8] = MMSI >> (24 - i*8);
		}
		else if(com1_rxbuf[7] == 3)//右舷
		{
			if(offset_len2 <= 0)//之前设置的值为左舷
			{
				offset_len2 = 50;
				WriteTuoWangInfo();
			}
			for(i=0;i<4;i++)
				tx1buf[i+8] = MMSI >> (24 - i*8);
		}
}

void com1sendback(void)   
{
	u8 i;
	u16 data;
	if(tx1buf[1]==0x31 | tx1buf[1]==0x06 |tx1buf[1]==0x32)
	{
		data = msg_crc(tx1buf,16);	//CRC校验数据生成
		tx1buf[16] = data>>8;
		tx1buf[17] = data;
	}
	for(i=0;i<18;i++)	
	{
		USART_SendData(USART1, tx1buf[i]);
		while (!(USART1->SR & USART_FLAG_TXE)) ;
	}
}


void Usart1GetCommand(void)  //串口1接收
{
	u8 i;
	
	if(com1_rxbuf[0] == 0x24)
	{
		switch(com1_rxbuf[1])	
		{
			case 0x01://向网位仪写船名
			{
				if(setting_flag == on)
				{
						for(i=0;i<16;i++)
						boatnum[i] = com1_rxbuf[i+2];
						WriteflashBoatnum();
						ReadflashBoatnum();

						tx1buf[0] = '$'; 
					 tx1buf[1] = 0x01;
					 tx1buf[2] = 0x01; //应答 写入船名成功
						for(i=3;i<18;i++)
							tx1buf[i]=0x00;
						com1sendback();
				}
			}
			break;
			
			case 0x02://向网位仪写MMSI号
			{
				if(setting_flag == on)
				{
						MMSI=0;
						MMSI += (com1_rxbuf[2] << 24); //+ (rxbuf[2] << 16) + (rxbuf[3] << 8) + rxbuf[4];
						MMSI += (com1_rxbuf[3] << 16);
						MMSI += (com1_rxbuf[4] << 8);
						MMSI += com1_rxbuf[5];
						WriteflashMMSI();
						ReadflashMMSI();
				
						tx1buf[0] = '$'; tx1buf[1] = 0x02;tx1buf[2] = 0x01; //正确写入mmsi
						for(i=3;i<18;i++)
							tx1buf[i]=0x00;
						com1sendback();
						//code_flag1=off;  //设置密码错误标志
				}
			}
			break;
			
			case 0x04://向网位仪读取船名
			{
				if(setting_flag == on)
				{
				tx1buf[0] = '$'; 
				tx1buf[1] = 0x04;
				
				for(i=0;i<16;i++)   
				tx1buf[i+2]=boatnum[i];
					com1sendback();
				}
			}
			break;
				
			case 0x05://向网位仪读取MMSI号
			{
				if(setting_flag == on)
				{
					tx1buf[0] = '$';tx1buf[1] = 0x05;
					for(i=0;i<4;i++)
						tx1buf[i+2] = MMSI >> (24 - i*8);
					for(i=6;i<18;i++)
						tx1buf[i]=0x00;

					com1sendback();		
				}
			}
			break;
			
			case 0x06://读取航速航向
								tx1buf[0] = '$';
								tx1buf[1] = 0x06;
								tx1buf[2] = sog>>8;
								tx1buf[3] = sog;
								tx1buf[4] = direction>>8;
								tx1buf[5] = direction;
								for(i=6;i<18;i++)
								{
										tx1buf[i]=0;
								}
								//MMSI
								for(i=0;i<4;i++)
									tx1buf[i+8] = MMSI >> (24 - i*8);
								tx1buf[7] = com1_rxbuf[7];
								com1sendback();
								break;
								
			case 0x17: //连接网位仪
			{
				setting_flag = on;  //进入写码状态
				Enter_Setting();
				
				tx1buf[0] = '$'; 
				tx1buf[1] = 0x17; 
				tx1buf[2] = 0x01;
				tx1buf[7] = com1_rxbuf[7];
				for(i=8;i<18;i++)   
					tx1buf[i]=0x00;
				
				//判断是不是第一次写码
				if(firstSetTime==0xffffffff)
					tx1buf[3] = 0x01;
				else
					tx1buf[3] = 0x00;
				com1sendback();
			}
			break;

			case 0x18: //断开网位仪
			{
				tx1buf[0] = '$'; tx1buf[1] = 0x18;
				for(i=2;i<18;i++)   tx1buf[i]=0x00;
				tx1buf[7] = com1_rxbuf[7];
				com1sendback();
 			setting_flag = off; //退出写码状态
				task_flag2=on;
				LED_OFF();
				LED_RED_OFF();			
				GPS_RecNum = 0;
    USART_Cmd(USART2,ENABLE);				
			}
			break;
			
			case 0x31:  //拖网位置 注入
			{
//				if(setting_flag == on)
//				{
						offset_len1 = (com1_rxbuf[2] << 8) + com1_rxbuf[3]; //纵偏移
						if (com1_rxbuf[4] == 0)
							offset_len2 =((com1_rxbuf[5] << 8) + com1_rxbuf[6])*-1;					//横偏移
						else 
							offset_len2 = (com1_rxbuf[5] << 8) + com1_rxbuf[6];
						
						//计算SOG的平均程度
						sog_sample_interval = com1_rxbuf[8];
						cog_sample_len = com1_rxbuf[9];
						if(sog_sample_interval<5)
							sog_sample_interval = 5;
						if(cog_sample_len<5)
							cog_sample_len=5;

						WriteFlash_AveragParam();						
						
						WriteTuoWangInfo();
						tx1buf[0] = '$'; 
						tx1buf[1] = 0x31; 
						tx1buf[2] = 0x01;
						for(i=3;i<18;i++)   
							tx1buf[i]=0x00;
						tx1buf[7] = com1_rxbuf[7];
						com1sendback();
//				}
			}
			break;
			
			case 0x32:  //拖网网位仪 读取
			{
					int offset1,offset2;	
					for(i=0;i<18;i++)
					tx1buf[i] = 0;		
					
					ReadTuoWangInfo();
				 
					//positionSel();
					offset1 = offset_len1;
					offset2 = offset_len2;
					tx1buf[0] = '$';
					tx1buf[1] = 0x32;
					tx1buf[2] = (int)offset1 >> 8;
					tx1buf[3] = (int)offset1;
					if(offset2 > 0) 
								tx1buf[4] = 1;
					else
					{
						tx1buf[4] = 0;
						offset2 = -offset2;
					}
					tx1buf[5] = (int)offset2 >> 8;
					tx1buf[6] = (int)offset2;
					tx1buf[7] = com1_rxbuf[7];
					//MMSI
					for(i=0;i<4;i++)
						tx1buf[i+8] = MMSI >> (24 - i*8);
					for (i=12 ;i<18 ;i++)
							tx1buf[i] = 0x00;
					com1sendback();
			}
			break;				
			
			case 0x33: //注入第一次写码时间
							{
								if(setting_flag == on)
								{
									firstSetTime = 0;
									firstSetTime += (com1_rxbuf[2]<<24);
									firstSetTime += (com1_rxbuf[3]<<16);
									firstSetTime += (com1_rxbuf[4]<<8);
									firstSetTime += (com1_rxbuf[5]);
									WriteFlash_Time();
									tx1buf[0] = '$';
									tx1buf[1] = 0x33;
									tx1buf[2] = 0x01;
									for(i=3;i<18;i++)   
										tx1buf[i]=0x00;
									com1sendback();
								}
							}	
							break;
							
		case 0x34: //读取第一次写码时间
						{
							if(setting_flag == on)
							{
								tx1buf[0] = '$';
								tx1buf[1] = 0x34;
								tx1buf[2] = firstSetTime>>24;
								tx1buf[3] = firstSetTime>>16;
								tx1buf[4] = firstSetTime>>8;
								tx1buf[5] = firstSetTime;						
								for(i=6;i<18;i++)
									tx1buf[i] = 0;
								com1sendback();
							}
						}	
						break;			
						
		case 0x35: //经销商名字注入
						{
							if(setting_flag == on)
							{
								for(i=3;i<18;i++)
									AgencyName[i-3] = com1_rxbuf[i];
								
								WriteFlash_AgencyName();	
								
								tx1buf[0] = '$';
								tx1buf[1] = 0x35;
								tx1buf[2] = 0x01;
								for(i=3;i<18;i++)
									tx1buf[i] = 0;
								
								com1sendback();
							}
						}	
						break;						
				
		case 0x36: //经销商名字读取
						{
							if(setting_flag == on)
							{
								tx1buf[0] = '$';
								tx1buf[1] = 0x36;
								tx1buf[2] = 0x01;
								for(i=0;i<16;i++)
									tx1buf[i+3] = AgencyName[i];
								com1sendback();
							}
						}	
						break;		
						
			default:
			break;		
		}	
	}
}





