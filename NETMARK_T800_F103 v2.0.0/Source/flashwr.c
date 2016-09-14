#include "main.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

//BKP_DR2 ����openflag,(ԭΪo_time)

//StartAddr[3] 0x0800B000 --- cogesl

/*

*/

//Ҫд��Flash�����ݵ��׵�ַ--FLASH��ʼ��ַ
u32 StartAddr[13]={0x08009800,0x0800A000,0x0800A800,0x0800B000,0x0800B800,
									 0x0800C000,0x0800C800,0x0800D000,0x0800D800,0x0800E000,
									 0x0800E800,0x0800F000,0x0800F800};
//Ҫд��Flash�����ݵ�ĩ��ַ--FLASH������ַ
u32 EndAddr[13] = {0x08009BFF,0x0800A3FF,0x0800ABFF,0x0800B3FF,0x0800BBFF,
									 0x0800C3FF,0x0800CBFF,0x0800D300,0x0800DB00,0x0800E300,
									 0x0800EB00,0x0800F300,0x0800FB00};

u32 FlashAddress=0x00;//Flash���ڲ���ַ	 
vu32 NbrOfPage = 0x00; //Ҫ������ҳ������
volatile FLASH_Status FLASHStatus;
volatile TestStatus MemoryProgramStatus;

/***********************************************************
 * ��������WriteflashBoatnum
 * ����  ���Ѵ�����Ϣ��CUPд��Flash
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void WriteflashBoatnum(void) 
{
	u8 i;
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;
	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[0] - StartAddr[0]) / FLASH_PAGE_SIZE;//ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[0] + (FLASH_PAGE_SIZE * NbrOfPage));

	FlashAddress = StartAddr[0];
	for(i=0;i<4;i++)
	{
		FLASHStatus = FLASH_ProgramWord(FlashAddress, ((boatnum[i*4+3]<<24) + (boatnum[i*4+2]<<16) + (boatnum[i*4+1]<<8) + boatnum[i*4]));
		FlashAddress = FlashAddress + 4;
	}
}
/***********************************************************
 * ��������ReadflashBoatnum
 * ����  ���Ѵ�����Ϣ��Flash����CPU
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void  ReadflashBoatnum(void)
{
	u8 i;
	for(i=0;i<16;i++)
	{
		boatnum[i] = *(unsigned char*)(StartAddr[0]+i);
	}
}
/***********************************************************
 * ��������WriteflashMMSI
 * ����  ����MMSI�Ŵ�CPUд��Flash
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void WriteflashMMSI(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ

	NbrOfPage = (EndAddr[1] - StartAddr[1]) / FLASH_PAGE_SIZE;//ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[1] + (FLASH_PAGE_SIZE * NbrOfPage)); 

	FlashAddress = StartAddr[1]; 
	FLASHStatus = FLASH_ProgramWord(FlashAddress, MMSI);
}
/***********************************************************
 * ��������ReadflashMMSI
 * ����  ����MMSI�Ŵ�Flash����CPU
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void  ReadflashMMSI(void)
{
	MMSI =*(unsigned long*)StartAddr[1];
}


/***********************************************************
 * ��������WriteflashMUID
 * ����  ���Ѵ�ChipUniqueID_Flashд��Flash,���ڳ��򼤻�
 * ����  : ��
 * ���  : ��
 ***********************************************************/
//void WriteflashMUID(void)
//{
//	u8 i;
//	FLASHStatus = FLASH_COMPLETE;
//	MemoryProgramStatus = PASSED; 

//	FLASH_Unlock();	//FLASH����
//	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ

//	NbrOfPage = (EndAddr[5] - StartAddr[5]) / FLASH_PAGE_SIZE;//ҳ������ӳ���
//	FLASHStatus = FLASH_ErasePage(StartAddr[5] + (FLASH_PAGE_SIZE * NbrOfPage)); 
//	FlashAddress = StartAddr[5]; 
//	for(i=0;i<=2;i++)
//	{
//		FLASHStatus = FLASH_ProgramWord(FlashAddress,ChipUniqueID_Flash[i]);  //��UIDд��flash
//		FlashAddress += 4;
//	}	
//}
/***********************************************************
 * ��������ReadflashMUID
 * ����  ����ChipUniqueID_Flash��Flash����CPU
 * ����  : ��
 * ���  : ��
 ***********************************************************/
