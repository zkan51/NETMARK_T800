#include "main.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

//BKP_DR2 用于openflag,(原为o_time)

//StartAddr[3] 0x0800B000 --- cogesl

/*

*/

//要写入Flash的数据的首地址--FLASH起始地址
u32 StartAddr[13]={0x08009800,0x0800A000,0x0800A800,0x0800B000,0x0800B800,
									 0x0800C000,0x0800C800,0x0800D000,0x0800D800,0x0800E000,
									 0x0800E800,0x0800F000,0x0800F800};
//要写入Flash的数据的末地址--FLASH结束地址
u32 EndAddr[13] = {0x08009BFF,0x0800A3FF,0x0800ABFF,0x0800B3FF,0x0800BBFF,
									 0x0800C3FF,0x0800CBFF,0x0800D300,0x0800DB00,0x0800E300,
									 0x0800EB00,0x0800F300,0x0800FB00};

u32 FlashAddress=0x00;//Flash的内部地址	 
vu32 NbrOfPage = 0x00; //要擦除的页面数量
volatile FLASH_Status FLASHStatus;
volatile TestStatus MemoryProgramStatus;

/***********************************************************
 * 函数名：WriteflashBoatnum
 * 描述  ：把船名信息从CUP写入Flash
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void WriteflashBoatnum(void) 
{
	u8 i;
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;
	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[0] - StartAddr[0]) / FLASH_PAGE_SIZE;//页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[0] + (FLASH_PAGE_SIZE * NbrOfPage));

	FlashAddress = StartAddr[0];
	for(i=0;i<4;i++)
	{
		FLASHStatus = FLASH_ProgramWord(FlashAddress, ((boatnum[i*4+3]<<24) + (boatnum[i*4+2]<<16) + (boatnum[i*4+1]<<8) + boatnum[i*4]));
		FlashAddress = FlashAddress + 4;
	}
}
/***********************************************************
 * 函数名：ReadflashBoatnum
 * 描述  ：把船名信息从Flash读入CPU
 * 输入  : 无
 * 输出  : 无
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
 * 函数名：WriteflashMMSI
 * 描述  ：把MMSI号从CPU写入Flash
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void WriteflashMMSI(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位

	NbrOfPage = (EndAddr[1] - StartAddr[1]) / FLASH_PAGE_SIZE;//页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[1] + (FLASH_PAGE_SIZE * NbrOfPage)); 

	FlashAddress = StartAddr[1]; 
	FLASHStatus = FLASH_ProgramWord(FlashAddress, MMSI);
}
/***********************************************************
 * 函数名：ReadflashMMSI
 * 描述  ：把MMSI号从Flash读入CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void  ReadflashMMSI(void)
{
	MMSI =*(unsigned long*)StartAddr[1];
}


/***********************************************************
 * 函数名：WriteflashMUID
 * 描述  ：把从ChipUniqueID_Flash写入Flash,用于程序激活
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
//void WriteflashMUID(void)
//{
//	u8 i;
//	FLASHStatus = FLASH_COMPLETE;
//	MemoryProgramStatus = PASSED; 

//	FLASH_Unlock();	//FLASH解锁
//	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位

//	NbrOfPage = (EndAddr[5] - StartAddr[5]) / FLASH_PAGE_SIZE;//页面擦除子程序
//	FLASHStatus = FLASH_ErasePage(StartAddr[5] + (FLASH_PAGE_SIZE * NbrOfPage)); 
//	FlashAddress = StartAddr[5]; 
//	for(i=0;i<=2;i++)
//	{
//		FLASHStatus = FLASH_ProgramWord(FlashAddress,ChipUniqueID_Flash[i]);  //将UID写入flash
//		FlashAddress += 4;
//	}	
//}
/***********************************************************
 * 函数名：ReadflashMUID
 * 描述  ：把ChipUniqueID_Flash从Flash读入CPU
 * 输入  : 无
 * 输出  : 无
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
 * 函数名：WriteflashMActivation
 * 描述  ：把activation_flag从CPU写入Flash,用于程序激活
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void WriteflashMActivation(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 

	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[6] - StartAddr[6]) / FLASH_PAGE_SIZE;//页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[6] + (FLASH_PAGE_SIZE * NbrOfPage));
	FlashAddress = StartAddr[6];
	FLASHStatus = FLASH_ProgramHalfWord(FlashAddress, activation_flag);
}
/***********************************************************
 * 函数名：ReadflashMActivation
 * 描述  ：把activation_flag从Flash读入CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void  ReadflashMActivation(void)
{
	activation_flag = *(u16*)StartAddr[6];
}

/***********************************************************
 * 函数名：WriteflashBoatInfo
 * 描述  ：把activation_flag从CPU写入Flash,位置信息
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void WriteflashBoatInfo(void) 
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[7] - StartAddr[7]) / FLASH_PAGE_SIZE; //页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[7] + (FLASH_PAGE_SIZE * NbrOfPage)); 

	FlashAddress = StartAddr[7]; 

	FLASHStatus = FLASH_ProgramWord(FlashAddress, boatlong);
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, boatwidth);
}

//船舶大小
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


void WriteTuoWangInfo(void) //写拖网偏移信息
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[8] - StartAddr[8]) / FLASH_PAGE_SIZE; //页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[8] + (FLASH_PAGE_SIZE * NbrOfPage));

	FlashAddress = StartAddr[8];

	FLASHStatus = FLASH_ProgramWord(FlashAddress, (int)(offset_len1));
	FlashAddress = FlashAddress + 4;
	FLASHStatus = FLASH_ProgramWord(FlashAddress, (int)(offset_len2));
}

void ReadTuoWangInfo(void) //读拖网偏移信息
{	
	FlashAddress = StartAddr[8]; 
	
	offset_len1 = *(int*)FlashAddress; 
 	FlashAddress += 4;	
	offset_len2 = *(int*)FlashAddress; 
}

/***********************************************************
 * 函数名：ReadflashInit
 * 描述  ：从Flash读取船名、MMSI、密码、激活码、UID、发射时间间隔到CPU
 * 输入  : 无
 * 输出  : 无
 ***********************************************************/
