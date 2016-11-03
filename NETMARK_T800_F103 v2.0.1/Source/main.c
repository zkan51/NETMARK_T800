#include "main.h"
#include "IWDG.h"
#include "stm32f10x_adc.h"
/*********************mkmsg.c*************************/
//-------GPS��γ����ʷ����--modified by Wangsi--------//
double gps_lat_tmp[N_medfilt];
double gps_lon_tmp[N_medfilt];
double gps_lat_tmp1[N_medfilt];
double gps_lon_tmp1[N_medfilt];
int idx_medfilt = -1;
bool is_gps_data_init = TRUE;
int idx_tail = -1; //һ��ʼָ����ǰ�������ǰ��һ��λ��
int idx_tail_sog = -1; //sog��������ʹ��ָ��
int len_tail_sog = 0 ;//sog���������¼�����������Ƶ�len_gps_data
u32 gps_time_tag[len_gps_data];
u32 gps_time_tag_sog[len_gps_data];//���ʼ���ʹ������ʱ��
double gps_longitude[len_gps_data];
double gps_latitude[len_gps_data];
double gps_longitude_sog[len_gps_data];//���ʼ���ʹ�����龭��
double gps_latitude_sog[len_gps_data];//���ʼ���ʹ������γ��
double sog_tmp[sog_num];//�洢sog�������˲�
unsigned int idx_sog_tag = sog_num-1;//���ʴ�������tag
double offset_len1=200, offset_len2=200, offset_len3; // ����λ�õ�ƫ��������λ���ף�len1~len3�ֱ��������ƫ����������ƫ������б�߳���
double offset_len1_tmp = 0, offset_len2_tmp = 0;	  // �����ж�offset��ֵ�Ƿ����仯
double offset_theta; 		// �����뺽��ļн�
double TP_theta = 0;		// ���з�������������ļн�
double TP_theta_ = 0;		// ���з�����x������ļн�
double jingdu_tmp;
double weidu_tmp;
unsigned long direction_tmp;
double X[len_gps_data];
double Y[len_gps_data];
u8 gps_data_available_cnt = 0;
u8 flag_gps_data_available = 0;
unsigned int cog_sample_len = 60; // ���ڼ���COG�����鳤��Ĭ��Ϊ60
//----------------------------------------------------//

u8 msg[45]={0};//�洢AIS��Ϣ
u8 len; //������Ϣ���ֽ���
unsigned long MMSI=000000111; //��λ��
u8 boatnum[16]={0x60,0x34,0x93,0x8c,0x24,0x13,0x99,0x57,0x0d,0x73,0xcc,0x0c,0x31,0x00,0x00,0x00}; //���� MINLIANYU0533@01
u8 Vender_ID[6]={0x21,0x31,0x2d,0x50,0xd2,0xc0}; //�����̺�,��Ϣ24Bʹ�� HSD-TMK
u8 Call_sign[6]={0x00,0x00,0x00,0x00,0x00,0x00}; //���ţ���Ϣ24Bʹ��
u16 boatlong; //����
u8 boatwidth; //����
u16 boatsize_a ; //�ബ�� 
u16 boatsize_b ; //�ബβ a+b = ����
u8 boatsize_c ;	//������
u8 boatsize_d ;	//������ c+d = ����
double jingdu=0; //GPS����
double weidu=0; //GPSγ��
unsigned long jingdu_flash; //�浽Flash�е�GPS����
unsigned long weidu_flash;//�浽Flash �е�GPSγ��
unsigned int sog=0; //msg18 SOG
double sog_double; //	�洢�㷨���Ƴ���SOG����������
unsigned int sog_gps=0; // �洢��GPSģ���ȡ��SOG
unsigned int sog_sample_interval;
unsigned int direction=0; //msg18 ʵ�ʺ���
unsigned long gpstime=0; //gpsʱ��
//////msg21/////
u8 dev_type = 4;//1;//�����豸���� 5bit
u8 dev_name[54]={0x48,0x05,0x04,0x0C,0x17,0x1C,0xB3,0x04,0x20,0xC4}; //APPLE123ABCD
u8 dev_local[5]={0x0C,0x21,0x24,0x0A,0x0A}; //�ߴ�λ�òο��� 30bit����5���ֽ�,ֻ�õ�6λ ;6 6 6 6 6 6
u8 dev_time; //ʱ�� 6bit
u8 dev_state; //AtoN״̬ 8bit
u8 dev_etype = 1; //���Ӷ�λװ������ 4bit Ĭ��ΪGPS

