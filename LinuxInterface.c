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


#include <stdio.h>
#include <stdbool.h>
#include <sys/io.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "FUresult.h"
#include "AppDisplay.h"
#include "drv/hwrw.h"


int HWRW_FD;

void open_hwrw_drv()
{
    int i = 0, retval;
    HWRW_FD = open("/dev/hwrw", O_RDONLY);
    
    if (HWRW_FD == -1) 
    {
        printf("Error can not open /dev/hwrw \n");
        printf("Trying to install driver\n");
        retval = system("insmod drv/hwrw.ko");
        do 
        {
            i++;
            HWRW_FD = open("/dev/hwrw", O_RDONLY);
            if (i > 1000) 
            {
                printf("Error in Opening /dev/hwrw (%d)\n",i);
                return;
            }
        }while (HWRW_FD == -1);
    }
}

/*--------------------------------------------------------------------------------------*/
/* Function: PcMemMap                                                                   */
/*                                                                                      */
/* Parameters:   AddressP - A pointer to a physical address                             */
/*               len      - Size to map                                                 */
/*                                                                                      */
/* Returns:      Correct flash configuration                                            */
/*                                                                                      */
/* Side effects: Sets the offset argument to point to the virtual address of            */
/*               the memory pointed by the physical address argument                    */
/*                                                                                      */
/* Description:                                                                         */
/*               This function maps a physical memory area of specified length          */
/*--------------------------------------------------------------------------------------*/
bool PcMemMap( unsigned long * AddressP, unsigned int len )
{
    GeneralParam_T P;    
    
    P.p_1 = (unsigned long)AddressP;
    P.p_2 = len;
    
    ioctl(HWRW_FD, HWRW_IOC_MEM_MAP, &P);    

    return true;
}

/*--------------------------------------------------------------------------------------*/
/* Function: PcMaxMapSize                                                               */
/*                                                                                      */
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      Max mapping size or 0 if there is no limit                             */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Max mapping size or 0 if there is no limit                             */
/*--------------------------------------------------------------------------------------*/

unsigned int PcMaxMapSize(void)
{
    return 0;                                             /* No limit in DOS            */
}


/*--------------------------------------------------------------------------------------*/
/* Function: PcMemUnmap                                                                 */
/*                                                                                      */
/* Parameters:   AddressP - A pointer to a physical address                             */
/*               len      - Size to unmap                                               */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Unmap the given size starting from the given address                   */
/*--------------------------------------------------------------------------------------*/
void PcMemUnmap(unsigned long *AddressP, unsigned int len)
{
    GeneralParam_T P;    
    
    P.p_1 = *AddressP;
    P.p_2 = len;
    
    ioctl(HWRW_FD, HWRW_IOC_MEM_UNMAP, &P);    
}

/*--------------------------------------------------------------------------------------*/
/* Function: PcMemReadB                                                                 */
/*                                                                                      */
/* Parameters:   Address - The physical address in the PC to write into                 */
/*                                                                                      */
/* Returns:      The read value                                                         */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Read one byte word from PC linear memory                               */
/*--------------------------------------------------------------------------------------*/

unsigned char PcMemReadB (unsigned long Address)
{
    GeneralParam_T P;
    unsigned char res;
    
    P.p_1 = Address;
    P.p_2 = (unsigned long)&res;
    
    ioctl(HWRW_FD, HWRW_IOC_MEM_READ_8, &P);

    return res;
}


/*--------------------------------------------------------------------------------------*/
/* Function: PcMemReadW                                                                 */
/*                                                                                      */
/* Parameters:   Address - The physical address in the PC to write into                 */
/*                                                                                      */
/* Returns:      The read value                                                         */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Read one word word from PC linear memory                               */
/*--------------------------------------------------------------------------------------*/

unsigned short PcMemReadW (unsigned long Address)
{
    GeneralParam_T P;
    unsigned short res;
    
    P.p_1 = Address;
    P.p_2 = (unsigned long)&res;
    
    ioctl(HWRW_FD, HWRW_IOC_MEM_READ_16, &P);

    return res;
}


/*--------------------------------------------------------------------------------------*/
/* Function: PcMemReadD                                                                 */
/*                                                                                      */
/* Parameters:   Address - The physical address in the PC to write into                 */
/*                                                                                      */
/* Returns:      The read value                                                         */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Read one double word from PC linear memory                             */
/*--------------------------------------------------------------------------------------*/

