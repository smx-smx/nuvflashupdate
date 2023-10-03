/*
 * Copyright © 2010, Nuvoton Technology Corporation. All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 * 
 * Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of Nuvoton Technology Corporation. or the names of 
 * contributors or licensors may be used to endorse or promote products derived 
 * from this software without specific prior written permission.
 * 
 * This software is provided "AS IS," without a warranty of any kind. 
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, 
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.
 * NUVOTON TECHNOLOGY CORPORATION. ("NUVOTON") AND ITS LICENSORS SHALL NOT BE LIABLE
 * FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.  IN NO EVENT WILL
 * SUN OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA,
 * OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR
 * PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF
 * LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */



#ifndef FUCORE_HEADER 
#define FUCORE_HEADER

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                      INCLUDES                                        */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


#include <stdio.h>
#ifndef __LINUX__
#include <conio.h>
#endif
#ifdef WIN32
#include <windows.h>
#else
    #ifndef BOOLEAN
    #define BOOLEAN int
    #define TRUE 1
    #define FALSE 0
    #endif
#endif
#include <string.h>
#include <stdlib.h>
#include "OSUtils.h"
#include "AppDisplay.h"
#include "PCInterface.h"
#include "FlashAccess.h" 




#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                   TYPE DEFINITIONS                                   */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


#define SECTORS_BITMAP_ARRAY_SIZE   128    /* Sectors bit-map size(multiply by 8 to get */
                                           /* the maximum number of sectors             */



/* Possible sector protection types */

typedef enum
{
    SECTOR_NOT_PROTECTED,
    SECTOR_ERASE_WRITE_PROTECTED,                  /* Erase and write are forbidden      */
    SECTOR_WRITE_PROTECTED,                        /* Able to erase but not to write     */
    
    SECTOR_ANY_PROTECTION
} SECTOR_PROTECTION_TYPE;


typedef enum
{
    SHM_MEM_LPC = 0,
    SHM_MEM_FWH,
    SHM_MEM_LPC_IO
} SHM_mem_access_t;


typedef struct SectorDef
{
  int num;
  int size;
} Sector_t;  


typedef struct FLASH_device_commands_t
{
     unsigned char read_device_id;
     unsigned char write_status_enable;
     unsigned char write_enable;
     unsigned char read_status_reg;
     unsigned char write_status_reg;
     unsigned char page_program;
     unsigned char sector_erase;
     unsigned char status_busy_mask;
     unsigned char status_reg_val;
     unsigned char program_unit_size;
     unsigned char page_size;
     unsigned char read_dev_id_type;
} FLASH_device_commands_t;




typedef struct FlashConfigurationData
{
    FLASH_device_commands_t FlashCommands;

    unsigned int     FlashID;
    unsigned int     FlashSize;
    unsigned int     FlashSectorSize;               /* Not realy the sector size. There can be  */
                                                    /* several secotr types that have diffrent  */
                                                    /* size                                     */
    unsigned int     FlashBlockSize;
    unsigned int     FlashPageSize;
    unsigned char    FlashBlockErase;
    unsigned char    FlashSectorErase;

        
    Sector_t         FlashSectors[20];

    int              NumOfSectoresInBlock;
    int              FlashLastBlock;
    int              NumOfSectores;

 
    /* Protected sectors bitmap array */
    
    unsigned char    FlashProtectedSectors[SECTORS_BITMAP_ARRAY_SIZE]; 


    /* Write Protected sectors bitmap array */
     
    unsigned char    FlashWriteProtectedSectors[SECTORS_BITMAP_ARRAY_SIZE];  
                                                       


}FlashConfigurationData;



