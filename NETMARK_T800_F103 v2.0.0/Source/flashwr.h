#ifndef __FLASHWR_H
#define	__FLASHWR_H

#include "stm32f10x.h"
#include <stdio.h>

#define FLASH_PAGE_SIZE    ((u16)0x400) //1024  1K 

void WriteflashBoatnum(void); 
void ReadflashBoatnum(void);
void WriteflashMMSI(void);
void ReadflashMMSI(void); 
void WriteflashMActivation(void);
void ReadflashMActivation(void);
void WriteflashBoatInfo(void);
void ReadflashBoatInfo(void);
void WriteTuoWangInfo(void); //Ð´ÍÏÍøÆ«ÒÆÐÅÏ¢
void ReadTuoWangInfo(void); //¶ÁÍÏÍøÆ«ÒÆÐÅÏ¢
void ReadFlashInit(void);
void WriteFlash_Time(void);
void ReadFlash_Time(void);
void WriteFlash_AgencyName(void);
void ReadFlash_AgencyName(void);
void WriteFlash_AveragParam(void);
void ReadFlash_AverageParam(void);

#endif /* __FLASHWR_H */
