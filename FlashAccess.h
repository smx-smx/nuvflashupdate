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



#ifndef FLASH_ACCESS_HEADER 
#define FLASH_ACCESS_HEADER

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                      INCLUDES                                        */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

#include "OSUtils.h"

#ifdef __cplusplus
extern "C" {
#endif


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                    DEFINITIONS                                       */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

typedef enum
{
    PROGRAM_THROUGH_IO,
    PROGRAM_THROUGH_MEMORY
    
} FLASH_ACCESS_TYPE;

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                FUNCTION DECLERATIONS                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


void SetFlashAccess(FLASH_ACCESS_TYPE p_accessType);
FLASH_ACCESS_TYPE GetFlashAccessType();

unsigned char Pc2FlashReadB (unsigned long Address);
unsigned short Pc2FlashReadW (unsigned long Address);
unsigned long Pc2FlashReadD (unsigned long Address);

void Pc2FlashWriteB(unsigned long Address, unsigned char Data);
void Pc2FlashWriteW(unsigned long Address, unsigned short Data);
void Pc2FlashWriteD(unsigned long Address, unsigned long Data);
int  Pc2FlashWrite (unsigned long Address, unsigned long NumOfBytes, int UnitSize, const void* Data);

#ifdef __cplusplus
}
#endif


#endif