/* Write Command Buffer (WCB) commands structure */
#ifdef __LINUX__
#pragma pack(1)
typedef struct WCB_struct
{
    unsigned char	Command;		                /* Byte  3                                   */
    union
    {
        FLASH_device_commands_t InitCommands;
        unsigned int    EnterCode;                  /* Bytes 4-7 For Flash Update "Unlock" code  */
        unsigned int    Address;                    /* Bytes 4-7 For Erase and Set Address       */
        unsigned char	Byte;                       /* Byte  4                                   */
        unsigned short	Word;                       /* Bytes 4-5                                 */   
        unsigned int    DWord;                      /* Bytes 4-7                                 */
        struct
        {
            unsigned int DWord1;                   /* Bytes 4-7                                 */
            unsigned int DWord2;                   /* Bytes 8-11                                */
        } EightBytes;
    } Param;
} WCB_struct;
#pragma pack()
#else
#pragma pack(1)
typedef struct WCB_struct
{
    unsigned char	Command;		                /* Byte  3                                   */
    union
    {
        FLASH_device_commands_t InitCommands;
        unsigned long   EnterCode;                  /* Bytes 4-7 For Flash Update "Unlock" code  */
        unsigned long	Address;                    /* Bytes 4-7 For Erase and Set Address       */
        unsigned char	Byte;                       /* Byte  4                                   */
        unsigned short	Word;                       /* Bytes 4-5                                 */   
        unsigned long	DWord;                      /* Bytes 4-7                                 */
        struct
        {
            unsigned long DWord1;                   /* Bytes 4-7                                 */
            unsigned long DWord2;                   /* Bytes 8-11                                */
        } EightBytes;
    } Param;
} WCB_struct;
#pragma pack()
#endif

/* Possible protocol termination options */

typedef enum
{
    WCB_EXIT_NORMAL = 0,
    WCB_EXIT_RESET_EC,
    WCB_EXIT_GOTO_BOOT_BLOCK,
    WCB_EXIT_LAST
} WCB_exit_t;



typedef struct FlashUpdateOptions
{    

    char                    *BinaryImageFilePtr;
    unsigned long           BinaryImageFileSize;
    unsigned long           NumOfBytesToDownload;
    
    WCB_exit_t              ExitType;
    SHM_mem_access_t        SharedMemoryInterface;
    int                     UseProtectionWindow;
    int                     Verbose;
    int                     SkipConfig;
    int                     SharedBIOS;
    int                     NotSharedBIOS;
    int                     NoCompare;              /* Download to flash without comparing first */
    int                     NoVerify;               /* No verify after downloading to flash */
    int                     NoTimeout;
    int                     BaseAddress;
    int                     UseUserBaseAddress;
    int                     VerifyByRead;
    int                     PreFlashUpdate;
    int                     RemoveFlashProtection;
    int                     OverwriteCalibrationValues;  /* Calibration values are taken from the binary file */
    int                     GenerateBSF;
    int                     RunBSF;
    long                    FlashOffset;
        
} FlashUpdateOptions;



/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                FUNCTION DECLERATIONS                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

FU_RESULT IsOffsetDescribesASectorStart(unsigned long p_offset);

FU_RESULT IgnoreProtectedSectors();

FU_RESULT DownloadToFlash();
                      
FU_RESULT ConfigFlashSharedMemory();

FU_RESULT GetFlashID(FlashConfigurationData *p_flashConfig, unsigned int *p_flashID);                  

FU_RESULT SetFlashUpdateOptions(FlashUpdateOptions *p_flashUpdateOptions);

FU_RESULT SetFlashConfiguration(FlashConfigurationData *p_flashConfig);

FU_RESULT StartUpdateFlow(FlashUpdateOptions *p_flashUpdateOptions);

FU_RESULT EndUpdateFlow(int p_normalExit);

FU_RESULT ReadBytesFromFlash(unsigned long p_offset,
                                unsigned char *p_buf,
                                unsigned long p_numOfbytes);

int       IsFlashAreaHasAnyProtection(unsigned long p_offset,
                                          unsigned long p_size);


void      GetSectorInfo(int p_sectorNumber,
                          unsigned long *p_address,
                          unsigned int *p_size);

BOOLEAN IsInternalClock();
BOOLEAN IsCalibrationValuesPreservationNeeded();
FU_RESULT GetCalibrationValuesFromFlash(char* p_binaryImageFilePtr);

#ifdef __cplusplus
}                                                       /* Closing brace for extern "C" */
#endif

#endif


