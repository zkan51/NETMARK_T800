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

// �洢һ�����ȵ�GPS��γ����ʷ���ݣ����ں������--modified by Wangsi
#define PI 3.141592653589793
#define len_gps_data 60 // �洢��GPS�������鳤��
#define N_medfilt 7
#define UART_RX2_LEN 400
#define sog_num 5

extern double gps_lat_tmp[N_medfilt];
extern double gps_lon_tmp[N_medfilt];
extern double gps_lat_tmp1[N_medfilt];
extern double gps_lon_tmp1[N_medfilt];
extern int idx_medfilt;
extern bool is_gps_data_init;
extern int idx_tail; // ָʾ�����β���������µ��������ڵ�λ��
extern int idx_tail_sog;//sog����ʹ������ָ�룬ָ����������
extern int len_tail_sog;//sog���������¼�����������Ƶ�len_gps_data
extern u32 gps_time_tag[len_gps_data];
extern double gps_longitude[len_gps_data];
extern double gps_latitude[len_gps_data];
extern double gps_longitude_sog[len_gps_data];//���ʼ���ʹ������
extern double gps_latitude_sog[len_gps_data];//���ʼ���ʹ������
extern u32 gps_time_tag_sog[len_gps_data];//���ʼ���ʹ������
extern double sog_tmp[sog_num];//�洢sog�������˲�
extern unsigned int idx_sog_tag;//���ʴ�������tag
extern unsigned int sog_sample_interval;//���ʼ����������
extern double offset_len1, offset_len2, offset_len3; // ����λ�õ�ƫ����
extern double offset_len1_tmp, offset_len2_tmp;	
extern double offset_theta; // �����뺽��ļн�
extern double TP_theta;		// ���з�������������ļн�
extern double TP_theta_;	// ���з�����x������ļн�
extern double jingdu_tmp;
extern double weidu_tmp;
extern unsigned long direction_tmp;
extern char rev_buf[UART_RX2_LEN];  // ���ջ���
void Delay(__IO u32 nCount);
extern double X[len_gps_data];
extern double Y[len_gps_data];
extern u8 gps_data_available_cnt;
extern u8 flag_gps_data_available;
extern unsigned int cog_sample_len; // ���ڹ���COG�����鳤�ȣ�������λ��ʵʱ�޸�

//////msg21/////
extern u8 dev_type;//�����豸���� 5bit
extern u8 dev_name[54];//�����豸���� 120bit+(0~84)bit
extern u8 dev_local[5]; //�ߴ�λ�òο��� 30bit
extern u8 dev_time; //ʱ�� 6bit
extern u8 dev_state; //AtoN״̬ 8bit
extern u8 dev_etype; //���Ӷ�λװ������ 4bit
////////////////

typedef enum
{ 
	off=0,
	on
}ais_status;

extern unsigned long MMSI;
extern u8 boatnum[16];
extern u8 CodeNum[6]; //����
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
extern unsigned long jingdu_flash; //�浽Flash�е�GPS����
extern unsigned long weidu_flash;//�浽Flash �е�GPSγ��
extern unsigned int sog;
extern double sog_double; // ��������sog����ֵ
extern unsigned int sog_gps;
extern unsigned int direction;
extern unsigned long gpstime;
extern u8 msg[45];
extern GPS_INFO   GPS;
extern u8 len; //������Ϣ���ֽ���
extern u16 slot_increacement; //���ʱ϶��������������������
extern ais_status task_flag2;//����������ɱ�־ !!��ֵ������Ϊ1,������ʶ�Ƿ����ʱ϶������������� 
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

extern u16 interval_s;  //�������������仯ֵ
extern u16 interval_num; //����������
extern u16 gps_invalid; //gpsʧЧ�Ĵ�����־
//extern u16 gps_invalid_num; //ʧЧ����ֵ
//extern u16 time_o;//GPSǰ��ʱ��洢
extern u16 intervalA;  //����������� �̶�ֵ
extern u16 intervalB;  //����������� ��ʱֵ
extern u8 cogsel;

extern u16 var_m24;
extern ais_status flag_m24;
//extern u16 msequence_poly;	//��λ��д�� ����ƵĽӿڡ�
extern u8 flag_channel;		 //�ŵ�ѡ���־λ,��ʼ��Ϊ�ŵ�1
extern ais_status flag_initial_slot;
extern u8 code_cnt; //������ȷ����־��code_flag1=6ʱ������ȷ
extern ais_status code_flag1; //�ж������Ƿ���ȷ��Ĭ�ϲ���ȷ
extern ais_status code_flag2; //�Ƿ�д�������־��Ĭ��Ϊδд����
extern ais_status setting_flag;
extern ais_status reset_flag;

extern u16 activation_flag;//�����־
extern u32 ChipUniqueID_Flash[3];   //оƬ96bit UID

extern u8 rxflag; //���ͱ�־,��ֹ����ʱ�����ж�
extern u8 status;	//�����жϽ���/����״̬
extern u8 tx1buf[31]; 	   //����1���ͻ���

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
