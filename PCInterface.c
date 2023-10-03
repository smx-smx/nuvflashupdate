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



/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                           INCLUDES                                            */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/
#ifdef __LINUX__
#include "PCInterface.h"
#include "OSUtils.h"
#include "FUCore.h"
#else
#include <stdio.h>
#include "PCInterface.h"
#include "OSUtils.h"
#include "FUCore.h"
#endif


/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                          DEFINITIONS                                          */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

#define SMI_REG_GLOBAL          0
#define SMI_REG_LEGACY_USB      3
#define SMI_REG_LEGACY_USB2     17

/* Mask for Boot BIOS Straps (BBS) field in General Control and Status Register (GCS) */
#define GCS_BBS_MASK            0x00000C00

#define SET_BIT(reg, bit)       ((reg)|=(0x1<<(bit)))
#define READ_BIT(reg, bit)      ((reg)&(0x1<<(bit)))
#define CLEAR_BIT(reg, bit)     ((reg)&=(~(0x1<<(bit))))
#define IS_BIT_SET(reg, bit)    (((reg)&(0x1<<(bit)))!=0)


/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                         VARIABLES                                             */ 
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

unsigned long LPC_Register             = 0;                  /* Will hold the ICH LPC register */
int           IsLPC_Register_Changed   = 0;
int           NeedToRestoePFE          = 0;

/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                      STATIC VARIABLES                                         */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

static int            m_SMI_Global_Bit       =  0;
static int            m_SMI_Legacy_USB_Bit  = 0;
static int            m_SMI_Legacy_USB2_Bit = 0;



/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                    FUNCTION DECLERATION                                       */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/                                      

static int isICH();

                                                     
/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                   FUNCTION IMPLEMENTATION                                     */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*/
/* Function: ReadPCI                                                                             */
/*                                                                                               */ 
/* Parameters:                                                                                   */
/*                                                                                               */
/* Returns:                                                                                      */ 
/*                                                                                               */ 
/* Side effects:                                                                                 */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Perform a PCI read transaction on the specified bus                             */
/*-----------------------------------------------------------------------------------------------*/

unsigned long ReadPCI(unsigned char Bus, 
                      unsigned char Device,
                      unsigned char Function, 
                      unsigned char Register)
{
    
    /*-------------------------------------------------------------------------------------------*/
    /* Config: 31; Bus: 16-23; Device: 11-15; Function: 8-10; Register: 2-7;                     */
    /*-------------------------------------------------------------------------------------------*/


    /* Index Select */

    PcIoWriteD(PCI_INDEX,
               (1 << 31) | (Bus << 16) | (Device << 11) | (Function << 8) | Register);	

 
    return (PcIoReadD(PCI_DATA));                     /* Return PCI Configuration register value */

}


/*-----------------------------------------------------------------------------------------------*/
/* Function: WritePCI                                                                            */
/*                                                                                               */ 
/* Parameters:                                                                                   */
/*                                                                                               */
/* Returns: None                                                                                 */ 
/*                                                                                               */ 
/* Side effects:                                                                                 */
/*                                                                                               */ 
/* Description:                                                                                  */
/*               Perform a PCI write transaction on the specified bus                            */
/*-----------------------------------------------------------------------------------------------*/

