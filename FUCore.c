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



/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                      INCLUDES                                        */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


#include "FUCore.h"
#include "FUScript.h"

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                      EXTERNS                                         */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

extern unsigned long LPC_Register;
extern int IsLPC_Register_Changed;
extern int NeedToRestoePFE;


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                     DEFINITIONS                                      */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/



/* SuperI/O Registers Indices */

#define SIO_LDN             0x7                 /* Super IO Logical Device Number (LDN) */
#define SIO_SID             0x20                /* Super IO identification              */


/* Shared Memory Logical Device Registers Indices */

#define SIO_ACTIVATE        0x30
#define SIO_BASE_ADDR_MSB   0x60
#define SIO_BASE_ADDR_LSB   0x61


/* Shared Memory Specific Registers Indices */

#define SHM_CFG             0xF0                /* Shared Memory Configuration          */
#define SHM_CFG2            0xF2
#define SHM_VER_BITS        0x07
#define WIN_CFG             0xF1                /* Shared Access Windows Configuration  */
#define SHAW1BA_0           0xF4                /* Shared Access Window 1 Base Address  */
#define SHAW1BA_1           0xF5
#define SHAW1BA_2           0xF6
#define SHAW1BA_3           0xF7
#define SHAW2BA_0           0xF8                /* Shared Access Window 2 Base Address  */
#define SHAW2BA_1           0xF9
#define SHAW2BA_2           0xFA
#define SHAW2BA_3           0xFB

#define SHMPWIN1_BASEL      0x0C
#define SHMPWIN1_BASEH      0x0D
#define SHMPWIN1_SIZEL      0x0E
#define SHMPWIN1_SIZEH      0x0F


/* Shared Memory Registers' bits */

#define WRAM1_IO            0x10
#define WRAM2_IO            0x20

#define SHW_FWH_ID_POS      4
#define SHW_FWH_ID_MASK     0xF0
#define FLASH_ACC_EN        0x4
#define SHWIN_ACC_FWH       0x40
#define BIOS_LPC_EN         0x1
#define BIOS_FWH_EN         0x8


#define FWIN1_SIZE_MASK     0xF
#define FWIN1_SIZE_128_KB   0x7
#define FWIN1_SIZE_256_KB   0x8
#define FWIN1_SIZE_512_KB   0x9
#define FWIN1_SIZE_1_MB     0xA                   
#define FWIN1_SIZE_2_MB     0xB                
#define FWIN1_SIZE_4_MB     0xC                 


/* SuperI/O Values */

#define LDN_SHM             0xF                 /* LDN of Shared Memory logical device  */
#define SID_WPC876xL        0xFC                /* Identity Number of the EC device family */
#define SID_NPCD378H        0x1C                /* Identity Number of the Desktop Super IO device family */


/* Logical Device values */

#define ACTIVATE_EN         0x1


/* Shared Memory values */

#define	SHM_IO_PORT_BASE    0x110   	        /* I/O Base Address for SHM registers   */



/* Memory Map definitions */


/* PC memory */

#define PC_WCB_BASE  		 0xFF800000	        /* 4GB-8MB */
#define PC_WCB_BASE_LPC      0xFFB00000         /* 4GB-3MB */


/* Firmware Update Protocol definitions */

/* Semaphore bits */

#define SHAW2_SEM_EXE           0x1
#define SHAW2_SEM_PCP           0x20
#define SHAW2_SEM_ERR           0x40
#define SHAW2_SEM_RDY           0x80


/* Command buffer size is 16 bytes - maximum data size is 8 bytes */

#define WCB_MAX_DATA             8
#define WCB_SIZE_ERASE_CMD       5              /* 1 command byte, 4 address bytes    */
#define WCB_SIZE_READ_IDS_CMD    1
#define WCB_SIZE_PRE_ENTER_CMD   1
#define WCB_SIZE_ENTER_CMD       5              /* 1 command byte, 4 code bytes       */
#define WCB_SIZE_EXIT_CMD        1
#define WCB_SIZE_ADDRESS_SET_CMD 5
#define WCB_SIZE_BYTE_CMD        2
#define WCB_SIZE_WORD_CMD        (WCB_SIZE_BYTE_CMD + 1)  
#define WCB_SIZE_DWORD_CMD       (WCB_SIZE_WORD_CMD + 2)
#define WCB_SIZE_LONGLONG_CMD    (WCB_SIZE_DWORD_CMD + 4)
#define WCB_SIZE_GENERIC_CMD     8   
#define WCB_SIZE_SET_WINDOW      9              /* 1 command byte, 8 code bytes       */
#define WCB_SIZE_READ_FLASH_CMD  1
#define WCB_SIZE_CALC_CHECKSUM   9


/* Command buffer offsets */

#define OFFSET_SEMAPHORE        0
#define OFFSET_COMMAND          3
#define OFFSET_ADDRESS          4
#define OFFSET_DATA             4               /* Commands have either address or data */
#define OFFSET_DATA_GENERIC     8               /* Generic commands have both

                                                   address and data                     */

/* Flash Header offsets */

#define OFFSET_FRCDIV_DAT1          0x48
#define OFFSET_FRCDIV_DAT2          0x4A
#define FRCDIV_DAT_SIZE             2

                                                   
#define WCB_MAX_READ_BYTES     (16 - OFFSET_DATA)
/* Timeouts definitions*/
#define WCB_TIMEOUT_A          1600000 // timeout for WCB "short" operations
#define WCB_TIMEOUT_B          2000000 // timeout for WCB "long" operations

/* WCB Commands */

#define FLASH_COMMANDS_INIT_OP  0x5A
#define WCB_ENTER_CODE          0xBECDAA55
#define ENTER_OP                0x10
#define PRE_ENTER_OP            0x1E
#define EXIT_OP                 0x20
#define RESET_EC_OP             0x21
#define GOTO_BOOT_BLOCK_OP      0x22
#define ERASE_OP                0x80
#define SECTOR_ERASE_OP         0x82
#define ADDRESS_SET_OP          0xA0
#define PROGRAM_OP              0xB0
#define READ_IDS_OP             0xC0
#define GENERIC_OP              0xD0
#define SET_WINDOW_OP           0xC5
#define CHECKSUM_OP             0xC6
#define READ_OP                 0xD0 // 0xD1-0xDC



#define NUM_OF_FLASH_PROTECTION_WINDOWS     5

typedef enum
{

    FlashProtection_ReadProtected = 0x20,
    FlashProtection_WriteProtected = 0x40,
    FlashProtection_LockProtected = 0x80
   
} FlashProtectionWindowType;

typedef struct FlashProtectedArea
{

    unsigned long offset;                        /* in bytes */
    unsigned long size;                          /* in bytes */
    FlashProtectionWindowType  type;
    
} FlashProtectedArea;


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                             LOCAL VARRIABLE DECLARATION                              */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


static FlashUpdateOptions   *m_flashUpdateOptions           = NULL;

static unsigned short          m_sioIndex;
static unsigned short          m_sioData;
static unsigned short          m_shmBaseAddress;

static WCB_struct	           m_WCB;

static int                     m_printProgress                 = 0;

static int                     m_useSharedBios                 = 1;

static FlashConfigurationData  *m_flashConfig                  = NULL;

static int                     m_memMapSize                    = 0;
static unsigned long           m_pcWcbBase                     = 0;  /* WCB base physical address */
static unsigned long           m_mappedWcbBaseAddress          = 0;  /* WCB base linear address */
#ifdef __LINUX__
static unsigned int            m_flashBase                     = 0;  /* Flash base physical address */ 
#else
static unsigned long           m_flashBase                     = 0;  /* Flash base physical address */ 
#endif
static unsigned long           m_mappedFlashBaseAddress        = 0;  /* Flash base linear address */
static unsigned int            m_lastMapped                    = 0;  /* Physical address of last mapped address */

const char                     PP[4]                           = {'|','/','-','\\'}; /* use for printing progress */


static int                     m_areFlashProtectionWindowsSupported = 0;
static FlashProtectedArea      m_flashProtectionWindows[NUM_OF_FLASH_PROTECTION_WINDOWS];


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                             LOCAL FUNCTION DECLARATION                               */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


static void           alignBinSizeByPageSize();

static unsigned long  shmGetWindow2BaseAddress();

static FU_RESULT      backupRegisters(int p_restoreFlag);

static FU_RESULT      configSharedMemoryInterface();

static int            isSharedBios();

static int            isSectorProtected(int p_sectorNumber, SECTOR_PROTECTION_TYPE p_protectionType);

static void           selectLogicalDevice(unsigned char p_logicalDeviceNumber);

static int            isBlockContainProtectedSectors(int p_blockNumber, SECTOR_PROTECTION_TYPE p_protectionType);

static void           setLogicalDeviceEnable(int p_enable);

static int            locateLogicalDeviceIndex();

//static void           printRegisters();

static void           shmFlashSetAddress(unsigned long p_address);

static FU_RESULT      shmFlashProgram(unsigned char *p_Data, unsigned int p_Size);

static FU_RESULT      shmReadIDs(unsigned int *p_id);

static FU_RESULT      shmFlashExit(WCB_exit_t p_exitType);

static FU_RESULT      shmFlashSectorErase(unsigned long p_address);

static FU_RESULT      shmFlashOpenProtectionWindow(unsigned long p_address, unsigned long p_size);

static FU_RESULT      shmPreFlashEnter();

static FU_RESULT      shmFlashEnter();

static FU_RESULT      shmFlashInit();

static FU_RESULT      shmGetFlashChecksum(unsigned long p_address,
                                             unsigned long p_size,
                                             unsigned short *p_checksum);

static FU_RESULT      shmFlashRead (unsigned char *p_data, unsigned int p_size);

static FU_RESULT      getSHMMemoryInterface(SHM_mem_access_t *p_memoryInterface);

static FU_RESULT      shmConfigMemAccess(SHM_mem_access_t p_memoryAccessType);

static FU_RESULT      shmRunCommand(unsigned short p_commandLength);

static FU_RESULT      shmSetFlashWindow(unsigned long p_baseAddr, unsigned long p_flashSize);

static FU_RESULT      shmSetWindow2BaseAddress(unsigned long p_baseAddr);

static FU_RESULT      programFlash(unsigned long p_address, unsigned long p_size);

static FU_RESULT      compareFlash(unsigned long p_address,
                                     unsigned long p_size,
                                     BOOLEAN *p_same,
                                     int p_forceCompareByRead);

                  
static FU_RESULT      loadFlashProtectionWindows();
static int            isFlashAreaProtected(unsigned long p_offset,
                                              unsigned long p_size,
                                              FlashProtectionWindowType p_protection);
static int            isFlashByteReadProtected(unsigned long p_byteOffset);
//static int            isFlashByteWriteProtected(unsigned long p_byteOffset);
static FU_RESULT      removeFlashProtectionWindows();
static FU_RESULT      areFlashProtectedWindowsSupported();

static int            getBlockAndSectorFromAddress(unsigned long p_address,
                                                               int *p_blockNumber,
                                                               int *p_sectorNumber);


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                             FUNCTION IMPLEMENTATION                                  */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


#if 0

static FU_RESULT TEST_ADD_FAKE_PROTECTION()
{
    FU_RESULT     l_retVal   = FU_RESULT_OK;  
    unsigned int i = 0;




    PrintText(APP_VERBOSE_PRINT, "TEST_ADD_FAKE_PROTECTION\n");

    if (m_areFlashProtectionWindowsSupported)
    {
        PrintText(APP_VERBOSE_PRINT, "TEST SETTING VALUES\n");

        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEL , 0);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEH , 0);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEL , 2);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEH , FlashProtection_ReadProtected);

        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEL + 4, 6);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEH + 4, 0);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEL + 4, 1);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEH + 4, FlashProtection_ReadProtected);

        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEL + 8, 9);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEH + 8, 0);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEL + 8, 2);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEH + 8, FlashProtection_ReadProtected | FlashProtection_WriteProtected);            


        
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEL + 16,15);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEH + 16, 0);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEL + 16, 5);
        PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEH + 16, FlashProtection_WriteProtected);            



      /*  for (i = 0; i < NUM_OF_FLASH_PROTECTION_WINDOWS; ++i)
        {            
            PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEL + (i * 4), i*(i*4));
            PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEH + (i * 4), i);
            PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEL + (i * 4), 2);
            PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEH + (i * 4), FlashProtection_LockProtected | FlashProtection_ReadProtected);
        }*/
           
    }

    return l_retVal;
}

#endif


/*--------------------------------------------------------------------------------------*/
/* Function: IsOffsetDescribesASectorStart                                              */
/*                                                                                      */ 
/* Parameters:   p_address      - The input address                                     */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                                                                                      */
/*               Check whether there is a sector that start in the given offset         */
/*--------------------------------------------------------------------------------------*/

FU_RESULT IsOffsetDescribesASectorStart(unsigned long p_offset)
{
    FU_RESULT l_retVal = FU_RESULT_OK;
    int l_blockNumber = 0;
    int l_sectorNumber = 0;


    
    if (getBlockAndSectorFromAddress(p_offset, &l_blockNumber, &l_sectorNumber) != 0)
    {
        l_retVal = FU_ERR_OFFSET_DONT_DESCRIBE_A_SECTOR_START;
    }

    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: getBlockAndSectorFromAddress                                               */
/*                                                                                      */ 
/* Parameters:   p_address      - The input address                                     */
/*               p_blockNumber  - Will hold the block number that corrisponds to the    */
/*                                address.                                              */
/*               p_sectorNumber - Will hold the sector number that corrisponds to the   */
/*                                address.                                              */
/*                                                                                      */
/* Returns:      0 there is a sector that starts at the given address                   */
/*               otherwise 1                                                            */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                                                                                      */
/*               Find the block and sector number that corresponding to the             */
/*               given address                                                          */
/*--------------------------------------------------------------------------------------*/

static int getBlockAndSectorFromAddress(unsigned long p_address,
                                                    int *p_blockNumber,
                                                    int *p_sectorNumber)
{

    int retVal = 1;
    int sectorNumber = 0;
    unsigned long sectorAddress = 0;
    unsigned int sectorSize = 0;
    
    


    if (p_address < m_flashConfig->FlashSize * 1024)
    {

        (*p_blockNumber) = (int) (p_address / ((long)m_flashConfig->FlashBlockSize * 1024));

        (*p_sectorNumber) = 0;
        
        while(sectorAddress <= p_address)
        {
            GetSectorInfo(sectorNumber, &sectorAddress, &sectorSize);

            if (sectorAddress == p_address)
            {
                (*p_sectorNumber) = sectorNumber;
                retVal = 0;
                break;
            }
            else
            {
                ++sectorNumber;
            }        
        }
    }


    return retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: IsFlashAreaHasAnyProtection                                                */
/*                                                                                      */ 
/* Parameters:   p_offset - The offset of the area we want to check about               */
/*               p_size   - The area size                                               */
/*                                                                                      */
/* Returns:      int                                                                    */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                                                                                      */
/*               Checks whether the Flash has any protection in the given area          */
/*--------------------------------------------------------------------------------------*/

int IsFlashAreaHasAnyProtection(unsigned long p_offset,
                                            unsigned long p_size)
{    
    return isFlashAreaProtected(p_offset,
                                p_size,
                                FlashProtection_WriteProtected |
                                FlashProtection_ReadProtected );
}


/*--------------------------------------------------------------------------------------*/
/* Function: removeFlashProtectionWindows                                               */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: Clears the EC flash protection windows                                 */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                                                                                      */
/*               Clears the EC flash protection windows, only if the register           */
/*               is unlocked                                                            */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT removeFlashProtectionWindows()
{
    FU_RESULT     l_retVal   = FU_RESULT_OK;  
    unsigned int i = 0;




    PrintText(APP_VERBOSE_PRINT, "Removing flash protection windows...\n");

    if (m_areFlashProtectionWindowsSupported)
    {
        for (i = 0; i < NUM_OF_FLASH_PROTECTION_WINDOWS; ++i)
        {
           
            /* Only if the register is unlocked */
            if(!(PcIoReadB(m_shmBaseAddress + SHMPWIN1_SIZEH + (i * 4)) & FlashProtection_LockProtected)) 
            {        
                PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEL + (i * 4), 0);
                PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEH + (i * 4), 0);
                PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEL + (i * 4), 0);
                PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEH + (i * 4), 0);                
            }
            else
            {
                PrintText(APP_VERBOSE_PRINT, "Protection window %d is locked\n", i);
            }
        }
    }

    return l_retVal;
}