unsigned long PcMemReadD (unsigned long Address)
{
    GeneralParam_T P;
    unsigned long res;
    
    P.p_1 = Address;
    P.p_2 = (unsigned long)&res;
    
    ioctl(HWRW_FD, HWRW_IOC_MEM_READ_32, &P);

    return res;
}



/*--------------------------------------------------------------------------------------*/
/* Function: PcMemWriteB                                                                */
/*                                                                                      */
/* Parameters:   Address - The physical address in the PC to write into                 */
/*               Data    - Data to write                                                */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Write one byte to PC linear memory                                     */
/*--------------------------------------------------------------------------------------*/

void PcMemWriteB(unsigned long Address, unsigned char Data)
{
    GeneralParam_T P;
    
    P.p_1 = Address;
    P.p_2 = Data;
    
    ioctl(HWRW_FD, HWRW_IOC_MEM_WRITE_8, &P);
}



/*--------------------------------------------------------------------------------------*/
/* Function: PcMemWriteW                                                                */
/*                                                                                      */
/* Parameters:   Address - The physical address in the PC to write into                 */
/*               Data    - Data to write                                                */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Write one word to PC linear memory                                     */
/*--------------------------------------------------------------------------------------*/

void PcMemWriteW(unsigned long Address, unsigned short Data)
{
    GeneralParam_T P;
    
    P.p_1 = Address;
    P.p_2 = Data;
    
    ioctl(HWRW_FD, HWRW_IOC_MEM_WRITE_16, &P);
}


/*--------------------------------------------------------------------------------------*/
/* Function: PcMemWriteD                                                                */
/*                                                                                      */
/* Parameters:   Address - The physical address in the PC to write into                 */
/*               Data    - Data to write                                                */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Write one double word to PC linear memory                              */
/*--------------------------------------------------------------------------------------*/

void PcMemWriteD(unsigned long Address, unsigned long Data)
{
    GeneralParam_T P;
    
    P.p_1 = Address;
    P.p_2 = Data;
    
    ioctl(HWRW_FD, HWRW_IOC_MEM_WRITE_32, &P);
}


/*--------------------------------------------------------------------------------------*/
/* Function: PcMemWrite                                                                 */
/*                                                                                      */
/* Parameters:   Address   - The PC physical memory address to write into               */
/*               NoOfBytes - total number of bytes to write                             */
/*               UnitSize  - The unit size access that will be used                     */
/*                           (1-BYTE, 2-WORD, 4-DWORD)                                  */
/*               Data      - Pointer to the buffer that stores the block to write       */
/*                                                                                      */
/* Returns:      1 - On success                                                         */
/*               0 - On error                                                           */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Write a block to PC physical memory                                    */
/*--------------------------------------------------------------------------------------*/

int PcMemWrite (unsigned long Address, unsigned long NumOfBytes, int UnitSize, const void* Data)
{
    unsigned long         i          = 0;
    unsigned char const  *ByteDataP  = Data;
    unsigned short const *WordDataP  = Data;
    unsigned long const  *DWordDataP = Data;


    
    for (i = 0; i < NumOfBytes; i += UnitSize)
    {
        switch(UnitSize)
        {
        case 1:
            //*(unsigned char *)(Address + i) = *ByteDataP;
            PcMemWriteB(Address+i, *ByteDataP);
            ByteDataP++;
            break;
        case 2:
            //*(unsigned short *)(Address + i) = *WordDataP;
            PcMemWriteW(Address+i, *WordDataP);
            WordDataP++;
            break;
        case 4:
            //*(unsigned long *)( Address + i)= *DWordDataP;
            PcMemWriteD(Address+i, *DWordDataP);
            DWordDataP++;
            break;
        default:
            return 0;
        }
    }
    return 1;
}


/******************************************************************************
* Function: PcIoReadB
*          
* Purpose:  Read one byte from PC io port
*           
* Params:   Address - the io port address in the PC to read from
*           
* Returns:  the value read
*           
******************************************************************************/
unsigned char PcIoReadB( unsigned long Address )
{
    GeneralParam_T P;
    unsigned char res;
    
    P.p_1 = Address;
    P.p_2 = (unsigned long)&res;
    
    ioctl(HWRW_FD, HWRW_IOC_IO_READ_8, &P);

    return res;
}

