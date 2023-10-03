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



#ifndef FLASH_UPDATE_TOOL_APP_HEADER
#define FLASH_UPDATE_TOOL_APP_HEADER

/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                           INCLUDES                                            */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                     TYPE DEFINITIONS                                          */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/
#define APP_VERSION                             "4.3.2" // New Version Marker

#define MIN_FLASH_CONFIG_FILE_SIZE              30               /* The minimum size of the      */
                                                                 /* flash configuration file     */

#define DEFAULT_FLASH_CONFIG_FILE_MANE          "Flupdate.cfg"
#define DEFAULT_COMMAND_LINE_FILE               "Flupdate.cmd"
#define DEFAULT_BSF_NAME                        "Flupdate.bsf"
#ifndef MAX_MSG_LENGTH
#define MAX_MSG_LENGTH                          200              /* The maximum charecters allowed in a message*/
#endif

#ifdef WIN32
    #include <windows.h>
    #define SLEEP(X)        (Sleep(X));
#endif
#ifdef DOS
    #include <i86.h>
    #define SLEEP(X)        (delay(X));
#endif
#ifdef __LINUX__
    #include <unistd.h>
    #define SLEEP(X)        (usleep(X));
#endif


#include "FUresult.h"


typedef struct AppOptions
{
    char                    ConfigFileName[FILENAME_MAX];
    char                    BinaryFileName[FILENAME_MAX];
    char                    DumpFileName[FILENAME_MAX];
    char                    AppExeName[FILENAME_MAX];
    char                    TsfName[FILENAME_MAX];
    char                    BsfName[FILENAME_MAX];
    char                    CommandLine[256];
    
    /* Command line flags */
    
    int                     ShowUsage;
    int                     IgnoreVersion;
    int                     IgnoreSystem;
    int                     NoPause;
    int                     NoReboot;
    int                     WipeClean;
    int                     WipeAll;
    int                     IgnoreSpecialSections;                   /* -ForceSections flag      */
    int                     Dump;
    int                     DumpOnly;
    int                     Compare;
    int                     IgnoreProtectedSectors;
    int                     Verbose;
    int                     DisableLegacyUSB;
    int                     DisableGlobalSMI;
    int                     ChooseBaseAddress;
    int                     IgnoreBIOSModelCheck;
    int                     FlashOffset;
    int                     Silent;
    short                   VideoMode;
    
    
}AppOptions;