void ReadFlashInit(void) 
{
	u8 i;
	for(i=0;i<16;i++)//读入船名
	{
		boatnum[i] = *(unsigned char*)(StartAddr[0]+i);
	}
	MMSI = *(unsigned long*)StartAddr[1];//读入MMSI
	ReadflashBoatInfo();  	//船舶大小信息
	ReadTuoWangInfo();					//拖网设置信息
	
	ReadFlash_Time();  				//出厂时间
	ReadFlash_AgencyName();//经销商名字
	ReadFlash_AverageParam();	//平均化参数
}

//第一次写码的 时间
void WriteFlash_Time()
{
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[9] - StartAddr[9]) / FLASH_PAGE_SIZE; //页面擦除子程序
	FLASHStatus = FLASH_ErasePage(StartAddr[9] + (FLASH_PAGE_SIZE * NbrOfPage));
	FlashAddress = StartAddr[9];
	
	FLASHStatus = FLASH_ProgramWord(FlashAddress, firstSetTime);
}

void ReadFlash_Time()
{
	firstSetTime = *(u32*)StartAddr[9];
}

//经销商名字
void WriteFlash_AgencyName()
{
	u8 i;
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[11] - StartAddr[11]) / FLASH_PAGE_SIZE; //页面擦除子程序
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


//航速航向平均化点数 存储
void WriteFlash_AveragParam()
{
	u8 i;
	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED; 
	FLASH_Unlock();	//FLASH解锁
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
	NbrOfPage = (EndAddr[5] - StartAddr[5]) / FLASH_PAGE_SIZE; //页面擦除子程序
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

