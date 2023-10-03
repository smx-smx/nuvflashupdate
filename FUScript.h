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



#ifndef FUSCRIPT_HEADER 
#define FUSCRIPT_HEADER

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                      INCLUDES                                        */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FUCore.h"


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                     DEFINITIONS                                      */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

#define NUM_OF_SCR_INT_REGISTERS    3
#define MAX_INTERRUPT_VALUE         255
#define DEFAULT_INT_CMD             21                  /*  21 (Decimal) = 0x15*/
#define ILLEGAL_REGISTER            -1


typedef struct RegisterAndValue
{    
    unsigned char          Register;    
    unsigned short         Value;
        
} RegisterAndValue;

typedef struct PciMemIO_CmdParams
{    
    int                     Cmd;
    int                     Width;
    unsigned char           Bus;
    unsigned char           Device;        
    unsigned char           Function;
    unsigned char           Register;
#ifdef __LINUX__    
    unsigned int            Address;
    unsigned int            Data;
    unsigned int            Mask;
#else
    unsigned long           Address;
    unsigned long           Data;
    unsigned long           Mask;
#endif
    
} PciMemIO_CmdParams;

typedef struct Int_CmdParams    
{
    int                     Cmd;
    int                     Interrupt;
    RegisterAndValue        RegisterAndValues[NUM_OF_SCR_INT_REGISTERS];
} Int_CmdParams;

typedef struct Delay_CmdParams    
{
    int                     Cmd;
    unsigned long           Delay;
} Delay_CmdParams;

    
typedef union CmdParameters
{
    
    PciMemIO_CmdParams      PciMemIO_CmdParams ;
    Int_CmdParams           Int_CmdParams;
    Delay_CmdParams         Delay_CmdParams;

} CmdParameters;


typedef enum
{
    SCR_BEFORE_SECTION,
    SCR_AFTER_SECTION,
    SCR_MAX_NUM_OF_SECTIONS
} SCR_TYPE;

typedef enum
{    
    CMD_INVALID_START,
    CMD_MEMW,
    CMD_IOW,
    CMD_PCIW,
    CMD_INT,
    CMD_DELAY,
    CMD_INVALID_END
} CMD_TYPE;

typedef struct {

     char const *keyword;
     CMD_TYPE   cmdType;
     FU_RESULT (* func)(CMD_TYPE cmdType,
                        char* scrFilePtr,
                        size_t scrFileSize,
                        CmdParameters *p_cmdParameters);
     int size;

} scrKeyword_t;


typedef enum
{    
    FU_PRE_FU,
    FU_POST_FU
} FU_STAGE;

typedef enum {
    SCR_CMD_PARAMS_WIDTH,
    SCR_CMD_PARAMS_BUS,
    SCR_CMD_PARAMS_DEVICE,
    SCR_CMD_PARAMS_FUNCTION,
    SCR_CMD_PARAMS_REGISTER,
    SCR_CMD_PARAMS_ADDRESS,
    SCR_CMD_PARAMS_DATA,
    SCR_CMD_PARAMS_MASK,
    SCR_CMD_PARAMS_INT_NUM,
    SCR_CMD_PARAMS_REGS_AND_VALUES,
    SCR_CMD_PARAMS_DELAY,
    SCR_CMD_PARAMS_DONE,
    SCR_CMD_PARAMS_ERROR
} SCR_CMD_PARAMS_STATE;


#define NUM_OF_SCR_KEYWORDS (CMD_INVALID_END-1)
#define CMD_PARAMS_ARRAY_SIZE 40

FU_RESULT  SCR_Init();
FU_RESULT  SCR_ExtractData(char* tsfName, char* bsfName );
FU_RESULT  SCR_RunCommands(int p_section);
FU_RESULT  SCR_ReadBSF(char* bsfName);
FU_RESULT  SCR_WriteBSF(char* tsfName);
void       SCR_SetCommandsExecution(BOOLEAN p_commandsExecution);




#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif

