#include "main.h"
#include "math.h"


static u8 t_convert=0;   //��¼�յ���Чλ����Ϣ�Ĵ���

int GPS_RMC_Parse(char *line, GPS_INFO *GPS)
{	
	
	
	char *buf;
	u8 ch, status;
	u16 time_n,delta_time;
	double tantemp1,tantemp2,tantemp3,tantemp4,tantemp5;
	double delta_jingdu,delta_weidu;
	
	if (buf!=NULL)  //($GPRMC)
	{	
		if (status == 'A')  //���������Ч�������
		{	
			t_convert++;
				
			GPS->latitude = Get_Double_Number(&buf[GetComma( 3, buf)]);
			GPS->longitude = Get_Double_Number(&buf[GetComma( 5, buf)]);				
				
			if(strlen(buf)>12){
				GPS->second = ((buf[7]-'0')*10+(buf[8]-'0'))*3600/2 + ((buf[9]-'0')*10+(buf[10]-'0'))*60/2 + ((buf[11]-'0')*10+(buf[12]-'0'))/2;	//��2��Ϊ��λ
			}else{
				GPS->second = -1;
			}

			GPS->NS = buf[GetComma(4, buf)];
			GPS->EW = buf[GetComma(6, buf)];
				
			tantemp4 = jingdu;  tantemp5 = weidu; //��
			jingdu=GPS->longitude; //���¾�γ����Ϣ
			weidu=GPS->latitude;
			tantemp1 = weidu; tantemp2 = jingdu; //��
			delta_jingdu = tantemp2 - tantemp4;
			delta_weidu = tantemp1 - tantemp5;				
				
			//����  time_o��backupŲ������
			if(GetComma( 7, buf)!=0){
				sog = Get_Double_Number_sog(&buf[GetComma( 7, buf)]);
			}
			
			//����
			Read_Flash_Cogsel();				
			
			Write_GPS_Info();
				
			TIM_Cmd(TIM2, DISABLE);
							
			if(charging_flag == off)
			{
				PWR_WakeUpPinCmd(ENABLE);
				PWR_EnterSTANDBYMode();
			}	
		}
	}
	
	return 1;
}