/*--------------------------------------------------------------------------------------*/
/* Function: areFlashProtectedWindowsSupported                                          */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: Set m_areFlashProtectionWindowsSupported                               */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                                                                                      */
/*              Checks whether the EC support flash protection windows                  */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT areFlashProtectedWindowsSupported()
{
    FU_RESULT     l_retVal   = FU_RESULT_OK;  



    PcIoWriteB(m_sioIndex, SHM_CFG2);
    m_areFlashProtectionWindowsSupported = (PcIoReadB(m_sioData)) & SHM_VER_BITS;

    
    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: loadFlashProtectionWindows                                                 */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                                                                                      */
/*              Load the flash protection windows from the EC registers into a static   */
/*              array (Only if the EC support flash protection windows)                 */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT loadFlashProtectionWindows()
{    

    FU_RESULT     l_retVal   = FU_RESULT_OK;    
    unsigned char l_lowByte  = 0;
    unsigned char l_highByte = 0;
    unsigned char i          = 0;




    PrintText(APP_VERBOSE_PRINT, "loadFlashProtectionWindows\n");

    if (m_areFlashProtectionWindowsSupported)
    {
        PrintText(APP_VERBOSE_PRINT, "Flash Protection Windows are supported\n");
       
        for (i = 0; i < NUM_OF_FLASH_PROTECTION_WINDOWS; ++i)
        {
                               
            l_lowByte = PcIoReadB(m_shmBaseAddress + SHMPWIN1_BASEL + (i * 4));            
            l_highByte = PcIoReadB(m_shmBaseAddress + SHMPWIN1_BASEH + (i * 4));       
            m_flashProtectionWindows[i].offset = l_lowByte * 1024 + (((l_highByte & 0x0F) << 8) * 1024);
                       
            l_lowByte = PcIoReadB(m_shmBaseAddress + SHMPWIN1_SIZEL + (i * 4));             
            l_highByte = PcIoReadB(m_shmBaseAddress + SHMPWIN1_SIZEH + (i * 4));
            m_flashProtectionWindows[i].size = (1 + l_lowByte) * 1024 + (((l_highByte & 0x0F) << 8) * 1024);

            m_flashProtectionWindows[i].type = l_highByte & 0xE0;

            PrintText(APP_VERBOSE_PRINT,
                      "Protection Window %d: Base 0x%x  Size 0x%x  Type 0x%x\n",
                      i,
                      m_flashProtectionWindows[i].offset,
                      m_flashProtectionWindows[i].size,
                      m_flashProtectionWindows[i].type);                                      
            
        }
    }
    else
    {
        PrintText(APP_VERBOSE_PRINT, "Flash Protection Windows are not supported\n");
    }


    return l_retVal;    
}


static int getFlashUnprotectedArea(unsigned long p_offset,
                                      unsigned long p_size,
                                      unsigned long *p_unprotectedOffset,
                                      unsigned long *p_unprotectedSize,
                                      FlashProtectionWindowType p_protection
                                      )
{
    int l_retVal = 0;
    int i        = 0;
    unsigned long l_start  = p_offset;
    unsigned long l_end    = p_offset + p_size;
    unsigned int  l_changedFlag = 1;




    while(l_changedFlag)
    {
        l_changedFlag = 0;
        for (i = 0; i < NUM_OF_FLASH_PROTECTION_WINDOWS; ++i)
        {
            if ((m_flashProtectionWindows[i].type & p_protection))
            {
                if ((l_start >= m_flashProtectionWindows[i].offset) &&
                    (l_start < m_flashProtectionWindows[i].offset + m_flashProtectionWindows[i].size))
                {
                    l_start = m_flashProtectionWindows[i].offset + m_flashProtectionWindows[i].size;
                    l_changedFlag = 1;                        
                }

                if(0 == l_changedFlag)
                {
                    if (((l_end > m_flashProtectionWindows[i].offset + m_flashProtectionWindows[i].size) &&
                          (l_start < m_flashProtectionWindows[i].offset)) ||
                        ((l_end >= m_flashProtectionWindows[i].offset) &&
                        (l_end < m_flashProtectionWindows[i].offset + m_flashProtectionWindows[i].size)))
                    {
                        l_end = m_flashProtectionWindows[i].offset - 1;
                        l_changedFlag = 1;            
                    }
                }
            }             
        }
    }


    if (l_start < l_end)
    {
        *p_unprotectedOffset = l_start;
        *p_unprotectedSize = l_end - l_start;
        l_retVal = 1;
    }
    else
    {
        l_retVal = 0;
    }
    
    
    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: isFlashAreaLocked                                                          */
/*                                                                                      */ 
/* Parameters:   p_offset - Area offset                                                 */
/*               p_size   - Area size                                                   */
/*               p_protection - protection type                                         */
/*                                                                                      */
/* Returns:      0 - None of the area bytes are locked                                  */
/*               1 - Part of the area bytes are locked                                  */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                                                                                      */
/*              Checks whether the given flash area has locked bytes                    */
/*--------------------------------------------------------------------------------------*/

static int isFlashAreaProtected(unsigned long p_offset,
                                   unsigned long p_size,
                                   FlashProtectionWindowType p_protection)
{
    int l_retVal = 0;
    int i      = 0;
        



    if (m_areFlashProtectionWindowsSupported)
    {
        for (i = 0; i < NUM_OF_FLASH_PROTECTION_WINDOWS; ++i)
        {
            if ((m_flashProtectionWindows[i].type & p_protection) &&
                (
                ((m_flashProtectionWindows[i].offset <= p_offset) &&
                (m_flashProtectionWindows[i].offset + m_flashProtectionWindows[i].size > p_offset)) ||
                ((m_flashProtectionWindows[i].offset >= p_offset) &&
                (m_flashProtectionWindows[i].offset < p_offset + p_size)) ||
                ((m_flashProtectionWindows[i].offset + m_flashProtectionWindows[i].size >= p_offset) &&
                (m_flashProtectionWindows[i].offset + m_flashProtectionWindows[i].size < p_offset + p_size))))
            {
                l_retVal = 1;
                break;
            }
        }
    }

    return l_retVal;        
}



/*--------------------------------------------------------------------------------------*/
/* Function: isFlashByteReadProtected                                                   */
/*                                                                                      */ 
/* Parameters:   p_byteOffset - Get protection information regarding this byte          */
/*                                                                                      */
/* Returns:      0 - The byte is not read protected                                     */
/*               1 - The byte is read protected                                         */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                                                                                      */
/*              Checks whether the given offset is read protected                       */
/*--------------------------------------------------------------------------------------*/

static int isFlashByteReadProtected(unsigned long p_byteOffset)
{
    int l_retVal = 0;
    int i        = 0;
        


    if (m_areFlashProtectionWindowsSupported)
    {
        for (i = 0; i < NUM_OF_FLASH_PROTECTION_WINDOWS; ++i)
        {
            if ((m_flashProtectionWindows[i].type & FlashProtection_ReadProtected) &&
                (p_byteOffset >= m_flashProtectionWindows[i].offset) &&
                (p_byteOffset < m_flashProtectionWindows[i].offset + m_flashProtectionWindows[i].size))
            {
                l_retVal = 1;
                break;
            }
        }
    }

    return l_retVal;        
}



/*--------------------------------------------------------------------------------------*/
/* Function: isFlashByteWriteProtected                                                  */
/*                                                                                      */ 
/* Parameters:   p_byteOffset - Get protection information regarding this byte          */
/*                                                                                      */
/* Returns:      0 - The byte is not write protected                                    */
/*               1 - The byte is write protected                                        */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                                                                                      */
/*              Checks whether the given offset is write protected                      */
/*--------------------------------------------------------------------------------------*/
#if 0
static int isFlashByteWriteProtected(unsigned long p_byteOffset)
{
    int l_retVal = 0;
    int i        = 0;
        


    if (m_areFlashProtectionWindowsSupported)
    {
        for (i = 0; i < NUM_OF_FLASH_PROTECTION_WINDOWS; ++i)
        {
            if ((m_flashProtectionWindows[i].type & FlashProtection_WriteProtected) &&
                (p_byteOffset >= m_flashProtectionWindows[i].offset) &&
                (p_byteOffset < m_flashProtectionWindows[i].offset + m_flashProtectionWindows[i].size))
            {
                l_retVal = 1;
                break;
            }
        }
    }

    return l_retVal;        
}
#endif


/*--------------------------------------------------------------------------------------*/
/* Function: ReadBytesFromFlash                                                         */
/*                                                                                      */ 
/* Parameters:   p_offset     - Read offset                                             */
/*               p_buf        - Buffer to hold the bytes read                           */
/*               p_numOfbytes - Number of bytes to read                                 */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                                                                                      */
/*              Read bytes from the flash into the given buffer                         */
/*--------------------------------------------------------------------------------------*/

FU_RESULT ReadBytesFromFlash(unsigned long p_offset,
                                unsigned char *p_buf,
                                unsigned long p_numOfbytes)
{    
    FU_RESULT     l_retVal           = FU_RESULT_OK;
    unsigned long i                  = 0;
    unsigned long l_mapBaseAddress   = 0;
    unsigned long l_mapTemp          = 0;
    unsigned long l_lastMapped       = 0;
    long          l_counter          = 0;
    int           l_bufIndex         = 0;
    unsigned long l_addr             = 0;
    unsigned long l_lastAddress  = 0;
    unsigned int  l_numOfBytesToRead = 0;



    PrintText(APP_VERBOSE_PRINT, "ReadBytesFromFlash\n");

    if (m_areFlashProtectionWindowsSupported &&
        isFlashAreaProtected(p_offset, p_numOfbytes, FlashProtection_ReadProtected))
    {             
        PrintText(APP_VERBOSE_PRINT,
                  "Warning! There is a read protected area between 0x%x to 0x%x\n",
                  p_offset,
                  p_offset + p_numOfbytes);
    }

    
    
    if (SHM_MEM_LPC_IO == m_flashUpdateOptions->SharedMemoryInterface)
    {
        shmFlashSetAddress(p_offset);
        i = 0;
        for (l_addr = 0; l_addr < p_numOfbytes; l_addr = l_addr + l_numOfBytesToRead)
        {
            if ((p_numOfbytes - l_addr) > WCB_MAX_READ_BYTES)
            {
                l_numOfBytesToRead = WCB_MAX_READ_BYTES;
            }
            else
            {
                l_numOfBytesToRead = p_numOfbytes - l_addr;
            }
                                
            l_retVal = shmFlashRead(p_buf + l_addr, l_numOfBytesToRead);
            if (FU_RESULT_OK != l_retVal)
            {
                return l_retVal;
            }
        }            
    }    
    else    /* Reading the flash through the memory */
    {  
        if(m_flashUpdateOptions->VerifyByRead)
        {
            /* Use WCB commands for any flash read operation  */           
            
            if (m_flashUpdateOptions->Verbose)
            {
                PrintText(APP_DEFAULT_PRINT, " Read (WCB Read) data from address %X to %X\n", p_offset, p_offset + p_numOfbytes);
            }
            
            shmFlashSetAddress(p_offset);
            
            l_lastAddress = p_offset + p_numOfbytes;
            
            for (l_addr = p_offset; (l_addr < l_lastAddress); l_addr = l_addr + l_numOfBytesToRead)
            {
                if ((l_lastAddress - l_addr) > WCB_MAX_READ_BYTES)
                {
                    l_numOfBytesToRead = WCB_MAX_READ_BYTES;
                }
                else
                {
                    l_numOfBytesToRead = l_lastAddress - l_addr;
                }
                
                if (!m_areFlashProtectionWindowsSupported || (!isFlashAreaProtected(l_addr,
                                                                                    l_numOfBytesToRead,
                                                                                    FlashProtection_ReadProtected)))
                {
                    l_retVal = shmFlashRead(&p_buf[l_bufIndex], l_numOfBytesToRead);
                    l_bufIndex += l_numOfBytesToRead;
                    if (FU_RESULT_OK != l_retVal)
                    {
                        return l_retVal;
                    }
                    
                }
                else
                {
                    shmFlashSetAddress(l_addr + l_numOfBytesToRead);
                }
            } 

        }
        else
        {
            /* Use memory read for any flash read operation  */
            if(0 == m_useSharedBios)
            {
                shmSetWindow2BaseAddress(0);
                
                /* Set base address for Shared Flash window */
                    
                l_retVal = shmSetFlashWindow(m_pcWcbBase, m_flashConfig->FlashSize * 1024); 
                if (FU_RESULT_OK != l_retVal)
                {
                    return l_retVal;
                }
            }
            

            l_mapBaseAddress = m_flashBase + p_offset;
            l_mapTemp        = l_mapBaseAddress;
            l_lastMapped     = l_mapBaseAddress;
            if (l_lastMapped == m_flashBase)
            {
                l_mapTemp = m_mappedFlashBaseAddress;
            }
            else
            {
                PcMemMap(&l_mapTemp, m_memMapSize);        
            }
            l_mapBaseAddress = l_mapTemp;

            for (i = 0; i < p_numOfbytes; i++)
            {       
                if (l_counter == m_memMapSize)        
                {
                    l_counter = 0;
                    l_mapTemp = l_lastMapped;

                    if (m_flashBase != l_lastMapped)
                    {               
                        PcMemUnmap(&l_mapTemp, m_memMapSize);
                    }
            
                    l_lastMapped = l_lastMapped + m_memMapSize;
                    l_mapTemp = l_lastMapped;            
                    if (!PcMemMap(&l_mapTemp, m_memMapSize))
                    {
                        PrintText(APP_ERROR_PRINT,
                                  "ERROR: Could not map %d bytes of memory at %lx !\n",
                                  m_memMapSize,
                                  l_lastMapped);
                  
                        return FU_ERR_MEMORY_MAP_FAILED;
                    }

                    l_mapBaseAddress = l_mapTemp - i;
                }

                p_buf[i] = PcMemReadB(l_mapBaseAddress + i);        

                l_counter++;
            }
            
            if(l_lastMapped != m_flashBase && l_lastMapped != m_pcWcbBase)
            {
                /* unmap the last mapped address only if it differs from m_flashBase and m_pcWcbBase*/
                PcMemUnmap(&l_lastMapped, m_memMapSize);
            }

            if(0 == m_useSharedBios)
            {
                shmSetFlashWindow(0, 0);
                shmSetWindow2BaseAddress(m_pcWcbBase);
            }
        }
    }
   

    return l_retVal;
}

/*--------------------------------------------------------------------------------------*/
/* Function: StartUpdateFlow                                                            */
/*                                                                                      */ 
/* Parameters:   p_flashUpdateOptions - Flash update options                            */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: Set m_flashUpdateOptions and m_wcbFlashBaseAddress                     */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Backup the shared memory registers.                                    */
/*               Configure the shared memory interface and map the WBC physical memory  */
/*               This function should be call at the start of the update flow           */
/*--------------------------------------------------------------------------------------*/

FU_RESULT StartUpdateFlow(FlashUpdateOptions *p_flashUpdateOptions)

{
    FU_RESULT l_retVal = FU_RESULT_OK;
    


    l_retVal = SetFlashUpdateOptions(p_flashUpdateOptions);     
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }

    l_retVal = SCR_RunCommands(SCR_BEFORE_SECTION);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }

    
    l_retVal = configSharedMemoryInterface();

    if (FU_RESULT_OK == l_retVal)
    {
        areFlashProtectedWindowsSupported();

#if 0      
        TEST_ADD_FAKE_PROTECTION();
#endif
        
        if (m_areFlashProtectionWindowsSupported &&
            m_flashUpdateOptions->RemoveFlashProtection)
        {
            removeFlashProtectionWindows();
        }
        if(m_areFlashProtectionWindowsSupported)
        {
            loadFlashProtectionWindows();
        }
    }
    
    if (m_flashUpdateOptions->PreFlashUpdate)
    {
        l_retVal = shmPreFlashEnter();                        
        if(FU_RESULT_OK != l_retVal)
        {
            return l_retVal;
        }
    }
    
    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: ConfigFlashSharedMemory                                                    */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: Set m_flashBase, m_memMapSize and m_lastMapped                         */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Set the flash physical memory base and mapping block size              */
/*               and perform initial mapping of physical memory                         */
/*--------------------------------------------------------------------------------------*/

FU_RESULT ConfigFlashSharedMemory()
{  
    FU_RESULT       l_retVal    = FU_RESULT_OK;
    unsigned long   l_flashSize = m_flashConfig->FlashSize * 1024; 



    if (SHM_MEM_LPC_IO == m_flashUpdateOptions->SharedMemoryInterface)
    {
        /* There is nothing to config */
    }
    else
    {
        if (m_useSharedBios == 0)
        {
            m_flashBase = m_pcWcbBase;
        }
        else
        {
            m_flashBase = 0xFFFFFFFF - l_flashSize + 1;
        }

        if (PcMaxMapSize() > 0)
        {
            m_memMapSize = PcMaxMapSize();
        }
        else
        {
            m_memMapSize = l_flashSize;
        }

        /* Mapping physical memory */

        m_mappedFlashBaseAddress = m_flashBase;
                
        if (!PcMemMap(&m_mappedFlashBaseAddress, m_memMapSize))
        {
            PrintText(APP_ERROR_PRINT,
                      "ERROR: Could not map %d bytes of memory at %lx !\n",
                      m_memMapSize,
                      m_mappedFlashBaseAddress);
                  
            return FU_ERR_MEMORY_MAP_FAILED;
        }
        else
        {
            m_lastMapped = m_flashBase;

            PrintText(APP_VERBOSE_PRINT, "Mapped %X to %X\n", m_flashBase, m_mappedFlashBaseAddress);        
        }
    }
    
    return l_retVal;           
}



/*--------------------------------------------------------------------------------------*/
/* Function: EndUpdateFlow                                                              */
/*                                                                                      */ 
/* Parameters:   p_normal - 0 exit the firmaware with the flash update options          */
/*                          exit type value                                             */
/*                          otherwise use nomal exit  .                                 */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Restore the shared memory register.                                    */
/*               Call this function after update process is over                        */
/*--------------------------------------------------------------------------------------*/

FU_RESULT EndUpdateFlow(int p_normalExit)
{
    FU_RESULT l_retVal = FU_RESULT_OK;
  
    /* Indicate Flash Update termination to Firmware */
    
    if (p_normalExit)        
    {
        shmFlashExit(WCB_EXIT_NORMAL);
    }
    else
    {
        shmFlashExit(m_flashUpdateOptions->ExitType);
    }
    
    l_retVal = backupRegisters(1);
    RestoreSMILegacyUSB();
    RestoreGlobalSMI();
    if (NeedToRestoePFE == 1)
    {
        RestorePFEMenlow();    
    }

    l_retVal = SCR_RunCommands(SCR_AFTER_SECTION);
    
    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: alignBinSizeByPageSize                                                     */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               In case of PROG_SIZE == 0xFF in the flash config file we need to align */
/*               the binary image by PAGE_SIZE                                          */
/*--------------------------------------------------------------------------------------*/

static void alignBinSizeByPageSize()
{
    unsigned long i                 = 0;
    unsigned long l_newDownloadSize = 0;
    char          *l_oldBinData     = NULL;



    
    if (0xFF == m_flashConfig->FlashCommands.program_unit_size)
    {

        /* When using burst mode we need to align the binary image to the page size */

        if(m_flashUpdateOptions->NumOfBytesToDownload % m_flashConfig->FlashPageSize)
        {
            l_newDownloadSize = m_flashUpdateOptions->NumOfBytesToDownload + 
                                m_flashConfig->FlashPageSize - 
                               (m_flashUpdateOptions->NumOfBytesToDownload % m_flashConfig->FlashPageSize);

            PrintText(APP_VERBOSE_PRINT,
                      "Aligning binary image size from %d to %d\n",
                      m_flashUpdateOptions->NumOfBytesToDownload,
                      l_newDownloadSize);
            
            l_oldBinData = m_flashUpdateOptions->BinaryImageFilePtr;            
            m_flashUpdateOptions->BinaryImageFilePtr = (char*)(malloc(sizeof(char) * l_newDownloadSize));
                        
            for (i = 0; i < m_flashUpdateOptions->NumOfBytesToDownload; i++)
            {
                m_flashUpdateOptions->BinaryImageFilePtr[i] = l_oldBinData[i];
            }  
            
            for (; i < l_newDownloadSize; i++)
            {
                m_flashUpdateOptions->BinaryImageFilePtr[i] = (char)0xFF;
            }                

            m_flashUpdateOptions->NumOfBytesToDownload = l_newDownloadSize;
        }           
    }
}


/*--------------------------------------------------------------------------------------*/
/* Function: DownloadToFlash                                                            */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Download the given bin file inside m_flashUpdateOptions to the flash   */
/*--------------------------------------------------------------------------------------*/

FU_RESULT DownloadToFlash()
                      
{
    FU_RESULT       l_retVal               = FU_RESULT_OK;
    
    unsigned long   l_address              = 0;            /* Current flash address being programmed */
                                                           /* being programmed                       */
   
    unsigned int    l_flashID              = 0;


    int             l_block                = 0;
    int             l_sector               = 0;
    int             l_lastSector           = 0;
    int             l_noBlocks             = 0;
    int             l_flashLastBlock       = 0;

    unsigned long   l_totalBytesDownloaded = 0;
    int             l_completePrecent      = 0;

    BOOLEAN         l_startDownloadingWithOffset = FALSE;
    BOOLEAN         l_same                       = FALSE;


    // used in case we start to download with an offset bigger than zero
    int             l_addressStartBlock    = 0; 
    int             l_addressSector        = 0;





    if (m_flashUpdateOptions->FlashOffset > 0)
    {
        getBlockAndSectorFromAddress(m_flashUpdateOptions->FlashOffset, &l_addressStartBlock, &l_addressSector);        
        PrintText(APP_VERBOSE_PRINT, "Sector Number: %d   Block number %d\n", l_addressSector, l_addressStartBlock);
        
    }    


    /* Initializing flash data */

    if ((m_flashConfig->FlashSize * 1024) - m_flashUpdateOptions->FlashOffset < m_flashUpdateOptions->BinaryImageFileSize)
    {
        m_flashUpdateOptions->NumOfBytesToDownload = (m_flashConfig->FlashSize * 1024) - m_flashUpdateOptions->FlashOffset;
    }
    else
    {
        m_flashUpdateOptions->NumOfBytesToDownload = m_flashUpdateOptions->BinaryImageFileSize;
    }
    alignBinSizeByPageSize();
    
    
    l_retVal = shmFlashInit();                          /* Define the flash device in use               */
    if(FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    /* @@ Yossi
    if (m_flashUpdateOptions->PreFlashUpdate)
    {
        l_retVal = shmPreFlashEnter();                        
        if(FU_RESULT_OK != l_retVal)
        {
            return l_retVal;
        }
    }
    */
    
    l_retVal = shmFlashEnter();                         /* Indicate Flash Update beginning to Firmware  */
    if(FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
        
    
    l_retVal = shmReadIDs(&l_flashID);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    
    if (l_flashID != m_flashConfig->FlashID)
    {
        shmFlashExit(WCB_EXIT_NORMAL);
        PrintText(APP_ERROR_PRINT,
                  "ERROR: Unexpected flash. Found: %xh, Expected = %xh\n",
                  l_flashID,
                  m_flashConfig->FlashID);
        
        return FU_ERR_UNEXPECTED_FLASH_ID;
    }

           
    m_flashConfig->FlashSectorErase = m_flashConfig->FlashCommands.sector_erase;
    if (m_flashConfig->FlashBlockSize != m_flashConfig->FlashSize)
    {    
        l_flashLastBlock = l_addressStartBlock +
                           ((m_flashUpdateOptions->NumOfBytesToDownload  + (m_flashConfig->FlashBlockSize * 1024) -1) / 
                           (m_flashConfig->FlashBlockSize * 1024));                           /* Round up */
    }
    else
    {
        l_flashLastBlock = 1;
    }
    
    l_noBlocks = (l_flashLastBlock == 1);


    if (m_flashUpdateOptions->FlashOffset > 0)
    {
        if (m_flashConfig->FlashBlockSize != m_flashConfig->FlashSize)
        {
            l_sector = l_addressSector - (l_addressStartBlock * m_flashConfig->NumOfSectoresInBlock);
            l_block  = l_addressStartBlock;            
        }
        else
        {
            l_sector = l_addressSector;
            l_block = 0;
        }
        l_startDownloadingWithOffset = TRUE;
        l_address = m_flashUpdateOptions->FlashOffset;
    }
    else
    {
        l_block = 0;
        l_sector = 0;
    }


    PrintText(APP_VERBOSE_PRINT,
              " m_flashUpdateOptions->NumOfBytesToDownload = %d l_noBlocks = %d, l_flashLastBlock = %d\n",
              m_flashUpdateOptions->NumOfBytesToDownload,
              l_noBlocks,
              l_flashLastBlock);

    
    for (; l_block < l_flashLastBlock; l_block++)
    {       

        if (( (l_startDownloadingWithOffset && l_sector != 0) || l_noBlocks || isBlockContainProtectedSectors(l_block, SECTOR_ANY_PROTECTION)) ||
           (m_areFlashProtectionWindowsSupported && (isFlashAreaProtected(l_block * m_flashConfig->FlashBlockSize * 1024,
                                                                            m_flashConfig->FlashBlockSize * 1024,
                                                                            FlashProtection_WriteProtected))))
        {
           
            /* Loop on all the sectors in the current block */
            
            if ((m_flashConfig->FlashCommands.sector_erase == m_flashConfig->FlashBlockErase) &&
                (!l_noBlocks))
            {
                    /* Select sector erase command */
                    
                    m_flashConfig->FlashCommands.sector_erase = m_flashConfig->FlashSectorErase;
                    l_retVal = shmFlashInit(); 
                    if (FU_RESULT_OK != l_retVal)
                    {
                        return l_retVal;
                    }
            }

            if (l_startDownloadingWithOffset)
            {
                l_startDownloadingWithOffset = FALSE;
                l_sector = l_block * m_flashConfig->NumOfSectoresInBlock + l_sector;
                l_lastSector = l_sector + m_flashConfig->NumOfSectoresInBlock - (l_sector % m_flashConfig->NumOfSectoresInBlock);
            }
            else                
            {
                l_sector = l_block * m_flashConfig->NumOfSectoresInBlock;
                l_lastSector = l_sector + m_flashConfig->NumOfSectoresInBlock;
            }
            
            
            for(; l_sector < l_lastSector ; l_sector++)
            {
                GetSectorInfo(l_sector, &l_address, &(m_flashConfig->FlashSectorSize));
                
                PrintText(APP_VERBOSE_PRINT, "Sector Number: %d\n", l_sector);

                if ((isFlashAreaProtected(l_address,
                                           m_flashConfig->FlashSectorSize,
                                           FlashProtection_WriteProtected))) 
                {
                    PrintText(APP_VERBOSE_PRINT, "Progress: Sector %d - Contains a write protected area\n", l_sector);
                }
                else
                {
                    if (!(isSectorProtected(l_sector, SECTOR_ERASE_WRITE_PROTECTED)))
                    {
                        /* Erase and program all unprotected sectors in this block */
                        
                        if (l_address < m_flashUpdateOptions->NumOfBytesToDownload + m_flashUpdateOptions->FlashOffset)
                        {

                            if (m_flashUpdateOptions->UseProtectionWindow == 1)
                            {  
                                l_retVal = shmFlashOpenProtectionWindow(l_address, m_flashConfig->FlashSectorSize);
                                if (FU_RESULT_OK != l_retVal)
                                {
                                    return l_retVal;
                                }
                            }

                            if (m_flashUpdateOptions->NoCompare)
                            {
                                l_same = FALSE;
                            }
                            else
                            {
                                l_retVal = compareFlash(l_address, m_flashConfig->FlashSectorSize, &l_same, 1);
                            }
                            if (FU_RESULT_OK != l_retVal)
                            {
                                return l_retVal;
                            }

                            if (!l_same || isSectorProtected(l_sector, SECTOR_WRITE_PROTECTED))
                            {
                                l_retVal = shmFlashSectorErase(l_address);
                                if (FU_RESULT_OK != l_retVal)
                                {
                                    return l_retVal;
                                }                                                

                                if (!isSectorProtected(l_sector, SECTOR_WRITE_PROTECTED))                              
                                {
                                    l_retVal =  programFlash(l_address, m_flashConfig->FlashSectorSize);
                                    if (FU_RESULT_OK != l_retVal)
                                    {
                                        return l_retVal;
                                    }
                                }
                            }
                        }
                        else
                        {
                            l_sector = l_lastSector;  /* end programing since this is the end of the data */
                        }

                    } 
                    else
                    {                    
                        PrintText(APP_VERBOSE_PRINT, "Progress: Sector %d - Protected\n", l_sector);                    
                    }
                }
                
                /* Updating progress */
                if(l_completePrecent < 100)
                {                      
                    l_totalBytesDownloaded = l_totalBytesDownloaded + m_flashConfig->FlashSectorSize;

                    l_completePrecent = (int)((((double)l_totalBytesDownloaded) / 
                                               ((double)m_flashUpdateOptions->NumOfBytesToDownload)) * 100);
                    UpdateProgress(l_completePrecent );
                }
            }
        } 
        else
        {
            /* Operate in block mode */
            
            if (m_flashConfig->FlashCommands.sector_erase != m_flashConfig->FlashBlockErase)
            {
                /* Select block erase command */
                
                m_flashConfig->FlashCommands.sector_erase = m_flashConfig->FlashBlockErase;

                l_retVal = shmFlashInit();                        /* update the FW with the new command */
                if (FU_RESULT_OK != l_retVal)
                {
                    return l_retVal;
                }
                
            }
            
            l_address = m_flashConfig->FlashBlockSize * 1024 * l_block;

            if (m_flashUpdateOptions->UseProtectionWindow == 1)
            {                
                l_retVal = shmFlashOpenProtectionWindow(l_address, m_flashConfig->FlashBlockSize * 1024);
                if (FU_RESULT_OK != l_retVal)
                {
                    return l_retVal;
                }
            }

            if (m_flashUpdateOptions->NoCompare)
            {
                l_same = FALSE;
            }
            else
            {
                l_retVal = compareFlash(l_address, m_flashConfig->FlashBlockSize * 1024, &l_same, 1);
                if (FU_RESULT_OK != l_retVal)
                {
                    return l_retVal;
                }
            }
            
            if (!l_same)
            {
                PrintText(APP_VERBOSE_PRINT, "Erasing Block %d\n", l_block);
                
                l_retVal = shmFlashSectorErase(l_address);                
                if (FU_RESULT_OK != l_retVal)
                {
                    return l_retVal;
                }
            
                l_retVal = programFlash(l_address, m_flashConfig->FlashBlockSize * 1024);
                if (FU_RESULT_OK != l_retVal)
                {
                    return l_retVal;
                }
            }             


            /* Updating progress */
            if(l_completePrecent < 100)
            {            
                l_totalBytesDownloaded = l_totalBytesDownloaded + m_flashConfig->FlashBlockSize * 1024;

                l_completePrecent = (int)((((double)l_totalBytesDownloaded) / 
                                           ((double)m_flashUpdateOptions->NumOfBytesToDownload)) * 100);
                UpdateProgress(l_completePrecent );
            }
                        
        }
    }
    
    PrintText(APP_DEFAULT_PRINT, "\n");

    return FU_RESULT_OK;
}


/*--------------------------------------------------------------------------------------*/
/* Function: GetFlashID                                                                 */
/*                                                                                      */ 
/* Parameters:   p_flashConfig - Flash configuration                                    */
/*               p_flashID     - Will hold the flash ID                                 */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: Set m_flashConfig                                                      */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Return the flash ID                                                    */
/*--------------------------------------------------------------------------------------*/

FU_RESULT GetFlashID(FlashConfigurationData *p_flashConfig, unsigned int *p_flashID)                         
                         
{
    FU_RESULT l_retVal = FU_RESULT_OK;    
      



    l_retVal = SetFlashConfiguration(p_flashConfig);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
         
    l_retVal = shmFlashInit();
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }

    l_retVal = shmFlashEnter();
    if(FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    
    l_retVal = shmReadIDs(p_flashID);
    if(FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    
    l_retVal = shmFlashExit(WCB_EXIT_NORMAL);
    if(FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }

    
    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: SetFlashOptions                                                            */
/*                                                                                      */ 
/* Parameters:   p_flashUpdateOptions - Flash update options                            */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: Set m_flashUpdateOptions                                               */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Set the current flash options to p_flashUpdateOptions                  */
/*--------------------------------------------------------------------------------------*/

FU_RESULT SetFlashUpdateOptions(FlashUpdateOptions *p_flashUpdateOptions)
{

    m_flashUpdateOptions = p_flashUpdateOptions;


    return FU_RESULT_OK;
}


/*--------------------------------------------------------------------------------------*/
/* Function: SetFlashConfiguration                                                      */
/*                                                                                      */ 
/* Parameters:   p_flashConfig - Flash configuration                                    */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: Set m_flashConfig                                                      */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Set the current flash configuration to p_flashConfig                   */
/*--------------------------------------------------------------------------------------*/

FU_RESULT SetFlashConfiguration(FlashConfigurationData *p_flashConfig)
{

    m_flashConfig = p_flashConfig;
    

    return FU_RESULT_OK;

}


FU_RESULT IgnoreProtectedSectors()
{
    int i = 0;


    if (NULL != m_flashConfig)
    {
        for(i = 0; i < SECTORS_BITMAP_ARRAY_SIZE; i++)
        {
            m_flashConfig->FlashProtectedSectors[i] = 0;
        }
    }    

    return FU_RESULT_OK;

}


/*--------------------------------------------------------------------------------------*/
/* Function: isSharedBios                                                               */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      1 when shared BIOS is used                                             */ 
/*               0 otherwise                                                            */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Check the SHM_CFG register to see if shared BIOS is used               */
/*--------------------------------------------------------------------------------------*/

static int isSharedBios()
{    
    
    unsigned char l_useBios = 0;



    
    /* Select shared window 1 for either flash or RAM access */
    
    PcIoWriteB(m_sioIndex, SHM_CFG);

   
    l_useBios = (PcIoReadB(m_sioData) & BIOS_FWH_EN) | (PcIoReadB(m_sioData) & BIOS_LPC_EN);

    if(l_useBios > 0)
    {
        PrintText(APP_VERBOSE_PRINT, "Using shared BIOS\n");
    }
    else
    {
        PrintText(APP_VERBOSE_PRINT, "Not Using Shared BIOS\n");
    }
    
    
    if (l_useBios > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }

        
}


/*--------------------------------------------------------------------------------------*/
/* Function: backupRegisters                                                            */
/*                                                                                      */ 
/* Parameters:   p_restoreFlag - 1: Restore the registers                               */
/*                               0: Backup registers                                    */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Backup/Restore the shared memory registers (window1 + window2)         */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT backupRegisters(int p_restoreFlag)
{
    
    FU_RESULT            l_retVal         = FU_RESULT_OK;
    unsigned long        l_LPCAddress     = 0;
    static unsigned char l_winCFG         = 0;
    static unsigned char l_shawlba_2      = 0;
    static unsigned char l_shawlba_3      = 0;
    static unsigned char l_shaw2ba_0      = 0;
    static unsigned char l_shaw2ba_1      = 0;
    static unsigned char l_shaw2ba_2      = 0;
    static unsigned char l_shaw2ba_3      = 0;    
    static int           l_registersRead  = 0;
    static unsigned char l_shmpwin_basel[NUM_OF_FLASH_PROTECTION_WINDOWS];
    static unsigned char l_shmpwin_baseh[NUM_OF_FLASH_PROTECTION_WINDOWS];
    static unsigned char l_shmpwin_sizel[NUM_OF_FLASH_PROTECTION_WINDOWS];
    static unsigned char l_shmpwin_sizeh[NUM_OF_FLASH_PROTECTION_WINDOWS];
    unsigned int i = 0;
    

  

    if (1 == p_restoreFlag)
    {
        if (1 == l_registersRead)      /* Verify we backup the registers beofre restart */
        {

           
           if(m_areFlashProtectionWindowsSupported && m_flashUpdateOptions->RemoveFlashProtection)
           {
               for (i = 0; i < NUM_OF_FLASH_PROTECTION_WINDOWS; ++i)
               {                                      
                   PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEL + (i * 4), l_shmpwin_basel[i]);
                   PcIoWriteB(m_shmBaseAddress + SHMPWIN1_BASEH + (i * 4), l_shmpwin_baseh[i]);
                   PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEL + (i * 4), l_shmpwin_sizel[i]);
                   PcIoWriteB(m_shmBaseAddress + SHMPWIN1_SIZEH + (i * 4), l_shmpwin_sizeh[i]);
               }
           }


            /* Disable Flash Access */
            
            PcIoWriteB(m_sioIndex, SHM_CFG);
            PcIoWriteB(m_sioData, (unsigned char) (PcIoReadB(m_sioData) & ~FLASH_ACC_EN));

            PcIoWriteB (m_sioIndex, SHAW1BA_2);
            PcIoWriteB(m_sioData, l_shawlba_2);
        
            PcIoWriteB (m_sioIndex, SHAW1BA_3);          
            PcIoWriteB(m_sioData, l_shawlba_3);

            PcIoWriteB (m_sioIndex,  WIN_CFG);          
            PcIoWriteB(m_sioData, l_winCFG); 

            PcIoWriteB(m_sioIndex, SHAW2BA_0);
            PcIoWriteB(m_sioData, l_shaw2ba_0); 

            PcIoWriteB(m_sioIndex, SHAW2BA_1);
            PcIoWriteB(m_sioData, l_shaw2ba_1); 

            PcIoWriteB(m_sioIndex, SHAW2BA_2);
            PcIoWriteB(m_sioData, l_shaw2ba_2); 

            PcIoWriteB(m_sioIndex, SHAW2BA_3);
            PcIoWriteB(m_sioData, l_shaw2ba_3);                   


            if (1 == IsLPC_Register_Changed)
            {
                /*-----------------------------------------------------------------------------------*/
                /* Restore the ICH LPC register                                                      */
                /*-----------------------------------------------------------------------------------*/
                
                l_LPCAddress = (ReadPCI(0x00, 0x1F, 0x00, 0xF0) & 0xFFFFFFFE) + 0x3410;
                PrintText(APP_VERBOSE_PRINT, "Physical address of RCRB General Control and Status is %lx\n", l_LPCAddress);
                
                if (!PcMemMap(&l_LPCAddress, 0x4))           
                {
                    PrintText(APP_ERROR_PRINT,
                              "ERROR: Could not map %d bytes of memory at %lx for PCI configuration!\n",
                              16,
                              l_LPCAddress);
                  
                    return FU_ERR_MEMORY_MAP_FAILED;
                }
                else
                {
                    PrintText(APP_VERBOSE_PRINT, "LPCAddress mapped to %lx\n", l_LPCAddress);                    
                }
                
                PcMemWriteD(l_LPCAddress, LPC_Register);
                
            }

        }
    }
    else
    {     
      
        PcIoWriteB (m_sioIndex, SHAW1BA_2);
        l_shawlba_2 = PcIoReadB(m_sioData);
        
        PcIoWriteB (m_sioIndex, SHAW1BA_3);          
        l_shawlba_3 = PcIoReadB(m_sioData);

        PcIoWriteB (m_sioIndex,  WIN_CFG);          
        l_winCFG = PcIoReadB(m_sioData);

        PcIoWriteB(m_sioIndex, SHAW2BA_0);
        l_shaw2ba_0 = PcIoReadB(m_sioData);

        PcIoWriteB(m_sioIndex, SHAW2BA_1);
        l_shaw2ba_1 = PcIoReadB(m_sioData);

        PcIoWriteB(m_sioIndex, SHAW2BA_2);
        l_shaw2ba_2 = PcIoReadB(m_sioData);

        PcIoWriteB(m_sioIndex, SHAW2BA_3);
        l_shaw2ba_3 = PcIoReadB(m_sioData);


        if(m_areFlashProtectionWindowsSupported && m_flashUpdateOptions->RemoveFlashProtection)
        {
            for (i = 0; i < NUM_OF_FLASH_PROTECTION_WINDOWS; ++i)
            {                                     
                l_shmpwin_basel[i] = PcIoReadB(m_shmBaseAddress + SHMPWIN1_BASEL + (i * 4));              
                l_shmpwin_baseh[i] = PcIoReadB(m_shmBaseAddress + SHMPWIN1_BASEH + (i * 4));         
                l_shmpwin_sizel[i] = PcIoReadB(m_shmBaseAddress + SHMPWIN1_SIZEL + (i * 4));
                l_shmpwin_sizeh[i] = PcIoReadB(m_shmBaseAddress + SHMPWIN1_SIZEH + (i * 4));
            }
        }

        
        l_registersRead = 1;
        
    }

    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: isSectorProtected                                                          */
/*                                                                                      */ 
/* Parameters:   p_sectorNumber   - Sector number                                       */
/*               p_protectionType - Sector protection type                              */
/*                                                                                      */
/* Returns:      1  - This sector is protected                                          */
/*               0  - Sector is not protected                                           */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Check if the sector is protected                                       */
/*--------------------------------------------------------------------------------------*/

static int isSectorProtected(int p_sectorNumber, SECTOR_PROTECTION_TYPE p_protectionType)
{
    int l_index = (p_sectorNumber / 8);
    int l_bitLocation = (p_sectorNumber & 0x7);



    switch (p_protectionType)
    {
    case SECTOR_NOT_PROTECTED:
        return 0;
        break;
        
    case SECTOR_ANY_PROTECTION:
        return (((m_flashConfig->FlashProtectedSectors[l_index] & (1 << l_bitLocation)) != 0) || 
               ((m_flashConfig->FlashWriteProtectedSectors[l_index] & (1 << l_bitLocation)) != 0));
        break;
           
    case SECTOR_ERASE_WRITE_PROTECTED:
        return((m_flashConfig->FlashProtectedSectors[l_index] & (1 << l_bitLocation)) != 0);
        break;
        
    case SECTOR_WRITE_PROTECTED:
        return((m_flashConfig->FlashWriteProtectedSectors[l_index] & (1 << l_bitLocation)) != 0);
        break;
        
    default:
        break;
    }

    return -1;                                      /* Error detecting if sector protected */
}


/*--------------------------------------------------------------------------------------*/
/* Function: isBlockContainProtectedSectors                                             */
/*                                                                                      */ 
/* Parameters:   p_blockNumer     - Block number                                        */
/*               p_protectionType - Sector protection type                              */
/*                                                                                      */
/* Returns:      1 - There are protected sectors in the block                           */
/*               0 - No protected sectors in this block                                 */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Check if there are any sectors with the given protected type           */
/*                 in the block                                                         */
/*--------------------------------------------------------------------------------------*/

static int isBlockContainProtectedSectors(int p_blockNumber,  SECTOR_PROTECTION_TYPE p_protectionType)
{
    int             l_protectedFlag = 0;
    int             l_startSector   = 0;
    int             l_endSector     = 0;
    int             l_currentSector = 0;
        


    
    l_startSector = p_blockNumber * m_flashConfig->NumOfSectoresInBlock;
    l_endSector  = l_startSector + m_flashConfig->NumOfSectoresInBlock;

    for (l_currentSector = l_startSector; l_currentSector < l_endSector && !l_protectedFlag; l_currentSector++)
    {
        l_protectedFlag = isSectorProtected(l_currentSector, p_protectionType);
    }


    if (l_protectedFlag)
    {
        PrintText(APP_VERBOSE_PRINT, "Block %d contain sectors with protection type %d\n", p_blockNumber, p_protectionType);
    }
    else
    {
        PrintText(APP_VERBOSE_PRINT, "Block %d do not contain sectors with protection type %d\n", p_blockNumber, p_protectionType);
    }
    
    return l_protectedFlag;
}



/*--------------------------------------------------------------------------------------*/
/* Function: GetSectorInfo                                                              */
/*                                                                                      */ 
/* Parameters:   p_sectorNumber - Sector number                                         */
/*               p_address      - Will hold the sector address                          */
/*               p_size         - Will hold the sector size                             */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Finds the sector address and size                                      */
/*--------------------------------------------------------------------------------------*/

void GetSectorInfo(int p_sectorNumber,
                     unsigned long *p_address,
                     unsigned int *p_size)
{

    int i = 0;
    int n = 0;




    *p_address = 0;
    
    while (m_flashConfig->FlashSectors[i].num)
    {
        if (p_sectorNumber <= (n + m_flashConfig->FlashSectors[i].num))
        {
            *p_size = m_flashConfig->FlashSectors[i].size * 1024;
            *p_address = *p_address + ((p_sectorNumber - n) * *p_size);
            
            break;
        }
        
        n = n + m_flashConfig->FlashSectors[i].num;
        
        *p_address = *p_address + 
                    m_flashConfig->FlashSectors[i].num * 
                    m_flashConfig->FlashSectors[i].size * 1024;
        i++;
    }
}



/*--------------------------------------------------------------------------------------*/
/* Function: selectLogicalDevice                                                        */
/*                                                                                      */ 
/* Parameters:   p_logicalDeviceNumber - Logical Device Number of the requested         */
/*                                       SIO logcal device                              */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */
/* Side effects: The logical device to be addressed by all LDN functions remains the    */
/*               same until the next call to this function.                             */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Select a specified Logical Device to be the currently addressed        */
/*               device                                                                 */
/*--------------------------------------------------------------------------------------*/

static void selectLogicalDevice(unsigned char p_logicalDeviceNumber)
{
    
    PcIoWriteB(m_sioIndex, SIO_LDN);                        /* Logical device number register   */
    PcIoWriteB(m_sioData,  p_logicalDeviceNumber);		    /* Select LDN                       */      

    PrintText(APP_VERBOSE_PRINT,
              "SelectLogicalDevice(Device = SIO, LogicalDeviceNumber = %02Xh)\n",
              p_logicalDeviceNumber);   
    
}


/*--------------------------------------------------------------------------------------*/
/* Function: setLogicalDeviceEnable                                                     */
/*                                                                                      */ 
/* Parameters:   p_enable - 1: Enable the device                                        */
/*                          0: Disable the device                                       */    
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Sets the enable state of the currently selected Logical Device         */
/*--------------------------------------------------------------------------------------*/

static void setLogicalDeviceEnable(int p_enable)
{	

    /* Select Activate register */
    
    PcIoWriteB(m_sioIndex, SIO_ACTIVATE);			
    
    if (p_enable)
    {
        PcIoWriteB(m_sioData, (unsigned char)(PcIoReadB(m_sioData) | ACTIVATE_EN));
    }
    else
    {
        PcIoWriteB(m_sioData, (unsigned char)(PcIoReadB(m_sioData) & ~ACTIVATE_EN));
    }

    PrintText(APP_VERBOSE_PRINT,
              "LDNEnable(Device = SIO, En = %s)\n",
              p_enable?"True":"False");    
}


/*--------------------------------------------------------------------------------------*/
/* Function: locateLogicalDeviceIndex                                                   */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      1 = Expected SIO device was located                                    */
/*               otherwise SIO device not found at any of the possible locations        */
/*                                                                                      */ 
/* Side effects: Sets the global variables m_sioIndex and m_sioData to the              */
/*               correct values                                                         */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Find the SIO configuration index-data register                         */
/*               pair setting in the current system                                     */ 
/*--------------------------------------------------------------------------------------*/

static int locateLogicalDeviceIndex()
{
    int           l_sioFound  = 0;
    int           l_passNum   = 1;
    unsigned char l_sioID     = 0;
    int           l_maxPasses = 3;



    while ((!l_sioFound) && (l_passNum < l_maxPasses + 1))
    {
        switch (l_passNum)
        {

        case 1:
            if (m_flashUpdateOptions->UseUserBaseAddress)
            {
                m_sioIndex = (unsigned short) m_flashUpdateOptions->BaseAddress;
                l_passNum = l_maxPasses + 1;
            }
            else
            {
                m_sioIndex = 0x164e;
            }
            break;
        case 2:
            m_sioIndex = 0x2e;
            break;
        case 3:
            m_sioIndex = 0x4e;
            break;
        default:
            
            /* Programmable Configuration Address mode - add values below if selected */
            
            break;
        }
                
        PrintText(APP_VERBOSE_PRINT, "Looking for Nuvoton EC Device at address %X\n", m_sioIndex);
                
        m_sioData = m_sioIndex + 1;
        PcIoWriteB(m_sioIndex, SIO_SID);                        /* Read SIO Device ID Register */
        l_sioID = PcIoReadB(m_sioData);
 
        if (l_sioID == SID_WPC876xL || l_sioID == SID_NPCD378H)
        {
            l_sioFound = 1;
        }
        else
        {
            l_passNum++;
        }
        
    }
    
    return l_sioFound;
} 



/*--------------------------------------------------------------------------------------*/
/* Function: shmGetWindow2BaseAddress                                                             */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      None                                                                   */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               retrive PC WCB base address according to the configuration             */
/*               m_flashUpdateOptions->SharedMemoryInterface must be set before         */
/*                 calling this function                                                */              
/*--------------------------------------------------------------------------------------*/

static unsigned long shmGetWindow2BaseAddress()
{
    unsigned long l_wcbAddress = 0;
    unsigned char l_charRead   = 0;
     


    
    PrintText(APP_VERBOSE_PRINT, "shmGetWindow2BaseAddress()\n");
  
    PcIoWriteB (m_sioIndex, SHAW2BA_0);                 /* read address bits 0-7    */
    l_charRead = PcIoReadB(m_sioData);
    l_wcbAddress = l_charRead ;
        

    PcIoWriteB (m_sioIndex, SHAW2BA_1);                 /* read address bits 8-15   */
    l_charRead = PcIoReadB(m_sioData);
    l_wcbAddress = l_wcbAddress + (l_charRead << 8);


    PcIoWriteB (m_sioIndex, SHAW2BA_2);                 /* read address bits 16-23  */
    l_charRead = PcIoReadB(m_sioData);
    l_wcbAddress = l_wcbAddress + (l_charRead  << 16);
  
    

    PcIoWriteB (m_sioIndex, SHAW2BA_3);          
    l_charRead = PcIoReadB(m_sioData);    

    if (m_flashUpdateOptions->SharedMemoryInterface == SHM_MEM_FWH)
    {
        l_charRead = (l_charRead | 0xF0);

    }
    l_wcbAddress = l_wcbAddress + (l_charRead << 24);     


    return l_wcbAddress;
    
}


/*--------------------------------------------------------------------------------------*/
/* Function: printRegisters                                                             */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Print the flash window1 + window2 registers                            */
/*--------------------------------------------------------------------------------------*/
#if 0
static void printRegisters()
{

    int i = 0;

    PrintText(APP_DEFAULT_PRINT, "\n");

    PcIoWriteB(m_sioIndex, 7);
    PcIoWriteB(m_sioData, 0xF);
    
    for (i = 0; i<16; i++)
    {
        PcIoWriteB(m_sioIndex, (unsigned char)(0xF0 + i));
        PrintText(APP_DEFAULT_PRINT, " %x ", PcIoReadB(m_sioData));
    }

    PrintText(APP_DEFAULT_PRINT, "\n");
}  
#endif


/*--------------------------------------------------------------------------------------*/
/* Function: shmReadIDs                                                                 */
/*                                                                                      */ 
/* Parameters:   p_id       - Will hold the flash ID                                    */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Read the flash ID                                                      */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmReadIDs(unsigned int *p_id)
{
    FU_RESULT l_retVal = FU_RESULT_OK;


    
   
    PrintText(APP_VERBOSE_PRINT, "shmFlashReadIDs\n");
    
    /* Setup the WCB for Read ID */
    
    m_WCB.Command = READ_IDS_OP;    
    l_retVal = shmRunCommand(WCB_SIZE_READ_IDS_CMD);     /* Execute the Command in WCB */
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    
    *p_id = (int)Pc2FlashReadW(m_mappedWcbBaseAddress + OFFSET_DATA);

    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: shmFlashRead                                                               */
/*                                                                                      */ 
/* Parameters:   p_Data - Will hold the read bytes                                      */
/*               p_Size - Number of bytes of the Data to read                           */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Uses the WCB protocol to request reading of the specified number       */
/*               of bytes of the flash device.                                          */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmFlashRead (unsigned char *p_data, unsigned int p_size)
{

    FU_RESULT    l_retVal = FU_RESULT_OK;
    unsigned int i        = 0;



    /* Setup the WCB for Read ID */
        
    m_WCB.Command = READ_OP + p_size;

        
    l_retVal = shmRunCommand(WCB_SIZE_READ_FLASH_CMD);    /* Execute the Command in WCB */
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    

    for (i = 0; i < p_size; ++i)
    {
        p_data[i] = (char)Pc2FlashReadB(m_mappedWcbBaseAddress + OFFSET_DATA + i);
    }
    
    return l_retVal;
}
                              

/*--------------------------------------------------------------------------------------*/
/* Function: shmFlashProgram                                                            */
/*                                                                                      */ 
/* Parameters:   p_Data - Pointer to the lsb byte of the data to be programmed          */
/*               p_Size - Number of bytes of the Data to be programmed                  */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Uses the WCB protocol to request programming of the specified number   */
/*               of bytes in the flash device with the provided data.                   */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmFlashProgram(unsigned char *p_data, unsigned int p_size)
{
    FU_RESULT      l_retVal      = FU_RESULT_OK;
    unsigned short i             = 0;
    unsigned short l_length      = 0;
#ifdef __LINUX__
    unsigned int *l_wcbPointer  = NULL;
    unsigned int *l_dataPointer = NULL;
#else
    unsigned long *l_wcbPointer  = NULL;
    unsigned long *l_dataPointer = NULL;
#endif



    if ((p_size < 1) || (p_size > 8))
    {
        return FU_ERR_TO_MUCH_BYTES_TO_PROGRAM;
    }

    /* Setup the WCB for Program */

    m_WCB.Command	 = PROGRAM_OP | ((unsigned char)p_size & 0xF);

    if (p_size == sizeof(unsigned char))
    {
        m_WCB.Param.Byte = *p_data;
        l_length = WCB_SIZE_BYTE_CMD;
    }
    else if (p_size == sizeof(unsigned short))
    {
        m_WCB.Param.Word = *(unsigned short *)p_data;
        l_length = WCB_SIZE_WORD_CMD;
    }

#ifdef __LINUX__
    else if (p_size == sizeof(unsigned int))
    {
        m_WCB.Param.DWord = *(unsigned int *)p_data;
        l_length = WCB_SIZE_DWORD_CMD;
    } 
    else if (p_size == sizeof(unsigned int) * 2)
    {
        l_wcbPointer = (unsigned int *) &m_WCB.Param.EightBytes.DWord1;
        l_dataPointer = (unsigned int *) p_data;
        l_length = WCB_SIZE_LONGLONG_CMD;

        for (i = 0; i < 2; i ++)                             /* Copy two dwords            */
        {
            *l_wcbPointer = *l_dataPointer;
            l_wcbPointer++;
            l_dataPointer++;
        }
    }
#else    
    else if (p_size == sizeof(unsigned long))
    {
        m_WCB.Param.DWord = *(unsigned long *)p_data;
        l_length = WCB_SIZE_DWORD_CMD;
    } 
    else if (p_size == sizeof(unsigned long) * 2)
    {
        l_wcbPointer = (unsigned long *) &m_WCB.Param.EightBytes.DWord1;
        l_dataPointer = (unsigned long *) p_data;
        l_length = WCB_SIZE_LONGLONG_CMD;

        for (i = 0; i < 2; i ++)                             /* Copy two dwords            */
        {
            *l_wcbPointer = *l_dataPointer;
            l_wcbPointer++;
            l_dataPointer++;
        }
    }
#endif    

    l_retVal = shmRunCommand(l_length);                     /* Execute the command in WCB */

    return l_retVal;
    
}


/*--------------------------------------------------------------------------------------*/
/* Function: shmFlashSetAddress                                                         */
/*                                                                                      */ 
/* Parameters:   p_address - Address of the data to be programmed                       */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Uses the WCB protocol to set the flash address from which flash will   */
/*               be programmed                                                          */
/*--------------------------------------------------------------------------------------*/

static void shmFlashSetAddress(unsigned long p_address)
{

    /* Setup the WCB for Set Address */
    
    m_WCB.Command            = ADDRESS_SET_OP;      /* Byte Program                 */
    m_WCB.Param.Address      = p_address;           /* Address to program           */

    shmRunCommand(WCB_SIZE_ADDRESS_SET_CMD);        /* Execute the Command in WCB   */
}


/*--------------------------------------------------------------------------------------*/
/* Function: shmFlashExit                                                               */
/*                                                                                      */ 
/* Parameters:   p_exitType - Type of termination to indicate through WCB protocol      */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Uses the WCB protocol to indicate the end of Flash Update procedure    */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmFlashExit(WCB_exit_t p_exitType)
{

    PrintText(APP_VERBOSE_PRINT, "shmFlashExit(exit_type = %Xh)\n", p_exitType);

    /* Setup the WCB for Exit */
    
    switch (p_exitType)
    {
        
    case WCB_EXIT_NORMAL:
        m_WCB.Command                       = EXIT_OP;
        break;
        
    case WCB_EXIT_GOTO_BOOT_BLOCK:
        m_WCB.Command                       = GOTO_BOOT_BLOCK_OP;
        break;
        
    case WCB_EXIT_RESET_EC:
        m_WCB.Command                       = RESET_EC_OP;
        break;
    
    default:
	break;
        
    }
    
    return shmRunCommand(WCB_SIZE_EXIT_CMD);               /* Execute the Command in WCB   */
    
}


/*--------------------------------------------------------------------------------------*/
/* Function: shmFlashSectorErase                                                        */
/*                                                                                      */ 
/* Parameters:   p_address - Address of a byte located in the sector which              */
/*                           is to be erased                                            */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Uses the WCB protocol to request erase of the sector in which          */
/*               Address is located                                                     */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmFlashSectorErase(unsigned long p_address)
{
    FU_RESULT l_retVal = FU_RESULT_OK;



    
    PrintText(APP_VERBOSE_PRINT, "%cProgress: Address [%X] E", 0xD, p_address);
    PrintText(APP_VERBOSE_PRINT, " FlashSectorErase(Address = %Xh)\n", p_address);
           
    /* Setup the WCB for Sector Erase */
    
    m_WCB.Command               = ERASE_OP;                 /* Erase Command             */
    m_WCB.Param.Address         = p_address;


    /* Address which resides in the sector to be erased */

    l_retVal = shmRunCommand(WCB_SIZE_ERASE_CMD);           /* Execute the Command in WCB */

    return l_retVal;
    
}


/*--------------------------------------------------------------------------------------*/
/* Function: shmFlashOpenProtectionWindow                                               */
/*                                                                                      */ 
/* Parameters:   p_address - Window start address                                       */
/*               p_size    - The size of the unprotected window                         */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Create a window that enable us to erase/ write to the flash            */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmFlashOpenProtectionWindow(unsigned long p_address, unsigned long p_size)
{   
    FU_RESULT l_retVal = FU_RESULT_OK;


    
    
    PrintText(APP_VERBOSE_PRINT, "shmFlashOpenProtectionWindow\n");
    PrintText(APP_VERBOSE_PRINT, "Set flash window, Address: %xh, window size: %xh\n", p_address, p_size);
            
     /* Setup the WCB for set flash window */
     
    m_WCB.Command                   = SET_WINDOW_OP;        /* Set window command           */
    m_WCB.Param.Address             = p_address;            /* Start address of the window  */
    m_WCB.Param.EightBytes.DWord2   = p_size;
   
    
    
    l_retVal = shmRunCommand(WCB_SIZE_SET_WINDOW);          /* Execute the Command in WCB   */

    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: shmPreFlashEnter                                                           */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Uses the WCB protocol to enable FW to perform preparation before       */
/*               the beginning of Flash Update procedure                                */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmPreFlashEnter(void)
{
    FU_RESULT l_retVal = FU_RESULT_OK;
    unsigned char l_semVal     = 0;
    unsigned long l_timeoutVal;

    PrintText(APP_VERBOSE_PRINT, "shmPreFlashEnter\n");
    
    /* Clear protocol */
       
    Pc2FlashWriteB(m_mappedWcbBaseAddress,
                  (unsigned char)(Pc2FlashReadB(m_mappedWcbBaseAddress) & ~SHAW2_SEM_EXE));

                                                         
    /* Setup the WCB for Enter */
    
    m_WCB.Command  = PRE_ENTER_OP;
    
    l_retVal = shmRunCommand(WCB_SIZE_PRE_ENTER_CMD);            /* Execute the Command in WCB  */
    
    /* Wait until done (indicated by PCP bit set by FW) */
    
    l_timeoutVal = WCB_TIMEOUT_B;
    
    do
    {
        l_timeoutVal--;
        l_semVal = Pc2FlashReadB(m_mappedWcbBaseAddress);
    } while ((!(l_semVal & SHAW2_SEM_PCP)) && (m_flashUpdateOptions->NoTimeout || l_timeoutVal));

    if(!m_flashUpdateOptions->NoTimeout && !(l_timeoutVal))
    {
        PrintText(APP_ERROR_PRINT,
                  "ERROR: Firmware did not set the PCP bit (semaphore = 0x%x)\n",
                  l_semVal);
        
        return FU_ERR_FIRMWARE_ERROR;
    }
    
    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: shmFlashEnter                                                              */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Uses the WCB protocol to indicate the beginning of                     */
/*               Flash Update procedure                                                 */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmFlashEnter(void)
{
    FU_RESULT l_retVal = FU_RESULT_OK;

    PrintText(APP_VERBOSE_PRINT, "shmFlashEnter\n");
    
    /* Clear protocol */
       
    Pc2FlashWriteB(m_mappedWcbBaseAddress,
                  (unsigned char)(Pc2FlashReadB(m_mappedWcbBaseAddress) & ~SHAW2_SEM_EXE));

                                                         
    /* Setup the WCB for Enter */
    
    m_WCB.Command  = ENTER_OP;
    m_WCB.Param.EnterCode = WCB_ENTER_CODE;    
    l_retVal = shmRunCommand(WCB_SIZE_ENTER_CMD);            /* Execute the Command in WCB  */        

    return l_retVal;
}



/*--------------------------------------------------------------------------------------*/
/* Function: shmFlashInit                                                               */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                Uses the WCB protocol to indicate the commands and                    */
/*                characteristics of the flash device in use                            */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmFlashInit()
{
    
    FU_RESULT l_retVal = FU_RESULT_OK;


    

    PrintText(APP_VERBOSE_PRINT,
              "\nshmFlashInit(%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)\n",
              m_flashConfig->FlashCommands.read_device_id, 
              m_flashConfig->FlashCommands.write_status_enable,
              m_flashConfig->FlashCommands.write_enable, 
              m_flashConfig->FlashCommands.read_status_reg, 
              m_flashConfig->FlashCommands.write_status_reg, 
              m_flashConfig->FlashCommands.page_program, 
              m_flashConfig->FlashCommands.sector_erase, 
              m_flashConfig->FlashCommands.status_busy_mask,
              m_flashConfig->FlashCommands.status_reg_val, 
              m_flashConfig->FlashCommands.program_unit_size, 
              m_flashConfig->FlashCommands.page_size,
              m_flashConfig->FlashCommands.read_dev_id_type);
      
    /* Clear protocol */   

    PrintText(APP_VERBOSE_PRINT, "Calling Pc2FlashWriteB\n");


    Pc2FlashWriteB(m_mappedWcbBaseAddress, (unsigned char)(Pc2FlashReadB(m_mappedWcbBaseAddress) & ~SHAW2_SEM_EXE));

    
    /* Setup the WCB for Init */
    
    m_WCB.Command = FLASH_COMMANDS_INIT_OP;
    m_WCB.Param.InitCommands   = m_flashConfig->FlashCommands; 

    PrintText(APP_VERBOSE_PRINT, "Calling shmRunCommand\n");
    
    l_retVal = shmRunCommand(1 + sizeof(FLASH_device_commands_t));	          /* Execute the Command in WCB */

    return l_retVal;    
}


/*--------------------------------------------------------------------------------------*/
/* Function: getSHMMemoryInterface                                                      */
/*                                                                                      */ 
/* Parameters:   p_memoryInterface - Will hold the memory interface type                */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Retrive the memory interface type (LPC or FWH)                         */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT getSHMMemoryInterface(SHM_mem_access_t *p_memoryInterface)
{
    FU_RESULT        l_retVal          = FU_RESULT_OK;
    unsigned char    l_charRead        = 0;

    
    
    PrintText(APP_VERBOSE_PRINT, "getSHMMemoryInterface()");
    
    PcIoWriteB(m_sioIndex, WIN_CFG);
    l_charRead = PcIoReadB(m_sioData);

    if (l_charRead & WRAM2_IO)
    {
        *p_memoryInterface = SHM_MEM_LPC_IO;
        PrintText(APP_VERBOSE_PRINT, " returned LPC IO\n");
    } 
    else if (l_charRead & SHWIN_ACC_FWH)
    {
        *p_memoryInterface = SHM_MEM_FWH;
        PrintText(APP_VERBOSE_PRINT, " returned FWH\n");        
    }
    else
    {      
        *p_memoryInterface = SHM_MEM_LPC;
        PrintText(APP_VERBOSE_PRINT, " returned LPC\n");
    }

    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: shmConfigMemAccess                                                         */
/*                                                                                      */ 
/* Parameters:   p_memoryAccessType - SHM_MEM_LPC - WPC876xL responds to LPC            */
/*                                                    memory transactions               */
/*                                    SHM_MEM_FWH - WPC876xL responds to FWH            */
/*                                                  memory transactions                 */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Configure the WPC876xL to respond to either LPC or FWH memory          */
/*               transactions for Shared Access Windows 1 and 2                         */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmConfigMemAccess(SHM_mem_access_t p_memoryAccessType)
{
    FU_RESULT l_retVal = FU_RESULT_OK;


    
    switch (p_memoryAccessType)
    {

    case SHM_MEM_LPC:

        PrintText(APP_VERBOSE_PRINT, "shmConfigMemAccess: configured LPC\n");
        
        PcIoWriteB(m_sioIndex, WIN_CFG);
        PcIoWriteB(m_sioData, (unsigned char)(PcIoReadB(m_sioData) & ~SHWIN_ACC_FWH));

        break;

        
    case SHM_MEM_FWH:

        PrintText(APP_VERBOSE_PRINT, "shmConfigMemAccess: configured FWH\n");
        
        PcIoWriteB(m_sioIndex, SHM_CFG);

        /* Configure Shared Windows FWH ID Value */
        
        PcIoWriteB(m_sioData, (unsigned char)(PcIoReadB(m_sioData) & ~SHW_FWH_ID_MASK));
        PcIoWriteB(m_sioData, (unsigned char)(PcIoReadB(m_sioData) | (SHW_FWH_ID << SHW_FWH_ID_POS))); 

        PcIoWriteB(m_sioIndex, WIN_CFG);
        PcIoWriteB(m_sioData, (unsigned char)(PcIoReadB(m_sioData) | SHWIN_ACC_FWH));

        break;


    default:
	break;
        
    }


    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: shmRunCommand                                                              */
/*                                                                                      */ 
/* Parameters:   p_commandLength - Size in bytes of command to be run                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Runs the command currently stored in the Write Command Buffer          */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmRunCommand(unsigned short p_commandLength)
{
    FU_RESULT     l_retVal     = FU_RESULT_OK;
    unsigned char l_semVal     = 0;
    unsigned long l_timeoutVal = WCB_TIMEOUT_A;



    
    /* Wait until firmware is ready to accept a command */
    /*  i.e. EXE and RDY bits in semaphore are cleared   */
       
    do
    {
        l_timeoutVal--;
        l_semVal = Pc2FlashReadB(m_mappedWcbBaseAddress);        
    } while ((l_semVal & (SHAW2_SEM_EXE | SHAW2_SEM_RDY)) && (m_flashUpdateOptions->NoTimeout || (l_timeoutVal)));
    
    if(!m_flashUpdateOptions->NoTimeout && !(l_timeoutVal))
    {
        PrintText(APP_ERROR_PRINT,
                  "ERROR: Firmware is not ready to accept command (semaphore = 0x%x)\n",
                  l_semVal);
        
        return FU_ERR_FIRMWARE_ERROR;
            
    }


    /* Write Command Buffer to memory  */

    Pc2FlashWrite(m_mappedWcbBaseAddress + OFFSET_COMMAND, p_commandLength, sizeof(unsigned char), &m_WCB);

   /* Enable EXE bit  */
   
    Pc2FlashWriteB(m_mappedWcbBaseAddress, (unsigned char)(Pc2FlashReadB(m_mappedWcbBaseAddress) | SHAW2_SEM_EXE));

    if (m_WCB.Command == RESET_EC_OP)
    {
        return FU_RESULT_OK;
    }

    /* Wait until done (indicated by RDY) */
    
    l_timeoutVal = WCB_TIMEOUT_A;
    do
    {
        l_timeoutVal--;
        l_semVal = Pc2FlashReadB(m_mappedWcbBaseAddress);
    } while ((!(l_semVal & SHAW2_SEM_RDY)) && (m_flashUpdateOptions->NoTimeout || l_timeoutVal));

    if(!m_flashUpdateOptions->NoTimeout && !(l_timeoutVal))
    {
        PrintText(APP_ERROR_PRINT,
                  "ERROR: Firmware did not set the RDY bit (semaphore = 0x%x)\n",
                  l_semVal);
        
        return FU_ERR_FIRMWARE_ERROR;
    }


    /* Clear EXE bit    */

    Pc2FlashWriteB(m_mappedWcbBaseAddress, (unsigned char)(Pc2FlashReadB(m_mappedWcbBaseAddress) & ~SHAW2_SEM_EXE));


    if (l_semVal & SHAW2_SEM_ERR)
    {
        PrintText(APP_ERROR_PRINT, 
                  "ERROR: Firmware reported error (semaphore value %x). Aborting\n",
                  l_semVal);
        
        return FU_ERR_FIRMWARE_ERROR;
    }


    /* Wait until Host clears RDY bit to acknowledge operation end */
    
    l_timeoutVal = WCB_TIMEOUT_A;
    do
    {
        l_timeoutVal--;
        l_semVal = Pc2FlashReadB(m_mappedWcbBaseAddress);
    } while ((l_semVal & SHAW2_SEM_RDY) && (m_flashUpdateOptions->NoTimeout || l_timeoutVal));

    if(!m_flashUpdateOptions->NoTimeout && !(l_timeoutVal))
    {
        PrintText(APP_ERROR_PRINT,
                  "ERROR: Host did not clear the RDY bit (semaphore = 0x%x)\n",
                  l_semVal);
        
        return FU_ERR_FIRMWARE_ERROR;
    }

    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: shmGetFlashChecksum                                                        */
/*                                                                                      */ 
/* Parameters:   p_address  - Start checksum from this address                          */
/*               p_size     - Number of bytes to checksum                               */
/*               p_checksum - Will contain the checksum number                          */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Get the checksum of a spesific area of the flash.                      */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmGetFlashChecksum(unsigned long p_address,
                                        unsigned long p_size,
                                        unsigned short *p_checksum)
{
    FU_RESULT l_retVal = FU_RESULT_OK;


     
    m_WCB.Command                   = CHECKSUM_OP;        /* Set command */
    m_WCB.Param.Address             = p_address;          /* Checksum start address */
    m_WCB.Param.EightBytes.DWord2   = p_size;             /* Checksum size */
   
    
    
    l_retVal = shmRunCommand(WCB_SIZE_CALC_CHECKSUM);        /* Execute the Command in WCB */
    
    *p_checksum = (unsigned short) Pc2FlashReadW(m_mappedWcbBaseAddress + OFFSET_DATA);

    return l_retVal;

}


/*--------------------------------------------------------------------------------------*/
/* Function: shmSetFlashWindow                                                          */
/*                                                                                      */ 
/* Parameters:   p_baseAddr  - Address to be set as base for host flash access          */
/*               p_flashSize - Size of flash window (must be size of total flash)       */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Sets the base address of the Shared Flash Window 1.                    */
/*               This window allows direct flash read access to the Host                */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmSetFlashWindow(unsigned long p_baseAddr, unsigned long p_flashSize)
{
    FU_RESULT l_retVal = FU_RESULT_OK;


    
    PrintText(APP_VERBOSE_PRINT, "SetFlashWindow(BaseAddr = %Xh)\n", p_baseAddr);

    /* Select shared window 1 for either flash or RAM access */
    
    PcIoWriteB(m_sioIndex, SHM_CFG);
    if (p_baseAddr != 0)
    {
        
        /* Flash window base address must be aligned to flash size */
        
        if (p_baseAddr % p_flashSize)
        {
            return FU_ERR_BASE_ADDRESS_NOT_ALIGNED;
        }

        /* Select Flash Access */
        
        PcIoWriteB(m_sioData, (unsigned char)(PcIoReadB(m_sioData) | FLASH_ACC_EN));
    }
    else
    {
        /* Disable Flash Access */
        
        PcIoWriteB(m_sioData, (unsigned char)(PcIoReadB(m_sioData) & ~FLASH_ACC_EN));
    }

    /* Configure the flash window size according to total flash size */
    
    PcIoWriteB (m_sioIndex, WIN_CFG);
    switch (p_flashSize)
    {
    case (128 * 1024):
        PcIoWriteB (m_sioData, (unsigned char)(FWIN1_SIZE_128_KB |
                               (PcIoReadB(m_sioData) & ~FWIN1_SIZE_MASK)));
        break;
        
    case (256 * 1024):
        PcIoWriteB (m_sioData, (unsigned char)(FWIN1_SIZE_256_KB |
                               (PcIoReadB(m_sioData) & ~FWIN1_SIZE_MASK)));
        break;
        
    case (512 * 1024):
        PcIoWriteB (m_sioData, (unsigned char)(FWIN1_SIZE_512_KB |
                               (PcIoReadB(m_sioData) & ~FWIN1_SIZE_MASK)));
        break;
        
    case (1024 * 1024):
        PcIoWriteB (m_sioData, (unsigned char)(FWIN1_SIZE_1_MB |
                               (PcIoReadB(m_sioData) & ~FWIN1_SIZE_MASK)));
        break;
        
    case (2 * 1024 * 1024):
        PcIoWriteB (m_sioData, (unsigned char)(FWIN1_SIZE_2_MB |
                               (PcIoReadB(m_sioData) & ~FWIN1_SIZE_MASK)));
        break;
        
    case (4 * 1024 * 1024):
        PcIoWriteB (m_sioData, (unsigned char)(FWIN1_SIZE_4_MB |
                               (PcIoReadB(m_sioData) & ~FWIN1_SIZE_MASK)));
        break;
        
    default:

        if (p_flashSize < (128 * 1024))
        {
            PcIoWriteB (m_sioData, (unsigned char)(FWIN1_SIZE_128_KB |
                                    (PcIoReadB(m_sioData) & ~FWIN1_SIZE_MASK)));
        }
        else
        {
            /* Illegal flash size */
            
            return FU_ERR_ILLEGAL_FLASH_SIZE;
        }

    }

    PcIoWriteB (m_sioIndex, SHAW1BA_2);                                        /* Configure address bits 17-23 */
    PcIoWriteB (m_sioData, (unsigned char)(((p_baseAddr >> 16) & 0xFE) |
                           (PcIoReadB(m_sioData) & 0x1)));
 
    if (m_flashUpdateOptions->SharedMemoryInterface == SHM_MEM_LPC)
    {
        PcIoWriteB (m_sioIndex, SHAW1BA_3);          
        PcIoWriteB (m_sioData, (unsigned char)((p_baseAddr >> 24) & 0xFF));     /* Configure address bits 24-31 */
    }
    else                                                                        /* FWH */
    {
        PcIoWriteB (m_sioIndex, SHAW1BA_3);                                     /* Configure address bits 24-27 */
        PcIoWriteB (m_sioData, (unsigned char)(((p_baseAddr >> 24) & 0x0F) |
                               (PcIoReadB(m_sioData) & 0xF0)));
    }

    return l_retVal;

}


/*--------------------------------------------------------------------------------------*/
/* Function: shmSetWindow2BaseAddress                                                   */
/*                                                                                      */ 
/* Parameters:   p_baseAddr  - Address to be set as base for host shared memory access  */     
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Sets the base address of the Shared Memory Window 2.                   */
/*               This window is used to implement the Windows Command Buffer            */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT shmSetWindow2BaseAddress(unsigned long p_baseAddr)
{
    FU_RESULT l_retVal = FU_RESULT_OK;


    
    PrintText(APP_VERBOSE_PRINT, "shmSetWindow2BaseAddress(BaseAddr = %Xh) \n", p_baseAddr);

    PcIoWriteB (m_sioIndex, SHAW2BA_0);                                       /* Configure address bits 0-7    */
    PcIoWriteB (m_sioData, (unsigned char)((p_baseAddr) & 0xFF));
    
    PcIoWriteB (m_sioIndex, SHAW2BA_1);                                       /* Configure address bits 8-15   */
    PcIoWriteB (m_sioData, (unsigned char)((p_baseAddr >> 8) & 0xFF));
    
    PcIoWriteB (m_sioIndex, SHAW2BA_2);                                       /* Configure address bits 16-23  */
    PcIoWriteB (m_sioData, (unsigned char)((p_baseAddr >> 16) & 0xFF));
    
    PcIoWriteB (m_sioIndex, SHAW2BA_3);          
    if (m_flashUpdateOptions->SharedMemoryInterface == SHM_MEM_LPC)
    {
        PrintText(APP_VERBOSE_PRINT, "For LPC\n");                
        PcIoWriteB (m_sioData, (unsigned char)((p_baseAddr >> 24) & 0xFF));  /* Configure address bits 24-31 */
    }
    else
    {
        PrintText(APP_VERBOSE_PRINT, "For FWH\n");        
    }

    PcIoWriteB (m_sioData, (unsigned char)((p_baseAddr >> 24) & 0x0F));  /* Configure address bits 24-27 */

    return l_retVal;
    
}


/*--------------------------------------------------------------------------------------*/
/* Function: programFlash                                                               */
/*                                                                                      */ 
/* Parameters:   p_address  - Starting address                                          */     
/*               p_size     - Number of bytes to program. Should be a multiple of 8     */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               program 'size' bytes to the flash starting from address 'Address'      */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT programFlash(unsigned long p_address, unsigned long p_size)
{
    FU_RESULT     l_retVal       = FU_RESULT_OK;
    unsigned long l_lastAddress  = 0;
    unsigned long l_addr         = 0;
    int           l_error        = 0;
    int           l_retry        = 0;
    BOOLEAN       l_same         = FALSE;
    unsigned int  l_padIndex     = 0;
    unsigned char l_pad[8]       = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};



    l_retry = 3;
    do
    {
        l_error = 0;
        l_retry--;

        if (m_flashUpdateOptions->Verbose)
        {
            PrintText(APP_DEFAULT_PRINT, "%cProgress: Address [%X] |", 0xD, p_address);
        }

        shmFlashSetAddress(p_address);
        if ((p_address + p_size) < m_flashUpdateOptions->NumOfBytesToDownload + m_flashUpdateOptions->FlashOffset)
        {
            l_lastAddress =  p_address + p_size;  
        }
        else
        {
            l_lastAddress = m_flashUpdateOptions->NumOfBytesToDownload + m_flashUpdateOptions->FlashOffset;
        }
                
        for (l_addr = p_address; l_addr < l_lastAddress; l_addr += 8)
        {
            if ((m_flashUpdateOptions->NumOfBytesToDownload + m_flashUpdateOptions->FlashOffset - l_addr) >= 8)
            {
                l_retVal = shmFlashProgram((unsigned char *)&m_flashUpdateOptions->BinaryImageFilePtr[l_addr - m_flashUpdateOptions->FlashOffset], 8);
            }
            else
            {
                for (l_padIndex = 0; l_addr < m_flashUpdateOptions->NumOfBytesToDownload + m_flashUpdateOptions->FlashOffset;)
                {
                    l_pad[l_padIndex++] = m_flashUpdateOptions->BinaryImageFilePtr[l_addr - m_flashUpdateOptions->FlashOffset];
                    ++l_addr;
                }
                l_retVal = shmFlashProgram(&l_pad[0], 8);
                
            }
            if (FU_RESULT_OK != l_retVal)
            {
                return l_retVal;
            }
            
            if (((l_addr & 0x3ff) == 0) && (m_flashUpdateOptions->Verbose))
            {               
                m_printProgress++;
                if (m_printProgress == 4)
                {
                    m_printProgress = 0;
                }
            }
        }

        if(!m_flashUpdateOptions->NoVerify)
        {
            /* Verify data */
            
            PrintText(APP_DEFAULT_PRINT, "%cV",0x08);
                  
            PrintText(APP_VERBOSE_PRINT, " Verify data from address %X to %X\n", p_address, p_address + p_size);

            l_retVal = compareFlash(p_address, p_size, &l_same, 0);
            if (FU_RESULT_OK != l_retVal)
            {
                return l_retVal;
            }
            if (!l_same)
            {
                l_error++;
                    
                shmFlashSectorErase(p_address);
            }
        }
        
    } while (l_retry && l_error);

    if (l_error)
    {
        PrintText(APP_ERROR_PRINT,
                  "ERROR: Fail programming address %X to %X. Aborting\n",                  
                  p_address,
                  p_address + p_size);  
            
        return FU_ERR_FAIL_PROGRAMING_ADDRESS;
    }
    

    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: compareFlash                                                               */
/*                                                                                      */
/* Parameters:   p_address  - Starting address                                          */    
/*               p_size     - Number of bytes to compare.                               */
/*               p_same     - TRUE - Data in flash is identical to data to be programmed*/
/*                          - FALSE - Otherwise                                         */
/*               p_forceCompareByRead - 0: always verify by reading                     */
/*                                      1: In IO mode verify by checksum                */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               compare 'p_size' bytes of the flash starting from address 'p_address'  */
/*               and set 'p_same' accordingly                                           */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT compareFlash(unsigned long p_address,
                                unsigned long p_size,
                                BOOLEAN *p_same,
                                int p_forceCompareByRead)
{
    FU_RESULT     l_retVal       = FU_RESULT_OK;
    unsigned long l_lastAddress  = 0;
    unsigned long l_addr         = 0;
    unsigned char l_data         = 0;
    int           l_diff         = 0;
    unsigned long l_mappedFlashBaseAddressTemp    = 0;
    unsigned int  l_numOfBytesToRead = 0;
    char          l_readData[12];
    unsigned long i = 0;
    unsigned short l_flashChecksum = 0;
    unsigned short l_binChecksum = 0;
    unsigned long l_startOffset = 0;
    unsigned long l_compareSize = 0;
    int           l_compareFlag = 0;
    unsigned long l_prevStartAddress = 0;
    
    


    if ((p_address + p_size) < (m_flashUpdateOptions->NumOfBytesToDownload + m_flashUpdateOptions->FlashOffset))
    {
        l_lastAddress =  p_address + p_size;
    }
    else
    {
        l_lastAddress = m_flashUpdateOptions->NumOfBytesToDownload + m_flashUpdateOptions->FlashOffset;
    }
                  
    *p_same = FALSE;

    if (SHM_MEM_LPC_IO == m_flashUpdateOptions->SharedMemoryInterface)
    {// IO
        if (!p_forceCompareByRead && !m_flashUpdateOptions->VerifyByRead)
        {// verify by checksum
            if (m_areFlashProtectionWindowsSupported)
            {
                l_compareFlag = getFlashUnprotectedArea(p_address,
                                                        l_lastAddress - p_address,
                                                        &l_startOffset,
                                                        &l_compareSize,
                                                        FlashProtection_ReadProtected);
                while (l_compareFlag)
                {
                    l_prevStartAddress = l_startOffset;
                    
                    PrintText(APP_DEFAULT_PRINT,
                              " Compare (Checksum) data from address %X to %X\n",
                              l_startOffset, 
                              l_startOffset + l_compareSize);
                             

                    l_diff = 1;
                    l_binChecksum = 0;
                    l_retVal = shmGetFlashChecksum(l_startOffset, l_compareSize, &l_flashChecksum);
                    if (FU_RESULT_OK != l_retVal)
                    {
                        return l_retVal;
                    }
                    for (i = l_startOffset; i < l_startOffset + l_compareSize; ++i)
                    {
                        l_binChecksum = l_binChecksum + (unsigned char)m_flashUpdateOptions->BinaryImageFilePtr[i - m_flashUpdateOptions->FlashOffset];
                    }

                    PrintText(APP_DEFAULT_PRINT, "Bin checksum: %d , flash checksum: %d\n", l_binChecksum , l_flashChecksum );
                    if (l_binChecksum == l_flashChecksum)
                    {
                        l_diff = 0;
                        *p_same = TRUE;
                   
                        if (l_startOffset + l_compareSize < l_lastAddress)
                        {

                                        
                            l_compareFlag = getFlashUnprotectedArea(l_startOffset + l_compareSize + 1,
                                                                    l_lastAddress - l_startOffset - l_compareSize - 1,
                                                                    &l_startOffset,
                                                                    &l_compareSize,
                                                                    FlashProtection_ReadProtected);                            
                                                    
                            if (l_prevStartAddress == l_startOffset)
                            {
                                l_compareFlag = 0;
                            }
                        }
                        else
                        {
                            l_compareFlag = 0;
                        }                        
                    }
                    else
                    {
                        *p_same = FALSE;
                        l_diff++;
                        l_compareFlag = 0;
                        break;
                    }                    
                }
                
            }
            else
            {                    
                if (m_flashUpdateOptions->Verbose)
                {
                    PrintText(APP_DEFAULT_PRINT, " Compare (Checksum) data from address %X to %X\n", p_address, p_address + p_size);
                }            

                l_diff = 1;
                l_retVal = shmGetFlashChecksum(p_address, l_lastAddress - p_address, &l_flashChecksum);
                if (FU_RESULT_OK != l_retVal)
                {
                    return l_retVal;
                }
                for (i = p_address; i < l_lastAddress; ++i)
                {
                    l_binChecksum = l_binChecksum + (unsigned char)m_flashUpdateOptions->BinaryImageFilePtr[i - m_flashUpdateOptions->FlashOffset];
                }

                PrintText(APP_DEFAULT_PRINT, "Bin checksum: %d , flash checksum: %d\n", l_binChecksum , l_flashChecksum );
                if (l_binChecksum == l_flashChecksum)
                {
                    l_diff = 0;
                    *p_same = TRUE;
                }
            }
        // verify by checksum - end
        }
        else
        {// verify by WCB read
            if (m_flashUpdateOptions->Verbose)
            {
                PrintText(APP_DEFAULT_PRINT, " Compare (WCB Read) data from address %X to %X\n", p_address, p_address + p_size);
            }
            
            shmFlashSetAddress(p_address);
            
            for (l_addr = p_address; (l_addr < l_lastAddress) && !l_diff; l_addr = l_addr + l_numOfBytesToRead)
            {
                if ((l_lastAddress - l_addr) > WCB_MAX_READ_BYTES)
                {
                    l_numOfBytesToRead = WCB_MAX_READ_BYTES;
                }
                else
                {
                    l_numOfBytesToRead = l_lastAddress - l_addr;
                }

                if (!m_areFlashProtectionWindowsSupported || (!isFlashAreaProtected(l_addr,
                                                                                    l_numOfBytesToRead,
                                                                                    FlashProtection_ReadProtected)))
                {
                    
                    l_retVal = shmFlashRead((unsigned char *)l_readData, l_numOfBytesToRead);
                    if (FU_RESULT_OK != l_retVal)
                    {
                        return l_retVal;
                    }

                    for ( i = 0; i < l_numOfBytesToRead; ++i)
                    {
                        if (l_readData[i] != m_flashUpdateOptions->BinaryImageFilePtr[l_addr + i - m_flashUpdateOptions->FlashOffset])
                        {
                            l_diff++;

                            if (m_flashUpdateOptions->Verbose)
                            {
                                PrintText(APP_DEFAULT_PRINT, 
                                          "Flash compare difference: Linear Base = %08Xh, Offset = %Xh, Destination = %Xh, Source = %Xh\n",
                                         m_mappedFlashBaseAddress, 
                                         l_addr + i,
                                         l_readData[i],
                                         (unsigned char)m_flashUpdateOptions->BinaryImageFilePtr[l_addr + i - m_flashUpdateOptions->FlashOffset]);
                            }
                       }
                    }
                }
                else
                {
                    shmFlashSetAddress(l_addr + l_numOfBytesToRead);
                }
            } 
        // verify by WCB read-end
        }
            
    // IO-end             
    }
    else
    {// MEM           
                    
        if (!m_flashUpdateOptions->VerifyByRead) // do not verify by WCB read command
        {
            if (m_flashUpdateOptions->Verbose)
            {
                PrintText(APP_DEFAULT_PRINT, " Compare (Mapped Read) data from address %X to %X\n", p_address, p_address + p_size);
            }
            
            if(0 == m_useSharedBios)
            {
                shmSetWindow2BaseAddress(0);
                
                /* Set base address for Shared Flash window */
                    
                l_retVal = shmSetFlashWindow(m_pcWcbBase, m_flashConfig->FlashSize * 1024); 
                if (FU_RESULT_OK != l_retVal)
                {
                    return l_retVal;
                }
            }

            for (l_addr = p_address; (l_addr < l_lastAddress) && !l_diff; ++l_addr)
            {

                /* Check need for mapping a new block (first block was mapped in ConfigFlashSharedMemory) */
                /* Compare physical address of current byte to physical address of last mapped byte */
                /* To prevent wraparound at 32 bits, the offsets are compared, instead of physical address values */
                    
                if ((l_addr >= (m_lastMapped - m_flashBase + m_memMapSize)) || (l_addr < (m_lastMapped - m_flashBase)))
                {
                    /* Check need for unmapping a previously mapped block */
                    /* Do not unmap WCB block as it it always needed */

                    if (m_lastMapped != m_pcWcbBase)
                    {
                        l_mappedFlashBaseAddressTemp = m_lastMapped;
                        PrintText(APP_DEFAULT_PRINT, "Unmap %lx\n", m_lastMapped);
                        PcMemUnmap(&l_mappedFlashBaseAddressTemp, m_memMapSize);
                    }

                    /* calculate current flash byte's physical address */
                         
                    l_mappedFlashBaseAddressTemp = m_lastMapped = m_flashBase + l_addr;

                    PrintText(APP_DEFAULT_PRINT, "Start mapping %lx\n", m_lastMapped);
                    if (!PcMemMap(&l_mappedFlashBaseAddressTemp, m_memMapSize))
                    {
                        PrintText(APP_ERROR_PRINT,
                                  "ERROR: Could not map %d bytes of memory at %lx !\n",
                                  m_memMapSize,
                                  m_lastMapped);
                      
                        return FU_ERR_MEMORY_MAP_FAILED;
                    }

                    /* Adjust m_mappedFlashBaseAddress to start of linear range since it is used in next loop */
                        
                    m_mappedFlashBaseAddress = l_mappedFlashBaseAddressTemp - l_addr;
                    PrintText(APP_DEFAULT_PRINT, "Map address: 0x%x to 0x%x, linear base is 0x%x\n",
                              m_flashBase + l_addr, l_mappedFlashBaseAddressTemp, m_mappedFlashBaseAddress);
                }



                if (!m_areFlashProtectionWindowsSupported || !isFlashByteReadProtected(l_addr))
                {
                    
                    /* Read and compare data */

                    l_data = PcMemReadB(m_mappedFlashBaseAddress + l_addr);

                    if (l_data != (unsigned char)m_flashUpdateOptions->BinaryImageFilePtr[l_addr - m_flashUpdateOptions->FlashOffset])
                    {
                        l_diff++;

                        if (m_flashUpdateOptions->Verbose)
                        {
                            PrintText(APP_DEFAULT_PRINT, 
                                      "Flash compare difference: Linear Base = %08Xh, Offset = %Xh, Destination = %Xh, Source = %Xh\n",
                                     m_mappedFlashBaseAddress, l_addr, l_data, (unsigned char)m_flashUpdateOptions->BinaryImageFilePtr[l_addr - m_flashUpdateOptions->FlashOffset]);
                        }
                    }                
                }
            }            
            
            if(0 == m_useSharedBios)
            {
                shmSetFlashWindow(0, 0);
                shmSetWindow2BaseAddress(m_pcWcbBase);
            }
        }
        else // verify by WCB read command
        {
            if (m_flashUpdateOptions->Verbose)
            {
                PrintText(APP_DEFAULT_PRINT, " Compare (WCB Read) data from address %X to %X\n", p_address, p_address + p_size);
            }
            
            shmFlashSetAddress(p_address);
            
            for (l_addr = p_address; (l_addr < l_lastAddress) && !l_diff; l_addr = l_addr + l_numOfBytesToRead)
            {
                if ((l_lastAddress - l_addr) > WCB_MAX_READ_BYTES)
                {
                    l_numOfBytesToRead = WCB_MAX_READ_BYTES;
                }
                else
                {
                    l_numOfBytesToRead = l_lastAddress - l_addr;
                }

                if (!m_areFlashProtectionWindowsSupported || (!isFlashAreaProtected(l_addr,
                                                                                    l_numOfBytesToRead,
                                                                                    FlashProtection_ReadProtected)))
                {
                    
                    l_retVal = shmFlashRead((unsigned char *)l_readData, l_numOfBytesToRead);
                    if (FU_RESULT_OK != l_retVal)
                    {
                        return l_retVal;
                    }

                    for ( i = 0; i < l_numOfBytesToRead; ++i)
                    {
                        if (l_readData[i] != m_flashUpdateOptions->BinaryImageFilePtr[l_addr + i - m_flashUpdateOptions->FlashOffset])
                        {
                            l_diff++;

                            if (m_flashUpdateOptions->Verbose)
                            {
                                PrintText(APP_DEFAULT_PRINT, 
                                          "Flash compare difference: Linear Base = %08Xh, Offset = %Xh, Destination = %Xh, Source = %Xh\n",
                                         m_mappedFlashBaseAddress, 
                                         l_addr + i,
                                         l_readData[i],
                                         (unsigned char)m_flashUpdateOptions->BinaryImageFilePtr[l_addr + i - m_flashUpdateOptions->FlashOffset]);
                            }
                       }
                    }
                }
                else
                {
                    shmFlashSetAddress(l_addr + l_numOfBytesToRead);
                }
            }    
        }
    // MEM-end    
    }

    if (!l_diff)
    {     
        *p_same = TRUE;
        if (m_flashUpdateOptions->Verbose)
        {
            PrintText(APP_DEFAULT_PRINT, " Identical data from address %X to %X\n", p_address, p_address + p_size);
        }
    }
    
    return l_retVal;
}

/*--------------------------------------------------------------------------------------*/
/* Function: configSharedMemoryInterface                                                */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Configure the WPC876xL device Shared Memory interface                  */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT configSharedMemoryInterface()
{
    FU_RESULT l_retVal = FU_RESULT_OK;



    
    /* Look for the WPC876xL Device */
    
    if (0 == m_flashUpdateOptions->SkipConfig) IOConfigure();

    if (!locateLogicalDeviceIndex())
    {
        PrintText(APP_ERROR_PRINT, "Nuvoton EC or Desktop Super IO Device wasn't found!\n");
        return FU_ERR_DEVICE_NOT_FOUND;
    }

    
    /* Select the Shared Memory LDN for subsequent LDN accesses */
    
    selectLogicalDevice(LDN_SHM);

    l_retVal = backupRegisters(0); 
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    
    /* Enable and map memory access from CPU to the memory range used as shared memory */
    
    if (0 == m_flashUpdateOptions->SkipConfig)
    {
            
        if (m_flashUpdateOptions->SharedMemoryInterface == SHM_MEM_FWH)
        {
            m_pcWcbBase = PC_WCB_BASE;            
        }
        else
        {
            m_pcWcbBase = PC_WCB_BASE_LPC;
        }

        
        /* Enable memory access through LPC                              */          
        /* (controlled by ICH) to the memory range used as shared memory */
        
        l_retVal = LPCConfigure(m_flashUpdateOptions->SharedMemoryInterface);
        if (FU_RESULT_OK != l_retVal)
        {
            return l_retVal;
        }
        
        /* Configure the WPC876xL to respond to either LPC or        */
        /* FWH memory transactions for Shared Access Windows 1 and 2 */
           
        l_retVal = shmConfigMemAccess(m_flashUpdateOptions->SharedMemoryInterface);
        if(FU_RESULT_OK != l_retVal)
        {
            return l_retVal;
        }
              
        /* Configure base address of the Shared Memory window (WCB)       */
        /* (the address space was enabled in the chipset by LPCConfigure) */
           
        l_retVal = shmSetWindow2BaseAddress(m_pcWcbBase);   
        if (FU_RESULT_OK != l_retVal)
        {
            return l_retVal;
        }

    }
    else
    {
        PcIoWriteB(m_sioIndex, 0x60);
        m_shmBaseAddress = PcIoReadB(m_sioData) << 8;
        PcIoWriteB(m_sioIndex, 0x61);
        m_shmBaseAddress = m_shmBaseAddress | PcIoReadB(m_sioData);
        PrintText(APP_VERBOSE_PRINT, "Shared Memory Base Address: 0x%x\n", m_shmBaseAddress);
    
        l_retVal = getSHMMemoryInterface(&m_flashUpdateOptions->SharedMemoryInterface);
        if(FU_RESULT_OK != l_retVal)
        {
            return l_retVal;
        }

        if (SHM_MEM_LPC_IO == m_flashUpdateOptions->SharedMemoryInterface)
        {
            SetFlashAccess(PROGRAM_THROUGH_IO);
        }
        else
        {
            SetFlashAccess(PROGRAM_THROUGH_MEMORY);
        }
        
        m_pcWcbBase = shmGetWindow2BaseAddress();
    }


    if ((m_pcWcbBase & 0x0FFFFFFF) == 0)
    {
        PrintText(APP_ERROR_PRINT, "ERROR: Shared memory base address not configured.\n");
        return FU_ERR_SHM_NOT_CONFIG;
    }
    else
    {
        PrintText(APP_VERBOSE_PRINT, "Shared memory base address is %lx\n", m_pcWcbBase);        
    }


    /* Mapping of WCB */
    
    m_mappedWcbBaseAddress = m_pcWcbBase;

    if (SHM_MEM_LPC_IO != m_flashUpdateOptions->SharedMemoryInterface)
    {
        /* When working with WCB over the memory we need to map 16-bits in WCB base address */
        
        if (!PcMemMap(&m_mappedWcbBaseAddress, 16 ))           
        {
            PrintText(APP_ERROR_PRINT,
                      "ERROR: Could not map %d bytes of memory at %lx !\n",
                      16,
                      m_pcWcbBase);
              
            return FU_ERR_MEMORY_MAP_FAILED;
        }
        else
        {
            PrintText(APP_VERBOSE_PRINT, "Mapped %X to %X for WCB\n", m_pcWcbBase, m_mappedWcbBaseAddress);
        }
    }

    /* Finally enable the Shared Memory logical device */
    
    setLogicalDeviceEnable(1);

    if (m_flashUpdateOptions->SharedBIOS)
    {
        PrintText(APP_VERBOSE_PRINT, "Using shared BIOS\n");
        m_useSharedBios = 1;
    }
    else if (m_flashUpdateOptions->NotSharedBIOS)
    {
        PrintText(APP_VERBOSE_PRINT, "Not using shared BIOS\n");
        m_useSharedBios = 0;
    }
    else
    {
        m_useSharedBios = isSharedBios();
    }
    
    if(!m_useSharedBios)
    {
        if (SHM_MEM_LPC_IO != m_flashUpdateOptions->SharedMemoryInterface)
        {
            SetToWorkAsSharedBIOS();
        }
    }

    DisablePFEMenlow();
    
    return FU_RESULT_OK;
}

/*--------------------------------------------------------------------------------------*/
/* Function: IsInternalClock                                                            */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      Boolean:                                                               */
/*               If the chip uses internal clock - TRUE. Else - FALSE.                  */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*              This functions checks whether the chip is using internal or exteranl    */
/*              clock by reading SRID registers                                         */
/*--------------------------------------------------------------------------------------*/

BOOLEAN IsInternalClock()
{ 
    unsigned char l_srid;
    BOOLEAN rc = FALSE;


    PrintText(APP_VERBOSE_PRINT, "IsInternalClock() \n");

    PcIoWriteB(m_sioIndex, 0x27);
    l_srid = PcIoReadB(m_sioData);
    
    PrintText(APP_DEFAULT_PRINT, "SRID value: %x \n", l_srid);
    
    if((l_srid & 0xE1) == 0x01)
    {
        PrintText(APP_DEFAULT_PRINT, "Internal clock is used \n");
        rc = TRUE;
    }
    else
    {
        PrintText(APP_DEFAULT_PRINT, "External clock is used \n");
        rc = FALSE;
    }    
    return rc;
}

/*--------------------------------------------------------------------------------------*/
/* Function: IsCalibrationValuesPreservationNeeded                                      */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      Boolean:                                                               */
/*               If calibration values preservation is needed - TRUE. Else - FALSE.     */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*              This functions checks whether there is a need to preserve the           */
/*              the calibration values. The result is true if:                          */
/*              1.ocv flag is not used  and                                             */
/*              2.EC has an internal clock                                              */
/*--------------------------------------------------------------------------------------*/

BOOLEAN IsCalibrationValuesPreservationNeeded()
{ 
    return ((FALSE == m_flashUpdateOptions->OverwriteCalibrationValues) && (TRUE == IsInternalClock()));
}


/*--------------------------------------------------------------------------------------*/
/* Function: GetCalibrationValuesFromFlash                                              */
/*                                                                                      */ 
/* Parameters:   p_binaryImageFilePtr - pointer to the binary image file                */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/*               Populates the return code from the function ReadBytesFromFlash         */ 
/*                                                                                      */ 
/* Side effects: Modifies the binary image file                                         */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*                                                                                      */
/*              Reads FRCDIV_DAT1/2 data from flash and stores it in the binary         */
/*              image file                                                              */
/*--------------------------------------------------------------------------------------*/

FU_RESULT GetCalibrationValuesFromFlash(char* p_binaryImageFilePtr)
{
    FU_RESULT rc = FU_ERR_UNEXPECTED_ERROR;
    int i = 0;
    
    PrintText(APP_VERBOSE_PRINT, "SaveCalibrationValues() \n");
    
    rc = ReadBytesFromFlash(OFFSET_FRCDIV_DAT1, (unsigned char *)&(p_binaryImageFilePtr[OFFSET_FRCDIV_DAT1]), FRCDIV_DAT_SIZE*2);

    for(i = 0;i < (FRCDIV_DAT_SIZE*2);i++)
    {
        PrintText(APP_VERBOSE_PRINT, "p_binaryImageFilePtr[%x]: %x \n",(OFFSET_FRCDIV_DAT1+i),(unsigned char)p_binaryImageFilePtr[OFFSET_FRCDIV_DAT1+i]);
    }
    return rc;
} 

 


