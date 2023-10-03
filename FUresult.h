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



#ifndef FURESULT_HEADER
#define FURESULT_HEADER

#ifndef MAX_MSG_LENGTH
#define MAX_MSG_LENGTH                          200              /* The maximum charecters allowed in a message*/
#endif

#ifdef WIN32
    #include <windows.h>
#else
    #ifndef BOOLEAN
        typedef int BOOLEAN;
        #define TRUE 1
        #define FALSE 0
    #endif
#endif

/* All errors in Flash Update core library should be from this type */

typedef enum
{
    
    FU_RESULT_OK,                                   /* Result OK */
    FU_RESULT_USER_ABORTED,                         /* User abort the update opertaion */
    FU_RESULT_NO_BIOS_MODEL,
    FU_RESULT_OLDER_BIN_FILE_BIOS,
    
    FU_ERR_FLASH_CONFIG_FILE_TOO_SMALL,             /* Flash configuration file is too small */
    FU_ERR_UNEXPECTED_FLASH_ID,                     /* The detected flash ID is diffrent from the flash configuration file */
    FU_ERR_DOWNLOADING_TO_FLASH,                    /* Failed download image to flash */
    FU_ERR_MEMORY_MAP_FAILED,                       /* The memory map process have failed */
    FU_ERR_FIRMWARE_ERROR,                          /* Unexpected error on the firmware side */
    FU_ERR_SHM_NOT_CONFIG,                          /* The shared memory hasn't been configured */
    FU_ERR_ILLEGAL_FLASH_SIZE,                      /* Illegal flash size */
    FU_ERR_BASE_ADDRESS_NOT_ALIGNED,                /* The given base address is not aligned */
    FU_ERR_DEVICE_NOT_FOUND,                        /* The flash device wasn't found */
    FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES,       /* At least one of the flash configuration values is not legal*/
    FU_ERR_FAIL_PROGRAMING_ADDRESS,                 /* Failed to program a specific address */
    FU_ERR_TO_MUCH_BYTES_TO_PROGRAM,                /* To much bytes to program in the shared memory program function */
    FU_ERR_CANT_FIND_CORRECT_FLASH_CONFIGURATION,   /* Can't find flash configuration that matches the current flash */
    FU_ERR_GIVEN_NULL_POINTER,    
    FU_ERR_MISSING_BIN_FILE_BIOS_MODEL,
    FU_ERR_MISSING_FLASH_BIOS_MODEL,
    FU_ERR_DIFFERENT_BIOS_MODEL,
    FU_ERR_MISSING_BIN_FILE_BIOS_VERSION,
    FU_ERR_MISSING_FLASH_BIOS_VERSION,
    FU_ERR_TRY_TO_READ_FROM_PROTECTED_AREA,
    FU_ERR_OFFSET_DONT_DESCRIBE_A_SECTOR_START,
    FU_ERR_SCR_INVALID_SECTION,                     /* Invalid or missing section was provided for SCR parser */    
    FU_ERR_SCR_INVALID_COMMAND,                     /* Invalid SCR command was provided */        
    FU_ERR_SCR_FILE_OPEN_ERROR,                     /* Couldn't open Script file */                
    FU_ERR_SCR_INVALID_TSF_NAME_ERROR,              /* Invalid TSF name (extension != ".tsf") */                
    FU_ERR_SCR_SYNTAX_ERROR,                        /* A syntax error in Script file */
    FU_ERR_SCR_COMMAND_NOT_SUPPORTED_IN_x64,        /* The CHC command is not supported in x64 systems */
    FU_ERR_CUSTOM_MSG_SYNTAX_ERROR,                 /* A syntax error in the Custom messages file*/
    FU_ERR_CUSTOM_MSG_TOO_LONG,                     /* A custom message cannot exceed MAX_MSG_LENGTH*/
    
    FU_ERR_UNEXPECTED_ERROR,                         /* A general unexpcted error message */
    FU_MAX_RESULT                                   /* Maximum error\result number */

} FU_RESULT;

typedef struct FuMessage
{
    const FU_RESULT     enumAsInt;
    const char*         enumAsStr;
    char                messageText[MAX_MSG_LENGTH];    
} FuMessage;

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                     DEFINITIONS                                      */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

char *GetFUErrorString(FU_RESULT p_fuResult);

FU_RESULT updateFuMessageTable(char* p_strEnum,char*  p_message, BOOLEAN toUpdate);

const char* getFUEnumAsString(int p_enum);

int verifyFuArrayConsistency();
#endif

