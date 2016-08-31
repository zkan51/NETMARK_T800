#include "main.h"
#include "IWDG.h"
#include "stm32f10x_adc.h"
/*********************mkmsg.c*************************/
//-------GPS经纬度历史数据--modified by Wangsi--------//
double gps_lat_tmp[N_medfilt];
double gps_lon_tmp[N_medfilt];
double gps_lat_tmp1[N_medfilt];
double gps_lon_tmp1[N_medfilt];
int idx_medfilt = -1;
bool is_gps_data_init = TRUE;
int idx_tail = -1; //一开始指向最前面的数的前面一个位置
int idx_tail_sog = -1; //sog计算数组使用指针
int len_tail_sog = 0 ;//sog计算数组记录数据数量，计到len_gps_data
u32 gps_time_tag[len_gps_data];
u32 gps_time_tag_sog[len_gps_data];//速率计算使用数组时间
double gps_longitude[len_gps_data];
double gps_latitude[len_gps_data];
double gps_longitude_sog[len_gps_data];//速率计算使用数组经度
double gps_latitude_sog[len_gps_data];//速率计算使用数组纬度
double sog_tmp[sog_num];//存储sog，用于滤波
unsigned int idx_sog_tag = sog_num-1;//速率储存数组tag
double offset_len1=200, offset_len2=200, offset_len3; // 拖网位置的偏移量，单位是米，len1~len3分别代表纵向偏移量、横向偏移量和斜边长度
double offset_len1_tmp = 0, offset_len2_tmp = 0;	  // 用于判断offset的值是否发生变化
double offset_theta; 		// 拖网与航向的夹角
double TP_theta = 0;		// 航行方向与正北方向的夹角
double TP_theta_ = 0;		// 航行方向与x轴正向的夹角
double jingdu_tmp;
double weidu_tmp;
unsigned long direction_tmp;
double X[len_gps_data];
double Y[len_gps_data];
u8 gps_data_available_cnt = 0;
u8 flag_gps_data_available = 0;
unsigned int cog_sample_len = 60; // 用于计算COG的数组长度默认为60
//----------------------------------------------------//

u8 msg[45]={0};//存储AIS消息
u8 len; //输入消息的字节数
unsigned long MMSI=000000111; //九位码
u8 boatnum[16]={0x60,0x34,0x93,0x8c,0x24,0x13,0x99,0x57,0x0d,0x73,0xcc,0x0c,0x31,0x00,0x00,0x00}; //船名 MINLIANYU0533@01
u8 Vender_ID[6]={0x21,0x31,0x2d,0x50,0xd2,0xc0}; //制造商号,消息24B使用 HSD-TMK
u8 Call_sign[6]={0x00,0x00,0x00,0x00,0x00,0x00}; //呼号，消息24B使用
u16 boatlong; //船长
u8 boatwidth; //船宽
u16 boatsize_a ; //距船首 
u16 boatsize_b ; //距船尾 a+b = 船长
u8 boatsize_c ;	//距左舷
u8 boatsize_d ;	//距右舷 c+d = 船宽
double jingdu=0; //GPS经度
double weidu=0; //GPS纬度
unsigned long jingdu_flash; //存到Flash中的GPS精度
unsigned long weidu_flash;//存到Flash 中的GPS纬度
unsigned int sog=0; //msg18 SOG
double sog_double; //	存储算法估计出的SOG（浮点数）
unsigned int sog_gps=0; // 存储由GPS模块获取的SOG
unsigned int sog_sample_interval;
unsigned int direction=0; //msg18 实际航向
unsigned long gpstime=0; //gps时间
//////msg21/////
u8 dev_type = 4;//1;//助航设备类型 5bit
u8 dev_name[54]={0x48,0x05,0x04,0x0C,0x17,0x1C,0xB3,0x04,0x20,0xC4}; //APPLE123ABCD
u8 dev_local[5]={0x0C,0x21,0x24,0x0A,0x0A}; //尺寸位置参考点 30bit；共5个字节,只用低6位 ;6 6 6 6 6 6
u8 dev_time; //时戳 6bit
u8 dev_state; //AtoN状态 8bit
u8 dev_etype = 1; //电子定位装置类型 4bit 默认为GPS