typedef enum
{
    
    APP_RESULT_OK,                                  /* Result OK */
    APP_RESULT_OK_SHOW_STATUS,                      /* Result OK, status is sent to the GUI*/        
    APP_RESULT_USER_ABORTED,                        /* User abort some operation */

    APP_ERR_FLASH_BIN_NOT_SPECIFIED,                /* The flash binary file wasn't specified in the command line */
    APP_ERR_EMPTY_FLASH_BIN_FILE,                   /* The given binary image file is empty */    
    APP_ERR_SMALL_FLASH_CONFIG_FILE,                /* The give flash configuration file is too small */
    APP_ERR_CREATING_DEFAULT_CFG_FILE,              /* Ubnable to create default flash configuration file */
    APP_ERR_NO_FLASH_CONFIG_FILE,                   /* When trying to open the flash configuration file*/
    APP_ERR_FAILED_COPY_DRIVER_TO_SYSTEM,           /* IO/Memory driver file could not be copy to system32 directory, it's possible that the program could not find it*/
    APP_ERR_UNRECOGNIZED_PARAMETER,                 /* Unrecognize parameter was given to the application in the command line */
    APP_ERR_ILLEGAL_EXIT_TYPE,                      /* The user specified illegal exit type */
    APP_ERR_CAN_NOT_READ_EXIT_TYPE,                 /* Can't read exit type from the command line (probably -x was given without a value)*/
    APP_ERR_FAILED_TO_OPEN_DRIVER,                  /* Can't open the IO/Memory driver */
    APP_ERR_FAILED_TO_LOAD_COMPANY_LOGO,            /* When we are trying to load the company logo bmp file */
    APP_ERR_FAILED_TO_LOAD_OEM_LOGO,
    APP_ERR_LOGO_NOT_LOADED,                        /* Can occur if accidentally try to draw the company logo without loading it first */
    APP_ERR_FILE_NOT_EXIST,
    APP_ERR_DUMP_FILE_NAME_NOT_SPECIFIED,           /* When the -d flag not followed by a file name */
    APP_ERR_FLASH_CONFIG_FILE_NOT_SPECIFIED,        /* When the -c flag not followed by a file name */
    APP_ERR_BASE_ADDRESS_NOT_SPECIFIED,             /* When the -cba flag not followed by a file name */
    APP_ERR_ILLEGAL_BASE_ADDRESS,
    APP_ERR_FAILED_TO_CREATE_FILE,
    APP_ERR_ILLEGAL_FLAG_MIXING,
    APP_ERR_DIFFERENT_BIOS_MODEL,                   /* The flash BIOS model is diffrent from the one in the binary image file */
    APP_ERR_MISSING_BIN_FILE_BIOS_MODEL,            /* In case we can not find the binary file BIOS model */
    APP_ERR_MISSING_FLASH_BIOS_MODEL,               /* In case we can not find the flash BIOS model */
    APP_ERR_MISSING_FLASH_BIOS_VERSION,             /* In case we can not find the flash BIOS version */
    APP_ERR_MISSING_BIN_FILE_BIOS_VERSION,          /* In case we can not find the binary file BIOS version */
    APP_ERR_MISSING_OFFSET_VALUE,
    APP_ERR_ILLEGAL_OFFSET_VALUE,
    APP_ERR_TSF_NOT_SPECIFIED,                      /* TSF is not provided */
    APP_ERR_BSF_NOT_SPECIFIED,                      /* BSF is not provided */
    APP_ERR_INVALID_VIDEO_MODE,                     /* Invalid video mode */
    APP_ERR_VIDEO_MODE_NOT_SPECIFIED,               /* When the -vm flag not followed by a number */
    APP_ERR_HELP_FILE_NOT_SPECIFIED,                /* Help file is not provided */
    APP_ERR_MSG_FILE_NOT_SPECIFIED,                 /* Custom messages file  is not provided */
    APP_ERR_EXEC_FILE_NOT_SPECIFIED,                /* Target executable file is not provided */
    APP_ERR_HELP_FILE_MISSING,                      /* Help file is missing*/
    APP_ERR_MSG_FILE_MISSING,                        /* Custom messages file  missing */
    APP_ERR_ILLEGAL_TARGET_EXE_NAME,                /* Illegal target executable file name */
    APP_ERR_TARGET_DRIVE_IS_FULL,                   /* Target drive doesn't have enough free space */
    APP_ERR_TOO_MANY_PARAMS,                        /* -cmg -o -coh cannot be used with other flags */
	APP_ERR_READING_FROM_FILE,						/* Error reading from file */
	APP_ERR_OPENING_FILE,							/* Error opening from file */
	APP_ERR_COMPRESSING_FILE,						/* Error compressing file */
    APP_ERR_MULTIPLE_INSTANCES,                     /* Only a single instance of the app can run at a time*/
      
    APP_ERR_UNEXPECTED_ERROR,                       /* A general unexpected error */
   
    APPLICATION_NAME,                               /* Application name */
    MAIN_WINDOW_HEADLINE,                           /* The main window headline */
    UPDATE_READY_TO_BEGIN,                          /* Update process is ready to begin */
    START_UPDATE_PROCESS_YES_NO,                    /* Ask the user to start the update process */
    START_COMPARISON_PROCESS_YES_NO,
    LOADING_CONFIG_AND_BINARY,                      /* Loading configuration and binary files */
    STATUS_STARTING_BIOS_UPDATE,                    /* Starting BIOS update process */          
    FLASH_UPDATED_SUCCESSFULLY,                     /* BIOS update succeed, no restart */
    PRESS_ANY_KEY_TO_CONTINUE,                      /* Press any key to continue message */
    UPDATE_OPERATION_CANCELED,                      /* Update operation canceled */    
    FLASH_UPDATE_NOT_COMPLETED,                     /* The BIOS update wasn't complete because of some error */
    FU_CORE_ERROR_OCCURRED,
    WELCOME_TO_THE_APPLICATION,
    DOWNLOADING_TO_FLASH,
    FLASH_CONFIGURATION_FILE_IS_MISSING,
    STATUS_LOADING_BIOS_IMAGE,    
    FLASH_UPDATED_SUCCESSFULLY_RESTART,
    FLASH_UPDATED_SUCCESSFULLY_RESTART_WIN,
    COMPARING_BIN_FILE_WITH_FLASH,
    FLASH_COMPARE_RESULT,
    FLASH_AND_BIN_FILE_ARE_THE_SAME,
    FLASH_AND_BIN_FILE_ARE_DIFFERENT,
    TARGET_DUMP_FILE_EXIST,
    DUMPING_FLASH_TO_FILE,
    PLEASE_WAIT,
    FLASH_PROTECTION_CONTINUE_YES_NO,
    FLASH_BIOS_VER_IS_NEWER_CONTINUE_ANYWAY,        /* Ask the user if he want to countinue although the flash BIOS version is newer/same than the one in the binary image file */
    APP_BSF_GENERATED_SUCCESSFULLY,                 /* Binary Script File was generated successfully */
    APP_REPACKED_SUCCESSFULLY,                      /* Flash update was repacked succefully*/
    APP_ERR_CUSTOM_MSG_SYNTAX_ERROR,                /* A syntax error in the Custom messages file*/
    APP_ERR_CUSTOM_MSG_TOO_LONG,                    /* A custom message cannot exceed MAX_MSG_LENGTH*/
    BINARY_DUMPED_SUCCESSFULLY,
    APP_MAX_RESULT                                      /* Maximum error\result number */
    
} APP_RESULT;

typedef struct AppMessage
{
    const APP_RESULT    enumAsInt;
    const char*         enumAsStr;
    char                messageText[MAX_MSG_LENGTH];    
} AppMessage;


/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                     FUNCTION DECLERATIONS                                     */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

char *GetAppCommandLineFromFile();
int IsFileExist(char *p_fileName);

char *GetAppTextString(unsigned int p_txtCode);
char *GetMsgBoxTitleString(int p_appResult);

void StringToTokens(char *p_str,
                      int *p_numOfTokens,
                      char ***p_tokens,
                      char *p_firstToken,
                      int p_maxTokens);

APP_RESULT parseCustomizeMessages(char *p_msgFilePtr, size_t p_msgFileSize, BOOLEAN toUpdate);

void trim (char * p_str);

APP_RESULT updateAppMessageTable(char* p_strEnum,char*  p_message, BOOLEAN toUpdate);

APP_RESULT convertGeneralErrorToAppResult(int p_generalError);

const char* getAPPEnumAsString(int p_enum);

int verifyAppArrayConsistency();


#endif
