#include "main.h"
#include "math.h"


static u8 t_convert=0;   //��¼�յ���Чλ����Ϣ�Ĵ���

int GPS_RMC_Parse(char *line, GPS_INFO *GPS)
{	
	
	
	char *buf;
	u8 ch, status;
	u16 time_n,delta_time;
	
	if (buf!=NULL)  //($GPRMC)
	{	
		if (status == 'A')  //���������Ч�������
		{	
			t_convert++;
			
			//����  time_o��backupŲ������
			if(GetComma( 7, buf)!=0){
				sog = Get_Double_Number_sog(&buf[GetComma( 7, buf)]);
			}
			
			//����
//			Read_Flash_Cogsel();				
			
//			Write_GPS_Info();
				
			TIM_Cmd(TIM2, DISABLE);
							
			if(setting_flag == off)
			{
				PWR_WakeUpPinCmd(ENABLE);
				PWR_EnterSTANDBYMode();
			}	
		}
	}
	
	return 1;
}
