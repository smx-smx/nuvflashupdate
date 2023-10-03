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



#ifndef _PCInterface_h_
#define _PCInterface_h_


#include "FUresult.h"

#ifdef __cplusplus
extern "C" {
#endif



/*--------------------------------------------------------------------------------------*/
/*                                  ICH constants                                       */
/*--------------------------------------------------------------------------------------*/

#define ICH0_ID             0x24108086	        /* Chaintec (ICH0)                      */
#define ICH2_ID             0x24408086	        /* Tyan & Asus (ICH2)                   */
#define ICH4_ID             0x24C08086	        /* Brownsville (ICH4)                   */
#define ICH5_ID             0x24D08086	        /* Bayfield (ICH5)                      */
#define ICH6_ID             0x26408086	        /* Eatonville (ICH6)                    */
#define ICH6M_ID            0x26418086	        /* Ohlone (ICH6M)                       */
#define ICH7_ID             0x27B88086	        /* ICH7                                 */
#define ICH7M_ID            0x27B98086	        /* Capell Valley (ICH7M)                */
#define ICH7M_DH_ID         0x27BD8086	        /* Capell Valley too (ICH7M DH)         */

#define MENLOW_ICH_ID       0x81198086


/*--------------------------------------------------------------------------------------*/
/*                                  ICH ID masks                                        */
/*--------------------------------------------------------------------------------------*/

#define ICHX_ID_MASK           0xFF00FFFF
#define ICH0_5_ID_MASK         0x24008086
#define ICH6X_ID_MASK          0x26008086
#define ICH7X_ID_MASK          0x27008086
#define ICH8X_ID_MASK          0x28008086    
#define ICH9X_ID_MASK          0x29008086    
#define PCH_IBEX_PEAK_ID_MASK  0x3B008086    


#define PCI_INDEX           0xCF8
#define PCI_DATA            0xCFC

#define SHW_FWH_ID          0xE                 /* Firmware Memory ID                   */


    
/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                Function Decleration                                  */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


unsigned long ReadPCI(unsigned char Bus,
                      unsigned char Device, 
                      unsigned char Function, 
                      unsigned char Register);

void          WritePCI(unsigned char Bus,
                       unsigned char Device, 
                       unsigned char Function, 
                       unsigned char Register,
                       unsigned long Data);


FU_RESULT     LPCConfigure(int SHM_interface);
FU_RESULT     SetToWorkAsSharedBIOS();

void          DisablePFEMenlow();
void          RestorePFEMenlow();

int           IOConfigure();

FU_RESULT     DisableSMILegacyUSB();
FU_RESULT     RestoreSMILegacyUSB();

FU_RESULT     DisableGlobalSMI();
FU_RESULT     RestoreGlobalSMI();
    


#ifdef __cplusplus
}                                                       /* Closing brace for extern "C" */
#endif

#endif                                                  /* _PCInterface_h_              */