/******************************************************************************
* Function: PcIoReadW
*           
* Purpose:  Read one word from PC io port
*           
* Params:   Address - the io port address in the PC to read from
*           
* Returns:  the value read
*           
******************************************************************************/
unsigned short PcIoReadW( unsigned long Address )
{
    GeneralParam_T P;
    unsigned short res;
    
    P.p_1 = Address;
    P.p_2 = (unsigned long)&res;
    
    ioctl(HWRW_FD, HWRW_IOC_IO_READ_16, &P);

    return res;
}

/******************************************************************************
* Function: PcIoReadD
*           
* Purpose:  Read one double word from PC io port
*           
* Params:   Address - the io port address in the PC to read from
*           
* Returns:  the value read
*           
******************************************************************************/
unsigned long PcIoReadD( unsigned long Address )
{
    GeneralParam_T P;
    unsigned long res;
    
    P.p_1 = Address;
    P.p_2 = (unsigned long)&res;
    
    ioctl(HWRW_FD, HWRW_IOC_IO_READ_32, &P);

    return res;
}

/******************************************************************************
* Function: PcIoWriteB
*           
* Purpose:  Write one byte to PC io port
*           
* Params:   Address - the io port address in the PC to write into
*           
******************************************************************************/
void PcIoWriteB( unsigned long Address, unsigned char Data )
{
    GeneralParam_T P;
    
    P.p_1 = Address;
    P.p_2 = Data;
    
    ioctl(HWRW_FD, HWRW_IOC_IO_WRITE_8, &P);
}

/*--------------------------------------------------------------------------------------*/
/* Function: PcIoWriteW                                                                 */
/*                                                                                      */
/* Parameters:   Address - The io port address in the PC to write into                  */
/*               Data    - Data to write                                                */
/*                                                                                      */
/* Returns:      1 - On success                                                         */
/*               0 - On error                                                           */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Write one word to PC io port                                           */
/*--------------------------------------------------------------------------------------*/

void PcIoWriteW (unsigned long Address, unsigned short Data)
{
    GeneralParam_T P;
    
    P.p_1 = Address;
    P.p_2 = Data;
    
    ioctl(HWRW_FD, HWRW_IOC_IO_WRITE_16, &P);
}

/*--------------------------------------------------------------------------------------*/
/* Function: PcIoWriteD                                                                 */
/*                                                                                      */
/* Parameters:   Address - The io port address in the PC to write into                  */
/*               Data    - Data to write                                                */
/*                                                                                      */
/* Returns:      1 - On success                                                         */
/*               0 - On error                                                           */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Write one double word to PC io port                                    */
/*--------------------------------------------------------------------------------------*/
void PcIoWriteD (unsigned long Address, unsigned long Data)
{
    GeneralParam_T P;
    
    P.p_1 = Address;
    P.p_2 = Data;
    
    ioctl(HWRW_FD, HWRW_IOC_IO_WRITE_32, &P); 
}

/*--------------------------------------------------------------------------------------*/
/* Function: PcIoWrite                                                                  */
/*                                                                                      */
/* Parameters:   Address   - The PC IO port address to write into                       */
/*               NoOfBytes - total number of bytes to write                             */
/*               UnitSize  - The unit size access that will be used                     */
/*                           (1-BYTE, 2-WORD, 4-DWORD)                                  */
/*               Data      - Pointer to the buffer that stores the block to write       */
/*                                                                                      */
/* Returns:      1 - On success                                                         */
/*               0 - On error                                                           */
/*                                                                                      */
/* Side effects: None                                                                   */
/*                                                                                      */
/* Description:                                                                         */
/*               Write a block to PC IO port                                            */
/*--------------------------------------------------------------------------------------*/

int PcIoWrite (unsigned long Address, unsigned long NumOfBytes, int UnitSize, const void* Data)
{
    unsigned long         i          = 0;
    unsigned char const  *ByteDataP  = Data;
    unsigned short const *WordDataP  = Data;
    unsigned long const  *DWordDataP = Data;


    
    for (i = 0; i < NumOfBytes; i += UnitSize)
    {
        switch(UnitSize)
        {
        case 1:
            PcIoWriteB(Address + i, *ByteDataP);
            ByteDataP++;
            break;
            
        case 2:
            PcIoWriteW(Address + i, *WordDataP);
            WordDataP++;
            break;
            
        case 4:
            PcIoWriteD(Address + i, *DWordDataP);
            DWordDataP++;
            break;
            
        default:
            return 0;
        }
    }
    return 1;
}