////////////////
/*********************msg_slot.c*************************/
u8 msg24_A[256]={0};//�洢��Ϣ24A
u8 msg24_B[256]={0};//�洢��Ϣ24B
u8 msg21_add[512]={0};//�� ����Ϣ21��ӦƵ���ֲ��ұ��ĵ�ַ��ÿһ��Ԫ��ȡֵ��Χ��0~16��
//u16 msequence_poly=0x8805;	//16λPN�����ɶ���ʽ������������������һ�����������λ������д�롣
u16 slot_increacement=0 ; //���ʱ϶�������뷢��������interval_s��ӣ����������ʱ������
//u16 time_o;//GPSǰ��ʱ��洢
u8 txBuf[64]={0}; //����AIS��Ϣ����

u8 sendrandom; //����ʱ϶���ʱ�䣻

//union dds_union dds_word_update[16][5]={0x00000000};  //�������º��Ƶ���ֲ��ұ�

u8 flag_channel=0;		 //�ŵ�ѡ���־λ,��ʼ��Ϊ�ŵ�1
									
/*********************gps.c*************************/									
GPS_INFO GPS; //gps��Ϣ�ṹ��
u8 GPS_RecNum = 0;  //GPS���յ��ֽ���

/*********************exti.c*************************/	
u8 CodeNum[6]; //����

u16 activation_flag=0;  //�����־��Ĭ��Ϊ0
u32 ChipUniqueID_Flash[3];   //оƬ96bit UID

/*********************adc.c*************************/
ais_status setting_flag=off; //�ж��Ƿ���д��״̬��offΪ��д��״̬

//u8 rxbuf1[4] = {0x06,0x9F,0x6B,0xC7}; //temp////////////////////


void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
}

/*********************flash.c*************************/
u32 firstSetTime = 0; //��һ��д���ʱ��
u8 AgencyName[16]={0}; //����������

// Cortex System Control register address
#define SCB_SysCtrl              ((uint32_t)0xE000ED10)
// SLEEPDEEP bit mask
#define SysCtrl_SLEEPDEEP_Set    ((uint32_t)0x00000004)

extern u8 swchflag;

u16 rftime = 0; //ÿ30s��һ��PA

unsigned long myflag;

int main(void)
{
	unsigned int i;
	SYS_CLK_UP();  //����ϵͳʱ��Ϊ48M

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	RCC->APB2ENR|=1<<2;     //��ʹ������IO PORTAʱ��
	RCC->APB2ENR|=1<<0;     //��������ʱ��
	GPIOA->CRL&=0XFFFFFFF0;//PA0���ó�����
	GPIOA->CRL|=0X00000008;
	
	delay_init();		        //ʵ�־�ȷ��ʱ����
	GPIO_Configuration();  //GPIO	
	USART1_Config(); //��д��ӿ�
	USART2_Config(); //GPS����
	// watch dog
	TIM2_Configuration();
	IWDG_Init(4,625);//���Ƶ��Ϊ64,����ֵΪ625,���ʱ��Ϊ1s
	Adc_Init();
	BackupRegisterInit();
	ReadFlashInit(); //��ȡflash����
	
	LED_RED_ON();
	delay_ms(1000);
	LED_RED_OFF();
	
	// si4463��ʼ��
	SPI1IO_Init();			      // IOģ��SPI
	SI4463_IOSET();	
	SeedSet(); 		//���������
	while (1)
 {	
			ProgramSelector();  //���뿪�� 
		 if(setting_flag==off)   //��д��
			{					
				send_on(); //����AIS��Ϣ
			}		
 }
}