void WritePCI(unsigned char Bus,
              unsigned char Device,
              unsigned char Function,
              unsigned char Register,
              unsigned long Data)
{

    /*-------------------------------------------------------------------------------------------*/
    /* Config: 31; Bus: 16-23; Device: 11-15; Function: 8-10; Register: 2-7;                     */
    /*-------------------------------------------------------------------------------------------*/

    /* Index Select */

    PcIoWriteD(PCI_INDEX, 
               (1 << 31) | (Bus << 16) | (Device << 11) | (Function << 8) | Register);	


    PcIoWriteD(PCI_DATA, Data);	                    /* Write data to PCI configuration register */
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: SetToWorkAsSharedBIOS                                                               */
/*                                                                                               */
/* Parameters:                                                                                   */
/*                                                                                               */
/* Returns:  FU_RESULT                                                                           */ 
/*                                                                                               */ 
/* Side effects:                                                                                 */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Configures ICHx to use shared BIOS                                              */
/*-----------------------------------------------------------------------------------------------*/

FU_RESULT SetToWorkAsSharedBIOS()
{    
    FU_RESULT     retVal = FU_RESULT_OK;
    unsigned long LPCAddress;
    unsigned long RootComplex;
    unsigned long LPCDeviceID;
    unsigned long ICHFamily;
    unsigned long GCS_BBS_Mask = GCS_BBS_MASK;
    
    
    
    PrintText(APP_VERBOSE_PRINT, "SetToWorkAsSharedBIOS\n");
    
    LPCDeviceID = ReadPCI(0x00, 0x1F, 0x00, 0x00);           /* Get the PCI LPC Bridge Device ID */
   
    PrintText(APP_VERBOSE_PRINT, "Found ICH ID %x\n", LPCDeviceID);

    ICHFamily = LPCDeviceID & ICHX_ID_MASK;
        
    switch (ICHFamily)
    {
    
    /* ICH6 and ICH7 FWH Configuration */
    
    case ICH6X_ID_MASK:
   
    case ICH7X_ID_MASK:
    
    case ICH8X_ID_MASK:
         
    case ICH9X_ID_MASK:

    case PCH_IBEX_PEAK_ID_MASK:

        /*---------------------------------------------------------------------------------------*/
        /* Find Root Complex Register Block Base Address and add offset                          */
        /* of General Control and Status register                                                */
        /*---------------------------------------------------------------------------------------*/
        
        LPCAddress = (ReadPCI(0x00, 0x1F, 0x00, 0xF0) & 0xFFFFFFFE) + 0x3410;
        PrintText(APP_VERBOSE_PRINT, "Physical address of RCRB General Control and Status is %lx\n", LPCAddress);
        
        if (!PcMemMap(&LPCAddress, 0x4))           
        {
            PrintText(APP_ERROR_PRINT,
                  "ERROR: Could not map %d bytes of memory at %lx for PCI configuration!\n",
                  16,
                  LPCAddress);
          
            return FU_ERR_MEMORY_MAP_FAILED;
        }
        else
        {
            PrintText(APP_VERBOSE_PRINT, "LPCAddress mapped to %lx\n", LPCAddress);                    
        }
        
        RootComplex = PcMemReadD(LPCAddress);
        PrintText(APP_VERBOSE_PRINT, "RootComplex  General Control and Status value is %lx\n", RootComplex);

        LPC_Register = PcMemReadD(LPCAddress);                     /* Backuping the LPC register */
        IsLPC_Register_Changed = 1;

        if(PCH_IBEX_PEAK_ID_MASK == ICHFamily)
        {
            GCS_BBS_Mask = ~GCS_BBS_MASK;
            PcMemWriteD(LPCAddress, RootComplex & GCS_BBS_Mask);  /* Select LPC for accesses to BIOS memory range */
            RootComplex = PcMemReadD(LPCAddress);                    
            PrintText(APP_VERBOSE_PRINT, "RootComplex value is now %lx should be %lx\n", RootComplex, RootComplex & GCS_BBS_Mask);
        }
        else
        {
            PcMemWriteD(LPCAddress, RootComplex | GCS_BBS_Mask);  /* Select LPC for accesses to BIOS memory range */
            RootComplex = PcMemReadD(LPCAddress);                    
            PrintText(APP_VERBOSE_PRINT, "RootComplex value is now %lx should be %lx\n", RootComplex, RootComplex | GCS_BBS_Mask);
        }
    
    }

    return retVal;
    
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: LPCConfigure                                                                        */
/*                                                                                               */ 
/* Parameters: SHM_interface - Shared nenory type                                                */
/*                                                                                               */
/* Returns:  FU_RESULT                                                                           */ 
/*                                                                                               */ 
/* Side effects:                                                                                 */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Configures ICHx LPC registers to enable addresses needed for                    */
/*               shared memory transactions                                                      */
/*-----------------------------------------------------------------------------------------------*/

FU_RESULT LPCConfigure(int SHM_interface)
{
    FU_RESULT     retVal = FU_RESULT_OK;
    unsigned char c6f;
    unsigned long LPCAddress;
    unsigned long ICHFamily;
    unsigned long RootComplex;
    unsigned long LPCDeviceID;
    unsigned long GCS_BBS_Mask = GCS_BBS_MASK;


    LPCDeviceID = ReadPCI(0x00, 0x1F, 0x00, 0x00);           /* Get the PCI LPC Bridge Device ID */

    PrintText(APP_VERBOSE_PRINT, "Found ICH ID %x\n", LPCDeviceID);    

    ICHFamily = LPCDeviceID & ICHX_ID_MASK;

    switch (ICHFamily)
    {

    /* ICH6 and ICH7 FWH Configuration */

    case ICH6X_ID_MASK:

    case ICH7X_ID_MASK:

    case ICH8X_ID_MASK:        

    case ICH9X_ID_MASK:

    case PCH_IBEX_PEAK_ID_MASK:
        
        if (SHM_interface == SHM_MEM_LPC)
        {
            return (0);
        }
        
        /*---------------------------------------------------------------------------------------*/
        /* Find Root Complex Register Block Base Address and add offset                          */
        /* of General Control and Status register                                                */
        /*---------------------------------------------------------------------------------------*/

        LPCAddress = (ReadPCI(0x00, 0x1F, 0x00, 0xF0) & 0xFFFFFFFE) + 0x3410;
        PrintText(APP_VERBOSE_PRINT, "Physical address of RCRB General Control and Status is %lx\n", LPCAddress);

        if (!PcMemMap(&LPCAddress, 0x4))           
        {
            PrintText(APP_ERROR_PRINT,
                  "ERROR: Could not map %d bytes of memory at %lx for PCI configuration!\n",
                  16,
                  LPCAddress);
          
            return FU_ERR_MEMORY_MAP_FAILED;
        }
        else
        {
            PrintText(APP_VERBOSE_PRINT, "LPCAddress mapped to %lx\n", LPCAddress);                    
        }

        RootComplex = PcMemReadD(LPCAddress);
        PrintText(APP_VERBOSE_PRINT, "RootComplex  General Control and Status value is %lx\n", RootComplex);

        LPC_Register = PcMemReadD(LPCAddress);
        IsLPC_Register_Changed = 1;            
        
        if(PCH_IBEX_PEAK_ID_MASK == ICHFamily)
        {
            GCS_BBS_Mask = ~GCS_BBS_MASK;
            PcMemWriteD(LPCAddress, RootComplex & GCS_BBS_Mask);  /* Select LPC for accesses to BIOS memory range */
            RootComplex = PcMemReadD(LPCAddress);                    
            PrintText(APP_VERBOSE_PRINT, "RootComplex value is now %lx should be %lx\n", RootComplex, RootComplex & GCS_BBS_Mask);
        }
        else
        {
            PcMemWriteD(LPCAddress, RootComplex | GCS_BBS_Mask);  /* Select LPC for accesses to BIOS memory range */
            RootComplex = PcMemReadD(LPCAddress);                    
            PrintText(APP_VERBOSE_PRINT, "RootComplex value is now %lx should be %lx\n", RootComplex, RootComplex | GCS_BBS_Mask);
        }
        
        /* Configure the ICH and enable addresses FF800000h-FF900000h (1MB) */

        WritePCI(0x00,
                 0x1F,
                 0x00,
                 0xD8,
                 ReadPCI(0x00, 0x1F, 0x00, 0xD8) | 0x00000300);	    /* Enable FWH_C0_EN and FWH_C8_EN   */

        WritePCI(0x00,
                 0x1F,
                 0x00,
                 0xD0,
                 (ReadPCI(0x00, 0x1F, 0x00, 0xD0) & 0xFFFFFF0F) | (SHW_FWH_ID << 4) ); /* Set FWH_C8_IDSEL */

        WritePCI(0x00,
                 0x1F,
                 0x00,
                 0xD0,
                 (ReadPCI(0x00, 0x1F, 0x00, 0xD0) & 0xFFFFFFF0) |  SHW_FWH_ID  );      /* Set FWH_C0_IDSEL */

        PrintText(APP_VERBOSE_PRINT, "FWH Configuration completed in ICH6/7/8\n");
        
        
        break;
        

    case ICH0_5_ID_MASK:                                                /* ICH0 - ICH5 FWH Configuration */

        if (SHM_interface == SHM_MEM_LPC)
        {
            return (0);
        }


        /* Configure the ICH and enable addresses FF800000h-FF900000h (1MB) */

        WritePCI(0x00, 0x1F, 0x00, 0xE0, ReadPCI(0x00, 0x1F, 0x00, 0xE0) | 0x03000000);
        WritePCI(0x00, 0x1F, 0x00, 0xE8, (ReadPCI(0x00, 0x1F, 0x00, 0xE8) & 0xFFFFFF0F) | (SHW_FWH_ID << 4));
        WritePCI(0x00, 0x1F, 0x00, 0xE8, (ReadPCI(0x00, 0x1F, 0x00, 0xE8) & 0xFFFFFFF0) | SHW_FWH_ID);


        /* Configure the ICH and enable addresses FF900000h-FFA00000h (1MB) */

        WritePCI(0x00, 0x1F, 0x00, 0xE0, ReadPCI(0x00, 0x1F, 0x00, 0xE0) | 0x0C000000);
        WritePCI(0x00, 0x1F, 0x00, 0xE8, (ReadPCI(0x00, 0x1F, 0x00, 0xE8) & 0xFFFFF0FF) | (SHW_FWH_ID << 8));
        WritePCI(0x00, 0x1F, 0x00, 0xE8, (ReadPCI(0x00, 0x1F, 0x00, 0xE8) & 0xFFFF0FFF) | (SHW_FWH_ID << 12));

        PrintText(APP_VERBOSE_PRINT, "FWH Configuration completed in ICH0/2/4/5\n");
        
        break;
        
    default:                                                        /* None-ICHx I/O Controllers */

        LPCDeviceID = ReadPCI(0x00, 20, 0x3, 0x02);                 /* ATI SB400                 */

        PrintText(APP_VERBOSE_PRINT, "Found non-ICHx I/O Controller Hub with DID %x\n", LPCDeviceID);
           
        if(SHM_interface == SHM_MEM_LPC)
        {
            PrintText(APP_VERBOSE_PRINT,"Using LPC Memory\n");

            LPCAddress = ReadPCI(0x00, 0x14, 0x03, 0x6C);
            LPCAddress &=0xFFFF0000;
            LPCAddress |= 0xFFB0;
            WritePCI(0x00, 0x14, 0x03, 0x6C, LPCAddress);
            PrintText(APP_VERBOSE_PRINT, "Configured LPC address range %lx\n", LPCAddress);           
        }

        WritePCI(0x00,
                 20,
                 0x3,
                 0x70,
                 (ReadPCI(0x00, 20, 0x3, 0x70) & 0xF000000F) | 0x0eeeeee0);      /* FWH IDSEL        */

        WritePCI(0x00, 20, 0x0, 0x40, ReadPCI(0x00, 20, 0x0, 0x40) | 0x3300);  /* FWH IDSEL        */
        WritePCI(0x00, 20, 0x0, 0x78, ReadPCI(0x00, 20, 0x0, 0x78) | 0x100);   /* C6F enable       */

        c6f = PcIoReadB(0xC6F);

        PcIoWriteB(0x80, 0x5a);                                                /* Delay            */
        PcIoWriteB(0x80, 0xa5);                                                /* Delay            */
        PcIoWriteB(0xC6F, (unsigned char)(c6f | 0x40));                        /* Flash ROM enable */

        PrintText(APP_VERBOSE_PRINT, "Configured FWH IDSEL\n");
        
        break;
    }

    return retVal;
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: IOConfigure                                                                         */
/*                                                                                               */ 
/* Parameters: None                                                                              */
/*                                                                                               */
/* Returns:    1 on success                                                                      */ 
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description: Configure IO                                                                     */ 
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

int IOConfigure()
{

    /* Get the PCI LPC Bridge Device ID */

    unsigned long LPCDeviceID = ReadPCI(0x00, 0x1F, 0x00, 0x00);


    switch (LPCDeviceID & ICHX_ID_MASK)
    {


    /* ICH6 and ICH7 FWH Configuration */

    case ICH6X_ID_MASK:

    case ICH7X_ID_MASK:

    case ICH8X_ID_MASK:

        /* Configure Generic Decode Range 2 to be forwarded to the LPC i/f */

        WritePCI(0x00, 0x1F, 0x00, 0x88, 0xfc164d);
        
        /* Configure Generic Decode Range 4 to be forwarded to the LPC i/f */
        /* WritePCI(0x00, 0x1F, 0x00, 0x90, 0xfc07f1); */
        
        break;
        
    case ICH0_5_ID_MASK:                                         /* ICH0 - ICH5 FWH Configuration */

        break;
        
    default:                                                     /* None-ICHx I/O Controllers     */
        
        break;
    }

    return(1);
}



/*-----------------------------------------------------------------------------------------------*/
/* Function:     isICH                                                                           */
/*                                                                                               */ 
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      0 - Not ICH                                                                     */ 
/*               1 - ICH exist                                                                   */ 
/*                                                                                               */
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:  Check to see whether ICH exists                                                 */ 
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

static int isICH()
{
    unsigned long LPCDeviceID = 0;
    int           l_result    = 0;
    
         

    LPCDeviceID = ReadPCI(0x00, 0x1F, 0x00, 0x00);           /* Get the PCI LPC Bridge Device ID */
                      
    switch (LPCDeviceID & ICHX_ID_MASK)
    {                
        case ICH6X_ID_MASK:         
        case ICH7X_ID_MASK:          
        case ICH8X_ID_MASK:               
        case ICH9X_ID_MASK:
        case PCH_IBEX_PEAK_ID_MASK:
            l_result = 1;
            break;
            
        default:
            break;
    }

    return l_result;    
}


/*-----------------------------------------------------------------------------------------------*/
/* Function:     DisableSMILegacyUSB                                                             */
/*                                                                                               */ 
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      FU_RESULT                                                                       */ 
/*                                                                                               */ 
/* Side effects: Sets m_legacyUSBDisabled and m_SMIreg                                           */
/*                                                                                               */ 
/* Description:  Disable the legacy USB keyboard                                                 */ 
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

FU_RESULT DisableSMILegacyUSB()
{
    FU_RESULT     l_retVal       = FU_RESULT_OK;
    unsigned long l_baseAddress  = 0;     
    unsigned long l_smiRegPrev   = 0;
    unsigned long l_smiReg       = 0;
    


    if (isICH())
    {
        PrintText(APP_VERBOSE_PRINT, "Disabling SMI legacy USB (Bits 3 and 17)");

        l_baseAddress = ReadPCI(0x00, 0x1F, 0x00, 0x40) - 0x01;       /* Reading PM base address */

        l_smiReg = PcIoReadD(l_baseAddress + 0x30);                   /* Read the SMI register   */   
        l_smiRegPrev = l_smiReg;
        m_SMI_Legacy_USB_Bit = IS_BIT_SET(l_smiReg, SMI_REG_LEGACY_USB);
        m_SMI_Legacy_USB2_Bit = IS_BIT_SET(l_smiReg, SMI_REG_LEGACY_USB2);

        CLEAR_BIT(l_smiReg, SMI_REG_LEGACY_USB);
        CLEAR_BIT(l_smiReg, SMI_REG_LEGACY_USB2);
        
        PcIoWriteD(l_baseAddress + 0x30, l_smiReg);

        PrintText(APP_VERBOSE_PRINT,
                  " from %d %d to 0 0, SMI register: %x (was %x)\n",
                  m_SMI_Legacy_USB_Bit,
                  m_SMI_Legacy_USB2_Bit,
                  l_smiReg,
                  l_smiRegPrev);
          
    }
        

    return l_retVal;    
}

/*-----------------------------------------------------------------------------------------------*/
/* Function:     RestoreSMILegacyUSB                                                             */
/*                                                                                               */ 
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      FU_RESULT                                                                       */ 
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:  Restore the Legacy USB/2 SMI bits in the SMI register                           */
/*-----------------------------------------------------------------------------------------------*/

FU_RESULT RestoreSMILegacyUSB()
{
    FU_RESULT     l_retVal       = FU_RESULT_OK;
    unsigned long l_baseAddress = 0;
    unsigned long l_smiReg       = 0;
     


    if (m_SMI_Legacy_USB_Bit || m_SMI_Legacy_USB2_Bit)
    {
        PrintText(APP_VERBOSE_PRINT,
                  "Restoring SMI legacy USB(Bits 3 and 17) to %d %d\n",
                  m_SMI_Legacy_USB_Bit,
                  m_SMI_Legacy_USB2_Bit);
        
        l_baseAddress = ReadPCI(0x00, 0x1F, 0x00, 0x40) - 0x01;       /* Reading PM base address */
        
        l_smiReg = PcIoReadD(l_baseAddress + 0x30);                   /* Read the SMI register   */

        if (m_SMI_Legacy_USB_Bit)
        {
            SET_BIT(l_smiReg, SMI_REG_LEGACY_USB);
        }
        if (m_SMI_Legacy_USB2_Bit)
        {
            SET_BIT(l_smiReg, SMI_REG_LEGACY_USB2);
        }
        
        PcIoWriteD(l_baseAddress + 0x30, l_smiReg);       
    }
    

    return l_retVal;    
}


/*-----------------------------------------------------------------------------------------------*/
/* Function:     DisableGlobalSMI                                                                */
/*                                                                                               */ 
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      FU_RESULT                                                                       */ 
/*                                                                                               */ 
/* Side effects: Sets m_legacyUSBDisabled and m_SMIreg                                           */
/*                                                                                               */ 
/* Description:  Disable the legacy USB keyboard                                                 */ 
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

FU_RESULT DisableGlobalSMI()
{
    FU_RESULT     l_retVal       = FU_RESULT_OK;
    unsigned long l_baseAddress  = 0; 
    unsigned long l_smiRegPrev   = 0;
    unsigned long l_smiReg       = 0;        



    if (isICH())
    {
        PrintText(APP_VERBOSE_PRINT, "Disabling Global SMI (Bit 0)");

        l_baseAddress = ReadPCI(0x00, 0x1F, 0x00, 0x40) - 0x01;       /* Reading PM base address */

        l_smiReg = PcIoReadD(l_baseAddress + 0x30);                   /* Read the SMI register   */
        l_smiRegPrev = l_smiReg;
        m_SMI_Global_Bit = IS_BIT_SET(l_smiReg, SMI_REG_GLOBAL);
        CLEAR_BIT(l_smiReg, SMI_REG_GLOBAL);
        PcIoWriteD(l_baseAddress + 0x30, l_smiReg);

        PrintText(APP_VERBOSE_PRINT,
                  " from %d to 0, SMI register: %x (was %x)\n",
                  m_SMI_Global_Bit,
                  l_smiReg,
                  l_smiRegPrev);
    }
        

    return l_retVal;    
}


/*-----------------------------------------------------------------------------------------------*/
/* Function:     RestoreGlobalSMI                                                                */
/*                                                                                               */ 
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      FU_RESULT                                                                       */ 
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:  Restore the global SMI bit in the SMI register                                  */
/*-----------------------------------------------------------------------------------------------*/

FU_RESULT RestoreGlobalSMI()
{
    FU_RESULT     l_retVal       = FU_RESULT_OK;
    unsigned long l_baseAddress  = 0;
    unsigned long l_smiReg       = 0;



    if (m_SMI_Global_Bit)
    {
        PrintText(APP_VERBOSE_PRINT, "Restoring Global SMI  USB (Bit 0) to 1\n");
        
        l_baseAddress = ReadPCI(0x00, 0x1F, 0x00, 0x40) - 0x01;       /* Reading PM base address */
        
        l_smiReg = PcIoReadD(l_baseAddress + 0x30);                   /* Read the SMI register   */
        SET_BIT(l_smiReg, SMI_REG_GLOBAL);
        PcIoWriteD(l_baseAddress + 0x30, l_smiReg);           
    }
    

    return l_retVal;    
}


/*-----------------------------------------------------------------------------------------------*/
/* Function:     DisablePFEMenlow                                                                */
/*                                                                                               */ 
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      None                                                                            */ 
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:  Disable BIOS prefetching on Nenlow platform                                     */
/*-----------------------------------------------------------------------------------------------*/

void DisablePFEMenlow()
{    
    unsigned long LPCDeviceID;
    unsigned long BIOSCtl;
    
    LPCDeviceID = ReadPCI(0x00, 0x1F, 0x00, 0x00);           /* Get the PCI LPC Bridge Device ID */    
    
    // check if MENLOW platform
    if (LPCDeviceID == MENLOW_ICH_ID)
    {
        PrintText(APP_VERBOSE_PRINT, "MENLOW ICH ID %x\n", LPCDeviceID);
        
        // read BIOS_CTL (D8) register
        BIOSCtl = ReadPCI(0x00, 0x1F, 0x00, 0xD8);

        // check PFE bit (bit 8)
        if (BIOSCtl & 0x00000100)
        {
            // disable BIOS prefetching - clear PFE bit (bit 8) in BIOS_CTL (D8) register
            PrintText(APP_VERBOSE_PRINT, "Disabling MENLOW PFE \n");
            // modify
            BIOSCtl &= 0xFFFFFEFF; // clear PFE bit (bit 8)

            NeedToRestoePFE = 1;
        }        

        // write back
        WritePCI(0x00, 0x1F, 0x00, 0xD8, BIOSCtl);
    }
        
}


/*-----------------------------------------------------------------------------------------------*/
/* Function:     RestorePFEMenlow                                                                */
/*                                                                                               */ 
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      None                                                                            */ 
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:  Restore/set BIOS prefetching on Nenlow platform                                 */
/*-----------------------------------------------------------------------------------------------*/

void RestorePFEMenlow()
{        
    unsigned long BIOSCtl;
    
    PrintText(APP_VERBOSE_PRINT, "Restoring MENLOW PFE \n");
        
    // read BIOS_CTL (D8) register
    BIOSCtl = ReadPCI(0x00, 0x1F, 0x00, 0xD8);

    // set PFE bit (bit 8)
    BIOSCtl |= 0x00000100;        
            
    // write back
    WritePCI(0x00, 0x1F, 0x00, 0xD8, BIOSCtl);

    NeedToRestoePFE = 0;
}