//void  ReadflashMUID(void) 
//{
//	u8 i;
//	FlashAddress = StartAddr[5]; 
//	for(i=0;i<=2;i++)
//	{
//		ChipUniqueID_Flash[i] = *(u32*)FlashAddress; 
//		FlashAddress += 4;
//	}
//	
//}
/***********************************************************
 * ��������WriteflashMActivation
 * ����  ����activation_flag��CPUд��Flash,���ڳ��򼤻�
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void WriteflashMActivation(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[6] - StartAddr[6]) / FLASH_PAGE_SIZE;//ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[6] + (FLASH_PAGE_SIZE * NbrOfPage));
	FlashAddress = StartAddr[6];
	FLASHStatus = FLASH_ProgramHalfWord(FlashAddress, activation_flag);
}
/***********************************************************
 * ��������ReadflashMActivation
 * ����  ����activation_flag��Flash����CPU
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void  ReadflashMActivation(void)
{
	activation_flag = *(u16*)StartAddr[6];
}

/***********************************************************
 * ��������WriteflashBoatInfo
 * ����  ����activation_flag��CPUд��Flash,λ����Ϣ
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void WriteflashBoatInfo(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[7] - StartAddr[7]) / FLASH_PAGE_SIZE; //ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[7] + (FLASH_PAGE_SIZE * NbrOfPage)); 

	FlashAddress = StartAddr[7]; 

	FLASHStatus = FLASH_ProgramWord(FlashAddress, boatlong);
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, boatwidth);
}

//������С
void ReadflashBoatInfo(void)
{
	FlashAddress = StartAddr[7]; 
	boatlong = *(u16*)StartAddr[7];
	FlashAddress += 4;
	boatwidth = *(u8*)FlashAddress;
	boatsize_a = 0;
	boatsize_b = 0;
	boatsize_c = 0;
	boatsize_d = 0;
}


void WriteTuoWangInfo(void) //д����ƫ����Ϣ
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[8] - StartAddr[8]) / FLASH_PAGE_SIZE; //ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[8] + (FLASH_PAGE_SIZE * NbrOfPage));

	FlashAddress = StartAddr[8];

	FLASHStatus = FLASH_ProgramWord(FlashAddress, (int)(offset_len1));
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, (int)(offset_len2));
}

void ReadTuoWangInfo(void) //������ƫ����Ϣ
{	
	FlashAddress = StartAddr[8]; 
	
	offset_len1 = *(int*)FlashAddress; 
 	FlashAddress += 4;	
	offset_len2 = *(int*)FlashAddress; 
}

/***********************************************************
 * ��������ReadflashInit
 * ����  ����Flash��ȡ������MMSI�����롢�����롢UID������ʱ������CPU
 * ����  : ��
 * ���  : ��
 ***********************************************************/
void ReadFlashInit(void) 
{
	u8 i;
	for(i=0;i<16;i++)//���봬��
	{
		boatnum[i] = *(unsigned char*)(StartAddr[0]+i);
	}
	MMSI = *(unsigned long*)StartAddr[1];//����MMSI
	ReadflashBoatInfo();  	//������С��Ϣ
	ReadTuoWangInfo();					//����������Ϣ
	
	ReadFlash_Time();  				//����ʱ��
	ReadFlash_AgencyName();//����������
	ReadFlash_AverageParam();	//ƽ��������
}

//��һ��д��� ʱ��
void WriteFlash_Time()
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[9] - StartAddr[9]) / FLASH_PAGE_SIZE; //ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[9] + (FLASH_PAGE_SIZE * NbrOfPage));
	FlashAddress = StartAddr[9];
	
	FLASHStatus = FLASH_ProgramWord(FlashAddress, firstSetTime);
}

void ReadFlash_Time()
{
	firstSetTime = *(u32*)StartAddr[9];
}

//����������
void WriteFlash_AgencyName()
{
	u8 i;
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[11] - StartAddr[11]) / FLASH_PAGE_SIZE; //ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[11] + (FLASH_PAGE_SIZE * NbrOfPage));
	FlashAddress = StartAddr[11];
	
	for(i=0;i<4;i++)
	{
		FLASHStatus = FLASH_ProgramWord(FlashAddress, ((AgencyName[i*4+3]<<24) + (AgencyName[i*4+2]<<16) + (AgencyName[i*4+1]<<8) + AgencyName[i*4]));
		FlashAddress = FlashAddress + 4;
	}
}

void ReadFlash_AgencyName()
{
	u8 i ;
	for(i=0;i<16;i++)
	{
		AgencyName[i] = *(u8*)(StartAddr[11]+i);
	}
}


//���ٺ���ƽ�������� �洢
void WriteFlash_AveragParam()
{
	u8 i;
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//���־λ
	
	NbrOfPage = (EndAddr[5] - StartAddr[5]) / FLASH_PAGE_SIZE; //ҳ������ӳ���
	FLASHStatus = FLASH_ErasePage(StartAddr[5] + (FLASH_PAGE_SIZE * NbrOfPage));
	FlashAddress = StartAddr[5];
	FLASHStatus = FLASH_ProgramWord(FlashAddress,sog_sample_interval);
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress,cog_sample_len);
}

void ReadFlash_AverageParam()
{
	sog_sample_interval = *(u8*)(StartAddr[5]);
	cog_sample_len = *(u8*)(StartAddr[5]+4);
	if(sog_sample_interval==0x000000ff)
	{
// 		sog_sample_interval = 15;
// 	 cog_sample_len = 60;
// 		WriteFlash_AveragParam();
	}
}