////////////////
/*********************msg_slot.c*************************/
u8 msg18_add[256]={0};//存储消息18对应频率字查找表的地址，每一个元素取值范围在0~16。
u8 msg24A_add[256]={0};//存储消息24A对应频率字查找表的地址，每一个元素取值范围在0~16。
u8 msg24B_add[256]={0};//存 储消息24B对应频率字查找表的地址，每一个元素取值范围在0~16。
u8 msg21_add[512]={0};//存 储消息21对应频率字查找表的地址，每一个元素取值范围在0~16。
u8 msg18_24AB_add[768];//存储消息18和24A/B对应频率字查找表的地址，每一个元素取值范围在0~16。
//u16 msequence_poly=0x8805;	//16位PN码生成多项式，用来产生发射间隔的一个随机数，上位机无线写入。
u16 slot_increacement=0 ; //随机时隙增量，与发射间隔参数interval_s相加，随机化发射时间间隔。
u16 TIM3_CAM1=0;  //时隙号，与RTC计数器中的时隙值比较
u16 TIM3_CAM2=0;
u16 TIM3_CAM3=0;
u16 TIM3_CAM4=0;
u16 TIM3_CAM5=0;  //PLL提前打开的时隙
u16 var_m24; //记录发送消息bit数，当只发送消息18时，var_m24=256*5-1;当发送消息24时，var_m24=512*5-1;
//u16 interval_s=1125; //375;      //2250; //初始发射间隔，1min.   //6750   
u16 interval_s;  //间隔计数变化值 
u16 interval_num; //发射间隔计数，变化值
u16 gps_invalid; //gps失效的次数时间
//u16 gps_invalid_num; //失效次数值
//u16 time_o;//GPS前次时间存储
u16 intervalA;  //间隔计数次数 固定值
u16 intervalB;  //间隔计数次数 超时值
u8 cogsel=0; //流网或张网选择变量 默认0=流网
u16 adc_level;
u8 sendrandom; //发送时隙随机时间；
int  dds_word_real[9][5] = {
														-197 ,-197 ,-197 ,-197 ,-197 ,
														-196 ,-190 ,-172 ,-126 ,-47 ,
														46 ,120 ,147 ,120 ,46 ,
														47 ,126 ,172 ,190 ,196 ,
														-47 ,-126 ,-172 ,-190 ,-196 ,
														-46 ,-120 ,-147 ,-120 ,-46 ,
														196 ,190 ,172 ,126 ,47 ,
														197 ,197 ,197 ,197 ,197 ,
	
														0,0,0,0,0
														}; //无频偏的频率字查找表

//union dds_union dds_word_update[16][5]={0x00000000};  //矫正更新后的频率字查找表

ais_status task_flag2=on;//发送任务完成标志，初值必须置为on,用来标识是否发射AIS消息
ais_status flag_initial_slot=on;  //第一次分配时隙标志，初值一定复位on，第一次GPS有效后10s即发射AIS消息
ais_status flag_m24=on;//是否发送消息24标志；on--发送消息18和消息24；off--只发送消息18；
ais_status task_flag1=off; //波形发射执行标志，初值为off
u8 flag_channel=0;		 //信道选择标志位,初始化为信道1
									
/*********************gps.c*************************/									
GPS_INFO GPS; //gps信息结构体
u8 GPS_RecNum = 0;  //GPS接收的字节数

/*********************exti.c*************************/	
ais_status reset_flag=off; //复位标志，上电默认为on.根据该标志判断是否频偏矫正
u16 fre_cnt=0;  //频偏矫正的误差；
								
u8 CodeNum[6]; //密码
u8 code_cnt=0; //密码正确与否标志。code_cnt=6时密码正确。
ais_status code_flag1=off; //判断密码是否正确，默认不正确。
ais_status code_flag2=off; //是否写过密码标志，默认为未写过。

u16 activation_flag=0;  //激活标志，默认为0
u32 ChipUniqueID_Flash[3];   //芯片96bit UID

/*********************adc.c*************************/
ais_status setting_flag=off; //判断是否处于写码状态，off为非写码状态

//u8 rxbuf1[4] = {0x06,0x9F,0x6B,0xC7}; //temp////////////////////

u8 gps_led_on = 0; //开机绿灯慢闪
u8 battery; //电量等级

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
}

/*********************flash.c*************************/
u32 firstSetTime = 0; //第一次写码的时间
u8 AgencyName[16]={0}; //经销商名字

// Cortex System Control register address
#define SCB_SysCtrl              ((uint32_t)0xE000ED10)
// SLEEPDEEP bit mask
#define SysCtrl_SLEEPDEEP_Set    ((uint32_t)0x00000004)

extern u8 swchflag;

u8 openflag; //开机

u16 rftime; //每30s开一次PA

unsigned long myflag;

int main(void)
{
	unsigned int i;
	SYS_CLK_UP();  //设置系统时钟为48M

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	RCC->APB2ENR|=1<<2;     //先使能外设IO PORTA时钟
	RCC->APB2ENR|=1<<0;     //开启辅助时钟
	GPIOA->CRL&=0XFFFFFFF0;//PA0设置成输入
	GPIOA->CRL|=0X00000008;
	
	stm32_init();  //GPIO	
	
	USART1_Config(); //读写码接口
	USART2_Config(); //GPS接收
	Usart3_Init(9600);
	
	// watch dog
	TIM2_Configuration();
	//TIM4_Configuration();
	IWDG_Init(4,625);//与分频数为64,重载值为625,溢出时间为1s
	
	Adc_Init();
	Delay(0xff);
	
	BackupRegisterInit();
	Read_GPS_Info();
	ReadFlashInit(); //读取flash数据
	SKY72310_Init();
	swchflag = 0;
	
	LED_RED_ON();
	RedDelay();
	LED_RED_OFF();
	SeedSet();
	GPS_ON();
	
	while (1)
  {	
			ProgramSelector();  //拨码开关
		 if(setting_flag==off)   //只有在非写码状态下才发射AIS消息
			{					
				send_on();
			}		
  }
}

