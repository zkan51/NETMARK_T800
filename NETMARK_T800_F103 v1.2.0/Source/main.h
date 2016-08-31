#ifndef __MAIN_H
#define	__MAIN_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "math.h"

#include "config.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "SKY72310.h"
#include "msg_slot.h"
#include "timer.h"
#include "gps.h"
#include "gps_algorithm.h"
#include "mkmsg.h"	
#include "cmdfunc.h"
#include "rtc.h"
#include "flashwr.h"
#include "power.h"
#include "adc.h"
#include "led.h"
#include "test.h"
#include "stm32f10x_dma.h"

// 存储一定长度的GPS经纬度历史数据，用于航向估计--modified by Wangsi
#define PI 3.141592653589793
#define len_gps_data 60 // 存储的GPS数据数组长度
#define N_medfilt 7
#define UART_RX2_LEN 400
#define sog_num 5

extern double gps_lat_tmp[N_medfilt];
extern double gps_lon_tmp[N_medfilt];
extern double gps_lat_tmp1[N_medfilt];
extern double gps_lon_tmp1[N_medfilt];
extern int idx_medfilt;
extern bool is_gps_data_init;
extern int idx_tail; // 指示数组的尾部，即最新的数据所在的位置
extern int idx_tail_sog;//sog计算使用数组指针，指向最新数据
extern int len_tail_sog;//sog计算数组记录数据数量，计到len_gps_data
extern u32 gps_time_tag[len_gps_data];
extern double gps_longitude[len_gps_data];
extern double gps_latitude[len_gps_data];
extern double gps_longitude_sog[len_gps_data];//速率计算使用数组
extern double gps_latitude_sog[len_gps_data];//速率计算使用数组
extern u32 gps_time_tag_sog[len_gps_data];//速率计算使用数组
extern double sog_tmp[sog_num];//存储sog，用于滤波
extern unsigned int idx_sog_tag;//速率储存数组tag
extern unsigned int sog_sample_interval;//速率计算采样点间隔
extern double offset_len1, offset_len2, offset_len3; // 拖网位置的偏移量
extern double offset_len1_tmp, offset_len2_tmp;	
extern double offset_theta; // 拖网与航向的夹角
extern double TP_theta;		// 航行方向与正北方向的夹角
extern double TP_theta_;	// 航行方向与x轴正向的夹角
extern double jingdu_tmp;
extern double weidu_tmp;
extern unsigned long direction_tmp;
extern char rev_buf[UART_RX2_LEN];  // 接收缓存
void Delay(__IO u32 nCount);
extern double X[len_gps_data];
extern double Y[len_gps_data];
extern u8 gps_data_available_cnt;
extern u8 flag_gps_data_available;
extern unsigned int cog_sample_len; // 用于估计COG的数组长度，可由上位机实时修改

//////msg21/////
extern u8 dev_type;//助航设备类型 5bit
extern u8 dev_name[54];//助航设备名称 120bit+(0~84)bit
extern u8 dev_local[5]; //尺寸位置参考点 30bit
extern u8 dev_time; //时戳 6bit
extern u8 dev_state; //AtoN状态 8bit
extern u8 dev_etype; //电子定位装置类型 4bit
////////////////

typedef enum
{ 
	off=0,
	on
}ais_status;

extern unsigned long MMSI;
extern u8 boatnum[16];
extern u8 CodeNum[6]; //密码
extern u8 Vender_ID[6];
extern u8 Call_sign[6];
extern u16 boatlong; 
extern u8 boatwidth; 
extern u16 boatsize_a; 
extern u16 boatsize_b; 
extern u8 boatsize_c;	
extern u8 boatsize_d;	
extern double jingdu;
extern double weidu;
extern unsigned long jingdu_flash; //存到Flash中的GPS精度
extern unsigned long weidu_flash;//存到Flash 中的GPS纬度
extern unsigned int sog;
extern double sog_double; // 输出浮点的sog估计值
extern unsigned int sog_gps;
extern unsigned int direction;
extern unsigned long gpstime;
extern u8 msg[45];
extern GPS_INFO   GPS;
extern u8 len; //输入消息的字节数
extern u16 slot_increacement; //随机时隙增量，由随机化程序产生
extern ais_status task_flag2;//发送任务完成标志 !!初值必须置为1,用来标识是否进入时隙号随机化函数； 
extern ais_status task_flag1;

//extern union dds_union dds_word_update[16][5];
//extern char  dds_word_real_num[8][5];
extern int  dds_word_real[9][5];
// extern u16 dds_w1[17][5];
// extern u16 dds_w2[17][5];
// extern u16 dds_w3[17][5];
// extern u16 dds_w4[17][5];
extern u16 fre_cnt;

extern u8 msg18_24AB_add[768];
extern u8 msg18_add[256];
extern u8 msg24A_add[256];
extern u8 msg24B_add[256];
extern u8 msg21_add[256*2];

// extern u8 valid_cnt_m18;
// extern u8 valid_cnt_m24A;
// extern u8 valid_cnt_m24B;

extern u16 TIM3_CAM1;
extern u16 TIM3_CAM2;
extern u16 TIM3_CAM3;
extern u16 TIM3_CAM4;
extern u16 TIM3_CAM5;

extern u16 interval_s;  //发射间隔计数，变化值
extern u16 interval_num; //发射间隔计数
extern u16 gps_invalid; //gps失效的次数标志
//extern u16 gps_invalid_num; //失效次数值
//extern u16 time_o;//GPS前次时间存储
extern u16 intervalA;  //间隔计数次数 固定值
extern u16 intervalB;  //间隔计数次数 超时值
extern u8 cogsel;

extern u16 var_m24;
extern ais_status flag_m24;
//extern u16 msequence_poly;	//上位机写入 与控制的接口。
extern u8 flag_channel;		 //信道选择标志位,初始化为信道1
extern ais_status flag_initial_slot;
extern u8 code_cnt; //密码正确与否标志。code_flag1=6时密码正确
extern ais_status code_flag1; //判断密码是否正确，默认不正确
extern ais_status code_flag2; //是否写过密码标志，默认为未写过。
extern ais_status setting_flag;
extern ais_status reset_flag;

extern u16 activation_flag;//激活标志
extern u32 ChipUniqueID_Flash[3];   //芯片96bit UID

extern u8 rxflag; //发送标志,防止发送时进入中断
extern u8 status;	//用于判断接收/发送状态
extern u8 tx1buf[31]; 	   //串口1发送缓冲

extern u8 gps_led_on;
extern u8 GPS_RecNum;
extern u8 openflag;

extern u8 battery;
extern u16 rftime;

extern void RedDelay(void);

extern u32 firstSetTime ;
extern u8 AgencyName[16];

#define SEND_INTERVAL                        ((uint16_t)9000)
#define BATTERYLEVEL 4

#endif /* __MAIN_H */
