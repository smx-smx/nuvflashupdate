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

#include <stdio.h>
#include "exe_archive.h"
#include "FUInterface.h"
#include "FUCore.h"
#include "Flupdcom.h"
#include "FUScript.h"
#ifndef __LINUX__
#include "AppDisplay.h"
#endif
/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                          DEFENITIONS                                          */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

#ifdef __LINUX__
void open_hwrw_drv();
#define _UNKNOWN   0
#define getch()   getchar()
#define strcmpi   strcmp

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Luka's Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

#endif



/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                   LOCAL FUNCTION DECLARATION                                  */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

#ifndef __LINUX__
static char * AttachExeExtension(char *p_fileName);
#endif

static int waitForYesNo();

static char* mapFileToMemory(char *p_fileName, unsigned long *p_fileSize);


#ifdef COMMON_RECEIVE_FLASH_CONFIG_FROM_USER

    static char* openConfigFile(char *p_configFileName, unsigned long *p_configFileSize);

#endif

#ifdef COMMON_RECEIVE_FLASH_BIN_IMG_FROM_USER

    static char* openBinaryImageFile(char *p_binaryFileName, unsigned long *p_binFileSize);

#endif



static void processApplicationOptions(AppOptions *p_appOptions, FlashUpdateOptions *p_fuOptions);
    
static FlashUpdateOptions* parseCommandLine(int p_argc, char* p_argv[], AppOptions *p_appOptions);

static AppOptions *loadDefaultAppOptions(char *p_appExeName);

char * getAppendixFilePtr(char           *p_exeFileName,
                                    size_t *p_fileSize,
                                    int            p_appendixNumber);

void printUsage(char *p_appExeName);


#ifdef COMMON_RECEIVE_FLASH_CONFIG_FROM_USER

    static void createDefaultFlashCfgFile(AppOptions *p_appOptions);
    static void checkForDefaultFlashCfgFile(AppOptions *p_appOptions);

#endif


#ifdef COMMON_SET_PROTECTED_SECTORS

    static FU_RESULT setProtectedSegments(AppOptions *p_appOptions,
                                             FlashUpdateOptions *p_fuOptions);

#endif


static int checkIfDownloadRequired(AppOptions *p_appOptions);
static FU_RESULT doFlashAndBinComparison(AppOptions *p_appOptions, FlashUpdateOptions *p_fuOptions);
static FU_RESULT dumpFlashToFileAppOptions(AppOptions *p_appOptions, FlashUpdateOptions *p_fuOptions);

static APP_RESULT displayFlashProtectionWarning(int *p_displayed,
                                                    AppOptions *p_appOptions,
                                                    FlashUpdateOptions *p_fuOptions);


int oldVersionWarning(char *p_flashVer, char *p_binVer);

/*-----------------------------------------------------------------------------------------------*/
/*                                                                                               */
/*                                    FUNCTION IMPLEMENTATION                                    */
/*                                                                                               */
/*-----------------------------------------------------------------------------------------------*/

int main(int argc, char ** argv)
{
    FU_RESULT             l_result            = FU_RESULT_OK;
    char                  *l_configFilePtr    = NULL;
    unsigned long         l_configFileSize    = 0;    
    FlashUpdateOptions    *l_fuOptions           = NULL;
    AppOptions            *l_appOptions         = NULL;
    char                  l_continueFlag      = 0;
    char                  *l_flashVer         = NULL;
    char                  *l_binVer           = NULL;
    char                  l_windowTitle[64];
    char                  **l_cmdFileArgs     = NULL;
    int                   l_numOfArgs         = 0;
    FILE                  *l_fileHandle       = NULL;
    int                   l_checkBIOSVersion  = 0;
    int                   l_msgDisplayedFlag  = 0;
    char                  l_buffer[10];

#ifndef __LINUX__
    char*                 l_msgFilePtr        = NULL;
    size_t                l_msgFileSize;
    char                  *l_appExeName       = NULL;    
#endif

#ifdef __LINUX__
    open_hwrw_drv();
#endif


#ifdef COMMON_CMD_FILE

    /*-------------------------------------------------------------------------------------------*/
    /* In case there are no parameters and the default command line file exists                  */
    /* We will load the program arguments from it                                                */
    /*-------------------------------------------------------------------------------------------*/
    
    if ((1 == argc) && (IsFileExist(DEFAULT_COMMAND_LINE_FILE)))
    {
        StringToTokens(GetAppCommandLineFromFile(), &l_numOfArgs, &l_cmdFileArgs, argv[0], 30);
        if (l_numOfArgs > 1)
        {
            argc = l_numOfArgs;
            argv = l_cmdFileArgs;
        }        
    }

#endif


    /* Generating the Main Window title */
        
    strcpy(l_windowTitle, GetAppTextString(MAIN_WINDOW_HEADLINE));
    strcat(l_windowTitle, APP_VERSION);
    
    SetMainWindowTopTitle(l_windowTitle);
    
#ifndef __LINUX__   
   
    SetMainWindowBottomTitle(NULL);
    /* Verifying application executable ends with ".exe" */
    
    l_appExeName = AttachExeExtension(argv[0]);

  
    /* Customize messages */
    
    l_msgFilePtr = getAppendixFilePtr(l_appExeName, &l_msgFileSize, AEPL_MESSAGES);    
    parseCustomizeMessages(l_msgFilePtr, l_msgFileSize, TRUE);
#endif

    /* Init Script module, should be done before parsing the command line */
    SCR_Init();
    
    /* Loading program options */  

    l_appOptions = loadDefaultAppOptions(argv[0]);    
    l_fuOptions = parseCommandLine(argc, argv, l_appOptions);
    
    PrintText(APP_VERBOSE_PRINT, "%s\n", l_windowTitle);
    PrintText(APP_VERBOSE_PRINT, "Command line:%s\n", l_appOptions->CommandLine);
    
    processApplicationOptions(l_appOptions, l_fuOptions);
    
    EnterGraphicMode(l_appOptions->VideoMode);


#ifdef COMMON_RECEIVE_FLASH_BIN_IMG_FROM_USER
    
    if ((0 == l_appOptions->BinaryFileName[0]) &&(!l_appOptions->DumpOnly))
    {
        AppExit(APP_ERR_FLASH_BIN_NOT_SPECIFIED, APP_RESULT_TYPE, NULL);        
    }

#endif


#ifdef COMMON_RECEIVE_FLASH_CONFIG_FROM_USER
    
    if (0 == l_appOptions->ConfigFileName[0])
    {        
        checkForDefaultFlashCfgFile(l_appOptions);
        strcpy(l_appOptions->ConfigFileName, DEFAULT_FLASH_CONFIG_FILE_MANE);        
    }

#endif
    
    /* Drawing main window */
    
    DisplayCenterMsgBox(MSG_BOX_GENERAL, GetMsgBoxTitleString(WELCOME_TO_THE_APPLICATION), 0, 0);
    PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(LOADING_CONFIG_AND_BINARY));

#ifdef COMMON_EXIT_TYPE_ALWAYS_RESET_EC

    l_fuOptions->ExitType = WCB_EXIT_RESET_EC;          /* Required at DOS else the keyboard will stuck */

#endif  


#ifdef COMMON_RECEIVE_FLASH_CONFIG_FROM_USER
    
    l_configFilePtr = openConfigFile(l_appOptions->ConfigFileName, &l_configFileSize);    

#else

    l_configFilePtr = getAppendixFilePtr(l_appExeName, &l_configFileSize, AEPL_FLASH_CONFIGURATION);

#endif


if (!l_appOptions->DumpOnly)
{
    #ifdef COMMON_RECEIVE_FLASH_BIN_IMG_FROM_USER

        l_fuOptions->BinaryImageFilePtr = openBinaryImageFile(l_appOptions->BinaryFileName, &l_fuOptions->BinaryImageFileSize);

    #else
        
        l_fuOptions->BinaryImageFilePtr = getAppendixFilePtr(l_appExeName, &l_fuOptions->BinaryImageFileSize, AEPL_FLASH_BINARY_IMAGE);

    #endif
}

#ifdef COMMON_AUTO_CONFIG_SHARED_MEMORY

    l_fuOptions->SkipConfig = FALSE;

#endif

    /* Analyzing the configuration file */
    
    l_result = AnalyzeConfigurationFile(l_configFilePtr, l_configFileSize);
    if (FU_RESULT_OK != l_result)
    {
        AppExit(l_result, FU_RESULT_TYPE, NULL);
    }


    if(checkIfDownloadRequired(l_appOptions))
    {
        /* Making sure (if needed) the user is willing */
        /* to continue with the update process         */  
        
        l_continueFlag = 0;
        if (l_appOptions->NoPause)
        {
            l_continueFlag = 1;
        }
        else
        {           
            DisplayCenterMsgBox(MSG_BOX_YES_NO, GetMsgBoxTitleString(START_UPDATE_PROCESS_YES_NO), 0, 0);
            PrintCenterMsgBoxText(1, TEXT_ALIGN_CENTER, GetAppTextString(START_UPDATE_PROCESS_YES_NO));
            
            if (waitForYesNo())
            {
                l_continueFlag = 1;
            }
        }

    }
    else
    {
        l_continueFlag = 1;
    }
    
    if (l_continueFlag)
    {
        
        if ((l_appOptions->Dump) || (l_appOptions->DumpOnly))
        {   
             /* Making sure the target dump file is not exist */
              
            l_fileHandle = fopen(l_appOptions->DumpFileName, "rb");
            if (NULL != l_fileHandle)
            {
                fclose(l_fileHandle);

                /* The target dump file exist, the user has to determine whether to overwrite it */
                
                DisplayCenterMsgBox(MSG_BOX_YES_NO, GetMsgBoxTitleString(TARGET_DUMP_FILE_EXIST), 350, 0);
                PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(TARGET_DUMP_FILE_EXIST), l_appOptions->DumpFileName);
                if (!waitForYesNo())
                {
                    if (l_appOptions->DumpOnly)
                    {

                        AppExit(APP_RESULT_USER_ABORTED, APP_RESULT_TYPE, NULL);
                        
                    }
                    else
                    {
                        l_appOptions->Dump = 0;
                    }
                }
            }
        }

   
        /* When using a disk on key with the -v flag, writing operation takes a lot of time */
        /* there for we have to display a temporary "Please wait..." message                */
        
        if (l_appOptions->Verbose)
        {
            DisplayCenterMsgBox(MSG_BOX_GENERAL, GetMsgBoxTitleString(START_UPDATE_PROCESS_YES_NO), 0, 0);
            PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(PLEASE_WAIT));            
        }



        /*---------------------------------------------------------------------------------------*/
        /* Disabling the USB SMI/ Global SMI accroding to the given command line flags           */
        /*---------------------------------------------------------------------------------------*/
        
        if (l_appOptions->DisableLegacyUSB)
        {                   
            DisableSMILegacyUSB();                                 /* Disabling the USB keyboard */
        }                    

         if (l_appOptions->DisableGlobalSMI)
        {                   
            DisableGlobalSMI();                                     /* Disabling the Global SMI  */
        }   

        
        /*---------------------------------------------------------------------------------------*/
        /* Starting the update flow                                                              */
        /*---------------------------------------------------------------------------------------*/
        
        l_result = StartUpdateFlow(l_fuOptions);    
        if (FU_RESULT_OK != l_result)
        {            
            AppExit(l_result, FU_RESULT_TYPE, NULL);
        } 


    #ifdef COMMON_SET_PROTECTED_SECTORS

        /*---------------------------------------------------------------------------------------*/
        /* Set all the write protected sectors according                                         */
        /* to the command line flags                                                             */
        /*---------------------------------------------------------------------------------------*/
        
        l_result = setProtectedSegments(l_appOptions, l_fuOptions);
        if (FU_RESULT_OK != l_result)
        {   
            EndUpdateFlow(1);
            AppExit(l_result, FU_RESULT_TYPE, NULL);
        } 

    #endif


    l_result = ConfigureFlashAccess(l_fuOptions);
    if (FU_RESULT_OK != l_result)
    {   
        EndUpdateFlow(1);
        AppExit(l_result, FU_RESULT_TYPE, NULL);
    } 


    l_result = displayFlashProtectionWarning(&l_msgDisplayedFlag, l_appOptions, l_fuOptions);
    if (APP_RESULT_OK != l_result)
    {
        AppExit(l_result, APP_RESULT_TYPE, NULL);
    }

    if (l_msgDisplayedFlag)
    {
        if (l_appOptions->Verbose)
        {
            DisplayCenterMsgBox(MSG_BOX_GENERAL, GetMsgBoxTitleString(START_UPDATE_PROCESS_YES_NO), 0, 0);
            PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(PLEASE_WAIT));            
        }
        
        /*---------------------------------------------------------------------------------------*/
        /* First, we need to disable the USB SMI/ Global SMI accroding to the given command      */
        /* line flags                                                                            */
        /*---------------------------------------------------------------------------------------*/
        
        if (l_appOptions->DisableLegacyUSB)
        {                   
            DisableSMILegacyUSB();                                 /* Disabling the USB keyboard */
        }                    
        
         if (l_appOptions->DisableGlobalSMI)
        {                   
            DisableGlobalSMI();                                     /* Disabling the Global SMI  */
        }   
        
        /*---------------------------------------------------------------------------------------*/
        /* Starting the update flow once again                                                   */
        /*---------------------------------------------------------------------------------------*/
        
        l_result = StartUpdateFlow(l_fuOptions);    
        if (FU_RESULT_OK != l_result)
        {            
            AppExit(l_result, FU_RESULT_TYPE, NULL);
        }
        l_result = ConfigureFlashAccess(l_fuOptions);

        if (FU_RESULT_OK != l_result)
        {   
            EndUpdateFlow(1);
            AppExit(l_result, FU_RESULT_TYPE, NULL);
        }
    }

    

        
    #ifdef COMMON_FLAG_DUMP

        if ((l_appOptions->Dump) || (l_appOptions->DumpOnly))
        {
            l_result = dumpFlashToFileAppOptions(l_appOptions, l_fuOptions);
            if (FU_RESULT_OK != l_result)
            {
                AppExit(l_result, FU_RESULT_TYPE, NULL);
            }
        }
        
    #endif
    

        if (0 == l_appOptions->IgnoreBIOSModelCheck)
        {
            l_checkBIOSVersion = 1;
            l_result = IsSameBIOSModel(l_fuOptions->BinaryImageFilePtr, NULL, NULL);
            if (FU_RESULT_OK != l_result)
            {                   
                switch(l_result)
                {
                case FU_RESULT_NO_BIOS_MODEL:

                    /* there is no BIOS model neither on the binary image nor the flash BIOS */
                    
                    l_checkBIOSVersion = 0;
                    
                    break;
                    
                case FU_ERR_DIFFERENT_BIOS_MODEL:
                    EndUpdateFlow(1);
                    AppExit(APP_ERR_DIFFERENT_BIOS_MODEL, APP_RESULT_TYPE, NULL);
                    break;
                    
                case FU_ERR_MISSING_BIN_FILE_BIOS_MODEL:
                    EndUpdateFlow(1);
                    AppExit(APP_ERR_MISSING_BIN_FILE_BIOS_MODEL, APP_RESULT_TYPE, NULL);
                    break;                        
                    
                case FU_ERR_MISSING_FLASH_BIOS_MODEL:
                    EndUpdateFlow(1);
                    AppExit(APP_ERR_MISSING_FLASH_BIOS_MODEL, APP_RESULT_TYPE, NULL);
                    break;
                    
                default:
                    EndUpdateFlow(1);
                    AppExit(l_result, FU_RESULT_TYPE, NULL);
                    break;
                }                
            }

            if (l_checkBIOSVersion)
            {
                /* Check the BIOS version */

                l_result = IsFlashBIOSNewer(l_fuOptions->BinaryImageFilePtr, &l_binVer, &l_flashVer);
                if (FU_RESULT_OK != l_result)
                {
                    EndUpdateFlow(1);
                    switch(l_result)
                    {
                    case FU_RESULT_OLDER_BIN_FILE_BIOS:

                        /* Flash BIOS version is newer than the one in the binary image file */

                        if (!oldVersionWarning(l_flashVer, l_binVer)) 
                        {
                            AppExit(APP_RESULT_USER_ABORTED, APP_RESULT_TYPE, NULL);
                        }

                        if (l_appOptions->Verbose)
                        {
                            DisplayCenterMsgBox(MSG_BOX_GENERAL, GetMsgBoxTitleString(START_UPDATE_PROCESS_YES_NO), 0, 0);
                            PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(PLEASE_WAIT));            
                        }
                     
                        /*---------------------------------------------------------------------------------------*/
                        /* First, we need to disable the USB SMI/ Global SMI accroding to the given command      */
                        /* line flags                                                                            */
                        /*---------------------------------------------------------------------------------------*/
                        
                        if (l_appOptions->DisableLegacyUSB)
                        {                   
                            DisableSMILegacyUSB();                                 /* Disabling the USB keyboard */
                        }                    
                        
                         if (l_appOptions->DisableGlobalSMI)
                        {                   
                            DisableGlobalSMI();                                     /* Disabling the Global SMI  */
                        }   

                        /*---------------------------------------------------------------------------------------*/
                        /* Starting the update flow once again                                                   */
                        /*---------------------------------------------------------------------------------------*/
                
                        l_result = StartUpdateFlow(l_fuOptions);    
                        if (FU_RESULT_OK != l_result)
                        {            
                            AppExit(l_result, FU_RESULT_TYPE, NULL);
                        }
                        l_result = ConfigureFlashAccess(l_fuOptions);
                        if (FU_RESULT_OK != l_result)
                        {   
                            EndUpdateFlow(1);
                            AppExit(l_result, FU_RESULT_TYPE, NULL);
                        }            
                        
                        break;

                    case FU_ERR_MISSING_BIN_FILE_BIOS_VERSION:
                        AppExit(APP_ERR_MISSING_BIN_FILE_BIOS_VERSION, APP_RESULT_TYPE, NULL);                            
                        break;

                    case FU_ERR_MISSING_FLASH_BIOS_VERSION:
                        AppExit(APP_ERR_MISSING_FLASH_BIOS_VERSION, APP_RESULT_TYPE, NULL);                            
                        break;
                        
                    default:
                        AppExit(l_result, FU_RESULT_TYPE, NULL);
                    }                    

                }
            }

        
        }

        
    
        if (l_appOptions->IgnoreProtectedSectors)
        {
            IgnoreProtectedSectors();
        }
        
        if(checkIfDownloadRequired(l_appOptions))
        {   
            #ifdef COMMON_FLAG_OFS
            if (l_fuOptions->FlashOffset > 0)
            {
                l_result = IsOffsetDescribesASectorStart(l_fuOptions->FlashOffset);
                if (FU_RESULT_OK != l_result)
                {
                    EndUpdateFlow(0);
                    AppExit(APP_ERR_ILLEGAL_OFFSET_VALUE, APP_RESULT_TYPE, itoa(l_fuOptions->FlashOffset, l_buffer, 16));
                }
            }
            #endif
          
            DisplayCenterMsgBox(MSG_BOX_GENERAL, GetMsgBoxTitleString(DOWNLOADING_TO_FLASH), 400, 0);
            PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(DOWNLOADING_TO_FLASH));
            FillProgressBar(0);
        
            l_result = DownloadToFlash();
            if (FU_RESULT_OK != l_result)
            {
                EndUpdateFlow(0);
                AppExit(l_result, FU_RESULT_TYPE, NULL);
            }
        }
        else
        {
        #ifdef COMMON_FLAG_COMPARE           

            if ((l_appOptions->Compare) && (!l_appOptions->DumpOnly))           /* If dumpOnly, there's no .bin file to compare to */
            {                     
                l_result = doFlashAndBinComparison(l_appOptions, l_fuOptions);
                if (FU_RESULT_OK != l_result)
                {            
                    AppExit(l_result, FU_RESULT_TYPE, NULL);
                }
            }

        #endif
        }
    
        l_result = EndUpdateFlow(0);
        if (FU_RESULT_OK != l_result)
        {            
            AppExit(l_result, FU_RESULT_TYPE, NULL);
        } 
        l_result = APP_RESULT_OK;
    }
    else
    {
        /* Abort the update process */
        
        l_result = APP_RESULT_USER_ABORTED;
    }

    free(l_fuOptions->BinaryImageFilePtr);
    free(l_fuOptions);
    free(l_configFilePtr);

    if(checkIfDownloadRequired(l_appOptions))
    {            
        if (FU_RESULT_OK == l_result)
        {   
            DisplayCenterMsgBox(MSG_BOX_GENERAL, GetMsgBoxTitleString(FLASH_UPDATED_SUCCESSFULLY_RESTART), 0, 1);
            PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(FLASH_UPDATED_SUCCESSFULLY_RESTART));
            if ((!l_appOptions->NoPause) && (l_appOptions->VideoMode != _UNKNOWN))
            {
                 getch();
            }
            AppExit(APP_RESULT_OK, APP_RESULT_TYPE, NULL);
        }
        else if (APP_RESULT_USER_ABORTED == l_result)
        {
            AppExit(l_result, APP_RESULT_TYPE, NULL);
        }
        else
        {
            AppExit(l_result, FU_RESULT_TYPE, NULL);
        }
    }
    else
    {
        if (l_appOptions->DumpOnly)
        {
            DisplayCenterMsgBox(MSG_BOX_GENERAL, GetMsgBoxTitleString(BINARY_DUMPED_SUCCESSFULLY), 0, 1);
            PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(BINARY_DUMPED_SUCCESSFULLY));
            if ((!l_appOptions->NoPause) && (l_appOptions->VideoMode != _UNKNOWN))
            {
                 getch();
            }
        }
        AppExit(l_result, APP_RESULT_TYPE, NULL);
    }
    
    return APP_RESULT_OK;
  
    
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: displayFlashProtectionWarning                                                       */
/*                                                                                               */
/* Parameters:   p_displayed  - 0 no warning displayed                                           */
/*                              1 a warning message was displayed                                */
/*               p_appOptions - The application options                                          */
/*               p_fuOptions  - Flash update options                                             */
/*                                                                                               */
/* Returns:      APP_RESULT                                                                      */
/*                                                                                               */ 
/* Side effects: Exist the update flow in case a message is displayed to the user                */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Display a warning message regarding flash protection, only if needed            */
/*-----------------------------------------------------------------------------------------------*/

static APP_RESULT displayFlashProtectionWarning(int *p_displayed,
                                                    AppOptions *p_appOptions,
                                                    FlashUpdateOptions *p_fuOptions)
{
    APP_RESULT l_retVal = APP_RESULT_OK;
    unsigned long l_checkSize = 0;
    FlashConfigurationData *l_flashConfig   = NULL;


    *p_displayed = 0;


    if(FU_RESULT_OK == GetCorrectFlashConfiguration(p_fuOptions, &l_flashConfig))
    {
        if (p_appOptions->Dump)
        {
            l_checkSize = l_flashConfig->FlashSize * 1024;
        }
        else
        {
            l_checkSize = l_flashConfig->FlashSize * 1024 - p_fuOptions->FlashOffset; 
        }
    }
    else
    {
        if (p_appOptions->Dump)
        {
            l_checkSize = p_fuOptions->BinaryImageFileSize;
        }
        else
        {
            l_checkSize = p_fuOptions->BinaryImageFileSize - p_fuOptions->FlashOffset;    
        }
    }
            
   
    
    if(IsFlashAreaHasAnyProtection(p_fuOptions->FlashOffset, l_checkSize))
    {
        EndUpdateFlow(1);
        *p_displayed = 1;
        DisplayCenterMsgBox(MSG_BOX_YES_NO, GetMsgBoxTitleString(FLASH_PROTECTION_CONTINUE_YES_NO), 350, 0);
        PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(FLASH_PROTECTION_CONTINUE_YES_NO));
         if (!waitForYesNo())
         {
            l_retVal = APP_RESULT_USER_ABORTED;            
         }
    }
    
    return l_retVal;
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: dumpFlashToFileAppOptions                                                           */
/*                                                                                               */
/* Parameters:   p_appOptions     - The application options                                      */
/*               p_fuOptions      - Flash update options                                         */
/*                                                                                               */
/* Returns:      FU_RESULT                                                                       */
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Dump flash data into the target dump file                                       */
/*-----------------------------------------------------------------------------------------------*/

static FU_RESULT dumpFlashToFileAppOptions(AppOptions *p_appOptions, FlashUpdateOptions *p_fuOptions)
{ 
    FU_RESULT                  l_retVal        = FU_RESULT_OK;
    FILE                      *l_fileHandle    = NULL;
    char                      *l_flashContent  = NULL;
    FlashConfigurationData *l_flashConfig   = NULL;



    /* Start dumping the flash content into the target file */

    DisplayCenterMsgBox(MSG_BOX_GENERAL, GetMsgBoxTitleString(DUMPING_FLASH_TO_FILE), 0, 0);
    PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(DUMPING_FLASH_TO_FILE), p_appOptions->DumpFileName);
 
    l_retVal = GetCorrectFlashConfiguration(p_fuOptions, &l_flashConfig);
    if (FU_RESULT_OK != l_retVal)
    {    
        return l_retVal;
    }
    
    l_fileHandle = fopen(p_appOptions->DumpFileName, "wb");
    if (NULL == l_fileHandle)
    {
        return APP_ERR_FAILED_TO_CREATE_FILE;
    }
    
    l_flashContent = (char*)(malloc(sizeof(char) * (unsigned long)l_flashConfig->FlashSize * 1024));
    if (NULL == l_flashContent)
    {
        fclose(l_fileHandle);
        l_retVal = FU_ERR_UNEXPECTED_ERROR;
    }
    
    l_retVal = ReadBytesFromFlash(0, (unsigned char*)l_flashContent, (unsigned long)l_flashConfig->FlashSize * 1024);
    if (FU_RESULT_OK != l_retVal)
    {   
        free(l_flashContent);
        fclose(l_fileHandle);
        return l_retVal;
    }

    fwrite(l_flashContent, sizeof(char), (unsigned long)l_flashConfig->FlashSize * 1024, l_fileHandle);
    fclose(l_fileHandle);
    free(l_flashContent);                
               

    return l_retVal;
        
}

/*-----------------------------------------------------------------------------------------------*/
/* Function: doFlashAndBinComparison                                                             */
/*                                                                                               */
/* Parameters:   p_appOptions     - The application options                                      */
/*               p_fuOptions      - Flash update options                                         */
/*                                                                                               */
/* Returns:      FU_RESULT                                                                       */
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Preform comparison between the given binary image file and the flash data       */
/*-----------------------------------------------------------------------------------------------*/

static FU_RESULT doFlashAndBinComparison(AppOptions *p_appOptions, FlashUpdateOptions *p_fuOptions)
{
    FU_RESULT l_result   = FU_RESULT_OK;
    int       l_sameFlag = 0;



    
    DisplayCenterMsgBox(MSG_BOX_GENERAL, GetMsgBoxTitleString(COMPARING_BIN_FILE_WITH_FLASH), 400, 0);
    PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(COMPARING_BIN_FILE_WITH_FLASH), p_appOptions->BinaryFileName);
   
    l_result = CompareWithFlashContent(p_fuOptions->BinaryImageFilePtr,
                                       p_fuOptions->BinaryImageFileSize,
                                       &l_sameFlag,
                                       p_fuOptions->FlashOffset);
    
    EndUpdateFlow(1);
    if (FU_RESULT_OK != l_result)
    {        
        AppExit(l_result, FU_RESULT_TYPE, NULL);
    }                
 
    DisplayCenterMsgBox(MSG_BOX_GENERAL, GetMsgBoxTitleString(FLASH_COMPARE_RESULT), 350, 1);
    if (l_sameFlag)
    {
        PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(FLASH_AND_BIN_FILE_ARE_THE_SAME), p_appOptions->BinaryFileName);
    }
    else
    {
        PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(FLASH_AND_BIN_FILE_ARE_DIFFERENT), p_appOptions->BinaryFileName);
    }
    if (p_appOptions->VideoMode != _UNKNOWN)
    {
        getch();
    }


    return l_result;
}


/*-----------------------------------------------------------------------------------------------*/
/* Function: checkIfDownloadRequired                                                             */
/*                                                                                               */
/* Parameters:   p_appOptions     - The application options                                      */
/*                                                                                               */
/* Returns:      1 - Download required                                                           */
/*               0 - No need to preform download                                                 */
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Determine according to the command line flags whether there is a need           */
/*               to preform download                                                             */
/*-----------------------------------------------------------------------------------------------*/

static int checkIfDownloadRequired(AppOptions *p_appOptions)
{
    int l_retVal = 1;                                  /* The default state is download required */



#ifdef COMMON_FLAG_COMPARE
    
    if ((p_appOptions->Compare) || (p_appOptions->DumpOnly))
    {
        l_retVal = 0;
    }
    
#endif
    
    return l_retVal;
}

#ifndef __LINUX__
/*-----------------------------------------------------------------------------------------------*/
/* Function: getAppendixFilePtr                                                                  */
/*                                                                                               */
/* Parameters:   p_exeFileName    - The application executbale file name                         */
/*               p_fileSize       - Will hold the file size                                      */
/*               p_appendixNumber - The index of the appendix we want to load                    */
/*                                                                                               */
/* Returns:      A pointer to the mapped file                                                    */
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Load an appendix from the given executable into the memory                      */
/*-----------------------------------------------------------------------------------------------*/

static char * getAppendixFilePtr(char           *p_exeFileName,
                                        size_t         *p_fileSize,
                                        int            p_appendixNumber)
{
    AppendixHeader *l_exeAppendixHeader = (AppendixHeader*)(malloc(sizeof(AppendixHeader)));
    char *l_filePtr = NULL;
    

   
    if (LoadAppendixHeaderFromExe(p_exeFileName, l_exeAppendixHeader) > 0)
    {

        l_filePtr =  ExtractAppendixFromExe(p_exeFileName,
                                            l_exeAppendixHeader->
                                                AppedixEntriesArray[p_appendixNumber],
                                            NULL);

        *p_fileSize = l_exeAppendixHeader->AppedixEntriesArray[p_appendixNumber]->Size;        
    }
    
    free(l_exeAppendixHeader);   
    


    return l_filePtr;
}
#endif

/*-----------------------------------------------------------------------------------------------*/
/* Function: processApplicationOptions                                                           */
/*                                                                                               */
/* Parameters:   p_appOptions - Application options                                              */
/*                                                                                               */
/* Returns:      None                                                                            */
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Process the application options before starting the main flow                   */
/*-----------------------------------------------------------------------------------------------*/

static void processApplicationOptions(AppOptions *p_appOptions, FlashUpdateOptions *p_fuOptions)
{
#ifndef __LINUX__
    size_t          l_fileSize    = 0;
    unsigned char  *l_filePtr     = NULL;
#endif
    FU_RESULT       l_retVal      = FU_RESULT_OK;



    if (p_fuOptions->SharedBIOS && p_fuOptions->NotSharedBIOS)
    {
        AppExit(APP_ERR_ILLEGAL_FLAG_MIXING, APP_RESULT_TYPE, "-shf and -nshf");
    }
    
    if (p_appOptions->ShowUsage)
    {
        printUsage(p_appOptions->AppExeName);
        AppExit(APP_RESULT_OK, APP_RESULT_TYPE, NULL);
    }

    if(p_fuOptions->GenerateBSF == 1)
	{
        l_retVal = SCR_ExtractData(p_appOptions->TsfName, NULL);
		if(FU_RESULT_OK != l_retVal)
		{
			AppExit(l_retVal, FU_RESULT_TYPE, NULL);
		}
        l_retVal = SCR_WriteBSF(p_appOptions->TsfName);
		if(FU_RESULT_OK != l_retVal)
		{
            AppExit(l_retVal, FU_RESULT_TYPE, NULL);
		}
		AppExit(APP_BSF_GENERATED_SUCCESSFULLY, APP_RESULT_TYPE, NULL);	
		
	}
	if(p_fuOptions->RunBSF == 1)
	{
        l_retVal = SCR_ReadBSF(p_appOptions->BsfName);
		if(FU_RESULT_OK != l_retVal)
		{
			AppExit(l_retVal, FU_RESULT_TYPE, NULL);
		}			
        SCR_SetCommandsExecution(TRUE);	
	}
    else if(TRUE == IsFileExist(DEFAULT_BSF_NAME))
    {       
        l_retVal = SCR_ReadBSF(DEFAULT_BSF_NAME);
        if(FU_RESULT_OK != l_retVal)
		{
			AppExit(l_retVal, FU_RESULT_TYPE, NULL);
		}			
        SCR_SetCommandsExecution(TRUE);
    }
#ifndef __LINUX__    
    /* Load the company logo */
   
    l_filePtr = (unsigned char *) getAppendixFilePtr(p_appOptions->AppExeName, &l_fileSize, AEPL_COMPANY_LOGO);
    LoadLogo(l_filePtr, l_fileSize, COMPANY_LOGO);
    free(l_filePtr);

#ifdef COMMON_SHOW_OEM_LOGO

    l_filePtr = (unsigned char *) getAppendixFilePtr(p_appOptions->AppExeName, &l_fileSize, AEPL_OEM_LOGO);
    LoadLogo(l_filePtr, l_fileSize, OEM_LOGO);
    free(l_filePtr);
    
#endif
#endif
    
}



static int waitForYesNo()
{
    char l_charRead = 0;

    /* When graphic mode is on, the window includes the y/n options */
    /* Therefore, when graphic mode is off, we need to print them */
   if(GetGraphicMode() == GRAPHIC_MODE_OFF)
   {
       printf("Y = Yes   N = No\n");
   }
    
        
    while(1)
    {
        l_charRead = getch(); 
        if((l_charRead == 'y') || (l_charRead == 'Y'))
        {
            return 1;
        }            
        else if((l_charRead == 'n') || (l_charRead == 'N'))                
        {               
            return 0;
        }
    }
}


#ifdef COMMON_SET_PROTECTED_SECTORS

    /*-----------------------------------------------------------------------------------------------*/
    /* Function: setProtectedSegments                                                                */
    /*                                                                                               */
    /* Parameters:   None                                                                            */
    /*                                                                                               */
    /* Returns:      FU_RESULT                                                                       */
    /*                                                                                               */ 
    /* Side effects: None                                                                            */
    /*                                                                                               */ 
    /* Description:                                                                                  */ 
    /*               Config FUcore with the defined write protected segments according               */
    /*               to the command line flags.                                                      */
    /*-----------------------------------------------------------------------------------------------*/

    static FU_RESULT setProtectedSegments(AppOptions         *p_appOptions,
                                          FlashUpdateOptions *p_fuOptions)
    {
        FU_RESULT l_retVal = FU_RESULT_OK;    
        


        PrintText(APP_VERBOSE_PRINT,
                  "setProtectedSegments: IgnoreSpecialSections: %d  WipeAll: %d  WipeClean: %d\n",
                   p_appOptions->IgnoreSpecialSections,
                   p_appOptions->WipeAll,
                   p_appOptions->WipeClean);
       
        if (!p_appOptions->IgnoreSpecialSections)
        {               
            if (p_appOptions->WipeAll)                                /* Implement the WipeAll flag */
            {
                /* We are allowed to erase the sectors but not write them */
                
                l_retVal = AddSegmentAsProtectedSectors(SEGMENT_ESCD, 
                                                        SEGMENT_SIZE,
                                                        SECTOR_WRITE_PROTECTED,
                                                        p_fuOptions);
                if (FU_RESULT_OK != l_retVal)
                {
                    return l_retVal;
                }
                
                l_retVal = AddSegmentAsProtectedSectors(SEGMENT_NV,
                                                        SEGMENT_SIZE,
                                                        SECTOR_WRITE_PROTECTED,
                                                        p_fuOptions);
            }
            else 
            {
                if (p_appOptions->WipeClean)                        /* Implement the WipeClean flag */
                {
                    l_retVal = AddSegmentAsProtectedSectors(SEGMENT_ESCD,
                                                            SEGMENT_SIZE,
                                                            SECTOR_WRITE_PROTECTED,
                                                            p_fuOptions);
                }
                else
                {
                    l_retVal = AddSegmentAsProtectedSectors(SEGMENT_ESCD,
                                                            SEGMENT_SIZE,
                                                            SECTOR_ERASE_WRITE_PROTECTED,
                                                            p_fuOptions);                
                }

                if (FU_RESULT_OK != l_retVal)
                {
                    return l_retVal;
                }
                            
                l_retVal = AddSegmentAsProtectedSectors(SEGMENT_NV,
                                                        SEGMENT_SIZE,
                                                        SECTOR_ERASE_WRITE_PROTECTED,
                                                        p_fuOptions); 
            }
                
        }
            
        return l_retVal;
    }

#endif


#ifdef __LINUX__
void printUsage(char *p_appExeName)
{

    printf("\n\n");
    printf("Nuvoton Flash Update Tool Version 4.3.2\n"); // New Version Marker
    printf("----------------------------------------\n\n");    
    printf("Copyright (C) 2010 Nuvoton Technology Corporation. All Rights Reserved.\n\n");    
    printf(" Usage: flupdate [options] <binary file>\n\n");    
    printf(" <binary file>\n");
    printf("     A file containing the data to program into flash, in binary format\n\n");    
    printf(" Options:\n");
    printf("       -h                 Show program usage\n");
    printf("       -v                 Verbose operation - create a log file and also report\n");
    printf("                          explicit error messages\n");
    printf("       -shf               The system uses shared flash (i.e., BIOS and EC FW\n");
    printf("                          are located on the same flash device).\n");
    printf("       -nshf              The system does not use shared flash (i.e., only FW is\n");
    printf("                          on the EC flash).\n");
    printf("                          Note that by default (i.e., -shf or -nshf are not\n");
    printf("                          selected), the flash layout (i.e., whether the system\n");
    printf("                          uses shared flash or not) is detected automatically.\n");
    printf("       -c <file name>     Flash Configuration file name. By default, the tool\n");
    printf("                          looks for a configuration file named flupdate.cfg\n");
    printf("       -x <exit type>     Select termination option:\n");
    printf("                          0 = Normal Exit (default)\n");
    printf("                          1 = Reset EC\n");
    printf("                          2 = Goto Boot Block\n");  
    printf("       -d <file name>     Dump the flash data to the <file name> file before\n");
    printf("                          downloading/comparing the <binary file> data\n");
    printf("       -sad <file name>   Stand alone dump. Dump the flash data to the <file name>\n");
    printf("                          file without performing download/compare.\n");
    printf("       -compare           Compare the given binary file with the flash data.\n");
    printf("                          In this mode, flash is not programmed. If this flag is\n");
    printf("                          provided in addition to the -ofs flag (see below),\n");
    printf("                          only the bytes starting from <offset> are compared\n");
    printf("       -bbl               Ignore protected sectors\n");
    printf("       -lpc               Use LPC Memory Access through LPC bus (default is FWH)\n");
    printf("                          Valid only if supported by the motherboard\n");
    printf("                          I/O Controller\n");
    printf("       -config            Enable shared memory configuration\n");
    printf("       -up                Use flash protection window\n");
    printf("       -nt                Cancel the timeout on flash operations\n");
    printf("       -noverify          Do not verify flash contents after programming\n");
    printf("       -nocomp            Program the flash without first comparing its contents\n");
    printf("                          to the binary file data.\n");
    printf("                          (By default, the contents of each section are compared\n");
    printf("                          to the source file, and the section is programmed only\n");
    printf("                          if it is different)\n");
    printf("       -noreboot          Do not ask the user to restart the computer after the\n");
    printf("                          download completes\n");
    printf("       -nopause           Start the download/compare without the user approval\n");
    printf("                          In addition, the application terminates\n");
    printf("                          automatically after downloading the binary file to the\n");
    printf("                          flash (without restart the computer)\n");
    printf("       -dislusb           For DOS only: disable the Legacy USB SMI (only in Intel\n");
    printf("                          chipsets) during the flash update process\n");
    printf("       -dissmi            For DOS only: disable SMI (only in Intel chipsets)\n");
    printf("                          during flash update process\n");
    printf("       -cba <address>     Set the Configuration Base Address of the device (in\n");
    printf("                          hex format); otherwise, try to locate the device at the\n");
    printf("                          following addresses (in this order): 0x164E, 0x2E, 0x4E\n");
    printf("       -vrd               Verify the flash contents after programming using read\n");
    printf("                          command\n");
    printf("       -rfp               Remove Flash Protection when the protection window\n");
    printf("                          registers are accessible (i.e., not locked)\n");
    printf("       -ofs <offset>      Load the provided binary file to the flash starting at\n");
    printf("                          offset <offset> from flash start. <offset> must be\n");
    printf("                          aligned with the start address of a flash sector. If\n");
    printf("                          this flag is provided in addition to the -compare flag,\n");
    printf("                          only the bytes starting from <offset> are compared\n");
    printf("       -pfu               Enable FW to perform operations before the Flash\n");
    printf("                          Update process starts (Pre Flash Update)\n");
    printf("       -ocv               Override Calibration Values in the flash with the\n");
    printf("                          values provided in the binary file\n");
    printf("       -genbsf <TSF name> Generate a Binary Script File (BSF) from a Text Script\n"); 
    printf("                          File (TSF). The BSF name is the same as the TSF, but\n");
    printf("                          with a different extension (*.bsf).\n");
    printf("       -runbsf <BSF name> Run the commands in the Binary Script FIle (BSF)\n");
    printf("                          during the Flash Update process\n");
    printf("       -vm <value>        For DOS only. This flag is used to change the default\n");
    printf("                          value of the video mode. It should be used only if\n");
    printf("                          there is a known graphic limitation in the system.\n");
    printf("                          Supported values are 18 (the default mode, uses 16\n");
    printf("                          colors), 17 (uses 2 colors) and 0 (text mode)\n");
    printf("       -s                 Silent mode (Windows only). This flag eliminates\n"); 
    printf("                          any user interaction and minimizes the visual effects\n");
    printf("       -bec               Binary Exit Code. The default exit code upon failure\n");
    printf("                          is a non-zero value. This flag limits the non-zero\n");
    printf("                          value to 1. Thus, the exit code is binary (0 for\n");
    printf("                          success, 1 for failure).\n\n");           
    printf("    Flupdate.cmd:\n");
    printf("       Optional file.\n");
    printf("       When Flash Update application is invoked with no command line parameters\n");
    printf("       it trys to locate the \"Flupdate.cmd\" file at the current directory.\n");
    printf("       If the file exists, Flash Update application reads its command line\n");
    printf("       parameters from this file.\n\n");           
    printf("       Example:\n\n");            
    printf("          If the Flupdate.cmd content is:\n\n");                                   
    printf("             -c flash.cfg flash.bin\n\n");                                   
    printf("          Then the Flash Update Application run as:\n\n");                          
    printf("             Flupdate.exe -c flash.cfg flash.bin\n\n\n");
        

}
#else
void printUsage(char *p_appExeName)
{   
    size_t         l_fileSize    = 0;
    char          *l_filePtr     = getAppendixFilePtr(p_appExeName, &l_fileSize, AEPL_README);
    unsigned long  i             = 0;
    int            l_currentLine = 0;
    int            l_currentPos  = 0;
    unsigned int   l_pressAnyKeyLength = strlen(GetAppTextString(PRESS_ANY_KEY_TO_CONTINUE));



    for (i = 0; i < l_fileSize; i++)
    {
        /* If we are after 'Press any key' we need to delete this text */
       if ((10 == l_filePtr[i]) && (l_currentLine == 0) && (l_currentPos<l_pressAnyKeyLength))
        {
            printf("%*c",l_pressAnyKeyLength, ' ');
        }
       
        printf("%c", l_filePtr[i]);
        ++l_currentPos;

        /* Check for new line */
        
        if (10 == l_filePtr[i])            
        {
            l_currentLine++;
            l_currentPos = 0;
            if (24 == l_currentLine)
            {
                l_currentLine = 0;
                printf(GetAppTextString(PRESS_ANY_KEY_TO_CONTINUE));
                fflush(stdout);
                getch();
                printf("%c", 13);
            }
        }
    }
    printf("\n");

   free(l_filePtr);
}
#endif


/*--------------------------------------------------------------------------------------*/
/* Function: mapFileToMemory                                                            */
/*                                                                                      */ 
/* Parameters:   p_fileName - File name to be mapped                                    */
/*               p_fileSize - Will hold the file size                                   */
/*                                                                                      */
/* Returns:      Pointer the file                                                       */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Map the given file into the memory                                     */
/*--------------------------------------------------------------------------------------*/

static char * mapFileToMemory(char *p_fileName, unsigned long *p_fileSize)
{
    FILE *l_fileHandle = NULL;
    char *l_filePtr = NULL;
    unsigned long l_fileSize = 0;
    unsigned long l_bytesRead = 0;

    


    *p_fileSize = 0;

    
    l_fileHandle = fopen(p_fileName, "rb");
    if (l_fileHandle == NULL)
    {       
        return NULL;
    }
    fseek(l_fileHandle, 0, SEEK_END);    
    l_fileSize = ftell(l_fileHandle);

    l_filePtr = (char *)(malloc(sizeof(char) * l_fileSize));    
    fseek(l_fileHandle, 0, SEEK_SET);
    l_bytesRead = fread(l_filePtr, sizeof(char), l_fileSize, l_fileHandle);
    if (l_bytesRead < l_fileSize)
    {        
        return NULL;
    }

    if (l_fileHandle == NULL)
    {        
        return NULL;
    }
    fclose(l_fileHandle);


    *p_fileSize = l_fileSize;

    
    return l_filePtr;
    
}



/*--------------------------------------------------------------------------------------*/
/* Function: openBinaryImageFile                                                        */
/*                                                                                      */ 
/* Parameters:   p_binaryFileName - Flash binary image file                             */
/*               p_binFileSize    - Will hold the flash binary image file size          */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Open the flash binary image file                                       */
/*--------------------------------------------------------------------------------------*/

#ifdef COMMON_RECEIVE_FLASH_BIN_IMG_FROM_USER

    static char* openBinaryImageFile(char *p_binaryFileName, unsigned long *p_binFileSize)
    {
        FILE *l_fileHandle = NULL;
        char *l_binFilePtr = NULL;


        
        *p_binFileSize = 0;
        l_binFilePtr = mapFileToMemory(p_binaryFileName, p_binFileSize);
        
        l_fileHandle = fopen(p_binaryFileName, "rb");
        if (NULL == l_fileHandle)
        {
            AppExit(APP_ERR_FILE_NOT_EXIST, APP_RESULT_TYPE, p_binaryFileName);
        }
        fclose(l_fileHandle);
        
        if (0 == (*p_binFileSize))
        {
            AppExit(APP_ERR_EMPTY_FLASH_BIN_FILE, APP_RESULT_TYPE, NULL);
        }
        
        return l_binFilePtr;
        
    }

#endif


#ifdef COMMON_RECEIVE_FLASH_CONFIG_FROM_USER

    
    /*--------------------------------------------------------------------------------------*/
    /* Function: openConfigFile                                                             */
    /*                                                                                      */ 
    /* Parameters:   p_configFileName - Configuration file name                             */
    /*               p_configFileSize - Will hold the flash configuration file size         */
    /*                                                                                      */
    /* Returns:      Pointer to the flash configuration file                                */
    /*                                                                                      */ 
    /* Side effects: None                                                                   */
    /*                                                                                      */ 
    /* Description:                                                                         */ 
    /*               Open the flash configuration file                                      */
    /*--------------------------------------------------------------------------------------*/
    
    static char* openConfigFile(char *p_configFileName, unsigned long *p_configFileSize)
    {
        FILE *l_fileHandle = NULL;
        char *l_configFilePtr = NULL;
    
        
        
        *p_configFileSize = 0;
        l_configFilePtr = mapFileToMemory(p_configFileName, p_configFileSize);
        
        l_fileHandle = fopen(p_configFileName, "rb");
        if (NULL == l_fileHandle)
        {
            AppExit(APP_ERR_FILE_NOT_EXIST, APP_RESULT_TYPE, p_configFileName);
        }
        fclose(l_fileHandle);
            
        if ((*p_configFileSize) < MIN_FLASH_CONFIG_FILE_SIZE)
        {           
            AppExit(APP_ERR_SMALL_FLASH_CONFIG_FILE, APP_RESULT_TYPE, NULL);
        }
        
        return l_configFilePtr;
        
    }


    /*--------------------------------------------------------------------------------------*/
    /* Function: checkForDefaultFlashCfgFile                                                */
    /*                                                                                      */ 
    /* Parameters:   p_appOptions - Application options                                     */
    /*                                                                                      */
    /* Returns:      None                                                                   */
    /*                                                                                      */ 
    /* Side effects: None                                                                   */
    /*                                                                                      */ 
    /* Description:                                                                         */ 
    /*               Search for default flash configuration file.                           */
    /*               Ask the user to create a flash config file in case the default         */
    /*               config file doesn't exist                                              */
    /*--------------------------------------------------------------------------------------*/

    static void checkForDefaultFlashCfgFile(AppOptions *p_appOptions)
    {
        FILE *l_configFileHandle = NULL;
        

        
        if( (l_configFileHandle = fopen(DEFAULT_FLASH_CONFIG_FILE_MANE, "rb" )) == NULL )
        { 
            
#ifdef __LINUX__            
            AppExit(APP_ERR_NO_FLASH_CONFIG_FILE, APP_RESULT_TYPE, NULL);
            return;
#endif

            DisplayCenterMsgBox(MSG_BOX_YES_NO, GetMsgBoxTitleString(FLASH_CONFIGURATION_FILE_IS_MISSING), 0, 0);
            PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, GetAppTextString(FLASH_CONFIGURATION_FILE_IS_MISSING));
            if (waitForYesNo())
            {
                createDefaultFlashCfgFile(p_appOptions);
            }
            else
            {
                AppExit(APP_ERR_NO_FLASH_CONFIG_FILE, APP_RESULT_TYPE, NULL);
            }
        }
        else
        {
            fclose(l_configFileHandle);
        }

    }

#ifdef __LINUX__
static void createDefaultFlashCfgFile(AppOptions *p_appOptions)
{
    AppExit(APP_ERR_CREATING_DEFAULT_CFG_FILE, APP_RESULT_TYPE, NULL);
}
#else
    /*--------------------------------------------------------------------------------------*/
    /* Function: createDefaultFlashCfgFile                                                  */
    /*                                                                                      */ 
    /* Parameters:   p_appOptions - Application options                                     */
    /*                                                                                      */
    /* Returns:      None                                                                   */
    /*                                                                                      */ 
    /* Side effects: None                                                                   */
    /*                                                                                      */ 
    /* Description:                                                                         */ 
    /*               Create a default configuration file                                    */
    /*--------------------------------------------------------------------------------------*/

    static void createDefaultFlashCfgFile(AppOptions *p_appOptions)
    {
        FILE          *l_configFileHandle = NULL;
        size_t         l_fileSize         = 0;
        
        char          *l_filePtr          = NULL;
        unsigned long  i                  = 0;
        
        

           
        if( (l_configFileHandle = fopen(DEFAULT_FLASH_CONFIG_FILE_MANE, "w+t" )) == NULL )
        {
            AppExit(APP_ERR_CREATING_DEFAULT_CFG_FILE, APP_RESULT_TYPE, NULL); 
        }

        l_filePtr = getAppendixFilePtr(p_appOptions->AppExeName,
                                       &l_fileSize,
                                       AEPL_FLASH_CONFIGURATION);
        
        for (i = 0; i < l_fileSize; i++)
        {

            /*---------------------------------------------------------------------------------------*/
            /* When using fwrite command, ascii 10 is transferd                                      */
            /* to two characters - 13 and 10 so we need to eliminate the                             */
            /* original ascii 13 char                                                                */
            /*---------------------------------------------------------------------------------------*/
            
            if (13 != l_filePtr[i])
            {
                fwrite((l_filePtr + i), 1, 1, l_configFileHandle);
            }
            
        }
        
        fclose(l_configFileHandle);
        free(l_filePtr);
    }
#endif // __LINUX__
#endif


/*-----------------------------------------------------------------------------------------------*/
/* Function: loadDefaultAppOptions                                                               */
/*                                                                                               */
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      The application default options                                                 */
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */
/* Description:                                                                                  */
/*               Load the default application options                                            */
/*-----------------------------------------------------------------------------------------------*/

static AppOptions *loadDefaultAppOptions(char *p_appExeName)
{
    AppOptions *l_appOptions = NULL;



    l_appOptions = (AppOptions*)(malloc(sizeof(AppOptions)));

    strcpy(l_appOptions->AppExeName, p_appExeName);
    l_appOptions->CommandLine[0]         = 0;
    l_appOptions->ConfigFileName[0]      = 0;
    l_appOptions->BinaryFileName[0]      = 0;    
    l_appOptions->IgnoreSpecialSections  = 0;
    l_appOptions->IgnoreSystem           = 0;
    l_appOptions->IgnoreVersion          = 0;
    l_appOptions->NoPause                = 0;
    l_appOptions->NoReboot               = 0;
    l_appOptions->WipeClean              = 0;
    l_appOptions->WipeAll                = 0;
    l_appOptions->Dump                   = 0;
    l_appOptions->DumpOnly               = 0;
    l_appOptions->Compare                = 0;
    l_appOptions->IgnoreProtectedSectors = 0;
    l_appOptions->ShowUsage              = 0;
    l_appOptions->Verbose                = 0;
    l_appOptions->DisableLegacyUSB       = 0;
    l_appOptions->DisableGlobalSMI       = 0;
    l_appOptions->ChooseBaseAddress      = 0;        
    l_appOptions->IgnoreBIOSModelCheck   = 0;
    l_appOptions->FlashOffset            = 0;
    l_appOptions->Silent                 = 0;
#ifdef __LINUX__
    l_appOptions->VideoMode              = _UNKNOWN;
#else //DOS
    l_appOptions->VideoMode              = _VRES16COLOR;
#endif

    return l_appOptions;    
}


/*--------------------------------------------------------------------------------------*/
/* Function: parseCommandLine                                                           */
/*                                                                                      */ 
/* Parameters:   p_argc - Number of command line arguments                              */
/*               p_argv - Command line arguments values                                 */
/*               p_appOptions - Will hold the application options structure             */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */ 
/* Side effects: Set m_configFileName and m_binFileName                                 */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Parse the command line                                                 */
/*--------------------------------------------------------------------------------------*/

static FlashUpdateOptions * parseCommandLine(int p_argc,
                                                  char* p_argv[],
                                                  AppOptions *p_appOptions)
                                                  
    
{
    int l_paramNumber;
    int l_flagVal;
    FlashUpdateOptions *l_fuOptions = NULL;
    int l_bytesRead = 0;
       



    l_fuOptions = GetDefaultFlashUpdateOptions();

    
#ifdef COMMON_DOS_DEFAULT_EXIT_TYPE_BOOT_BLOCK
    
        l_fuOptions->ExitType = WCB_EXIT_GOTO_BOOT_BLOCK;
    
#endif


#ifdef COMMON_SHOW_README_WHEN_NO_PARAMETERS
    
    if (1 == p_argc)
    {
        p_appOptions->ShowUsage = 1;
    }

#endif

    for (l_paramNumber = 1; l_paramNumber < p_argc; l_paramNumber++)
    {   
        strcat(p_appOptions->CommandLine, " ");
        strcat(p_appOptions->CommandLine, p_argv[l_paramNumber]);
        
        if (strcmpi(p_argv[l_paramNumber], "-h") == 0) {       
            p_appOptions->ShowUsage = 1;
        } 

    #ifdef COMMON_FLAG_CONFIG

        else if (strcmpi(p_argv[l_paramNumber], "-config") == 0) {            
            l_fuOptions->SkipConfig = FALSE;
        }  
        
    #endif

    
    #ifdef COMMON_FLAG_V

        else if (strcmpi(p_argv[l_paramNumber], "-v") == 0) {            
            l_fuOptions->Verbose = 1;
            p_appOptions->Verbose = 1;
            SetLogLevel(2);
            SetErrorReportLevel(REPORT_ALL_ERRORS);
        }

    #endif


    #ifdef COMMON_FLAG_BBL

        else if (strcmpi(p_argv[l_paramNumber], "-bbl") == 0) {
            p_appOptions->IgnoreProtectedSectors = 1;
        }
        
    #endif

    #ifdef COMMON_FLAG_RFP

        else if (strcmpi(p_argv[l_paramNumber], "-rfp") == 0) {           
            l_fuOptions->RemoveFlashProtection = TRUE;
        }
        
    #endif

    #ifdef COMMON_FLAG_UP

        else if (strcmpi(p_argv[l_paramNumber], "-up") == 0) {           
            l_fuOptions->UseProtectionWindow = TRUE;
        }
        
    #endif

    #ifdef COMMON_FLAG_LPC

        else if (strcmpi(p_argv[l_paramNumber], "-lpc") == 0) {
            l_fuOptions->SharedMemoryInterface = SHM_MEM_LPC;
        }

    #endif

    #ifdef COMMON_FLAG_COMPARE

        else if (strcmpi(p_argv[l_paramNumber], "-compare") == 0) {
            p_appOptions->Compare = 1;
        }

    #endif

    #ifdef COMMON_FLAG_NOVR
    
        else if (strcmpi(p_argv[l_paramNumber], "-novr") == 0) {
            p_appOptions->IgnoreBIOSModelCheck = 1;
        }
            
    #endif

    #ifdef COMMON_FLAG_OFS

        else if (strcmpi(p_argv[l_paramNumber], "-ofs") == 0) {
            p_appOptions->FlashOffset = 1;
            l_paramNumber++;
            if (l_paramNumber < p_argc)
            {                
                l_bytesRead = sscanf(p_argv[l_paramNumber], "%lx", &l_fuOptions->FlashOffset);
                if ((l_bytesRead < 1) || (l_bytesRead > 4) || l_fuOptions->FlashOffset < 0)
                {
                    AppExit(APP_ERR_ILLEGAL_OFFSET_VALUE, APP_RESULT_TYPE, p_argv[l_paramNumber]);
                }                 
            }
            else
            {
                AppExit(APP_ERR_MISSING_OFFSET_VALUE, APP_RESULT_TYPE, p_argv[l_paramNumber - 1]);
            }
        }

    #endif
    
    #ifdef COMMON_FLAG_CBA

        else if (strcmpi(p_argv[l_paramNumber], "-cba") == 0) {
            p_appOptions->ChooseBaseAddress = 1;
            l_paramNumber++;
            if (l_paramNumber < p_argc)
            {
                l_fuOptions->UseUserBaseAddress = 1;
                l_bytesRead = sscanf(p_argv[l_paramNumber], "%x", &l_fuOptions->BaseAddress);
                if ((l_bytesRead < 1) || (l_bytesRead > 4) || l_fuOptions->BaseAddress < 0)
                {
                    AppExit(APP_ERR_ILLEGAL_BASE_ADDRESS, APP_RESULT_TYPE, p_argv[l_paramNumber]);
                }
            }
            else
            {
                AppExit(APP_ERR_BASE_ADDRESS_NOT_SPECIFIED, APP_RESULT_TYPE, p_argv[l_paramNumber - 1]);
            }
        }

    #endif

     #ifdef COMMON_FLAG_GENBSF

        else if (strcmpi(p_argv[l_paramNumber], "-genbsf") == 0) {
            l_paramNumber++;
            if (l_paramNumber < p_argc)
            {
                l_fuOptions->GenerateBSF = 1;
       			strcpy(p_appOptions->TsfName, p_argv[l_paramNumber]);                
            }
            else
            {
                AppExit(APP_ERR_TSF_NOT_SPECIFIED, APP_RESULT_TYPE, p_argv[l_paramNumber - 1]);
            }
        }

    #endif

    #ifdef COMMON_FLAG_RUNBSF

        else if (strcmpi(p_argv[l_paramNumber], "-runbsf") == 0) {
            l_paramNumber++;
            if (l_paramNumber < p_argc)
            {
                l_fuOptions->RunBSF= 1;
       			strcpy(p_appOptions->BsfName, p_argv[l_paramNumber]);                
            }
            else
            {
                AppExit(APP_ERR_BSF_NOT_SPECIFIED, APP_RESULT_TYPE, p_argv[l_paramNumber - 1]);
            }
        }

    #endif

    #ifdef COMMON_FLAG_VM

        else if (strcmpi(p_argv[l_paramNumber], "-vm") == 0) {
            l_paramNumber++;
            if (l_paramNumber < p_argc)
            {
                l_bytesRead = sscanf(p_argv[l_paramNumber], "%d", (int *)&p_appOptions->VideoMode);
                if ((l_bytesRead < 1) || (l_bytesRead > 4))
                {
                    AppExit(APP_ERR_ILLEGAL_BASE_ADDRESS, APP_RESULT_TYPE, p_argv[l_paramNumber]);
                }
            }
            else
            {
                AppExit(APP_ERR_BASE_ADDRESS_NOT_SPECIFIED, APP_RESULT_TYPE, p_argv[l_paramNumber - 1]);
            }
        }

    #endif

    #ifdef COMMON_FLAG_DUMP
    
        else if (strcmpi(p_argv[l_paramNumber], "-d") == 0) {
            p_appOptions->Dump = 1;
            l_paramNumber++;
            if (l_paramNumber < p_argc)
            {
                strcpy(p_appOptions->DumpFileName, p_argv[l_paramNumber]); 
            }
            else
            {
                AppExit(APP_ERR_DUMP_FILE_NAME_NOT_SPECIFIED, APP_RESULT_TYPE, p_argv[l_paramNumber - 1]);
            }
        }
    
    #endif

        else if (strcmpi(p_argv[l_paramNumber], "-sad") == 0) {
            p_appOptions->DumpOnly= 1;
            l_paramNumber++;
            if (l_paramNumber < p_argc)
            {
                strcpy(p_appOptions->DumpFileName, p_argv[l_paramNumber]); 
            }
            else
            {
                AppExit(APP_ERR_DUMP_FILE_NAME_NOT_SPECIFIED, APP_RESULT_TYPE, p_argv[l_paramNumber - 1]);
            }
        }

    #ifdef COMMON_FLAG_FACTORY

        else if (strcmpi(p_argv[l_paramNumber], "-factory") == 0) {
            p_appOptions->NoPause = TRUE;
            l_fuOptions->ExitType = WCB_EXIT_NORMAL;
        }
    #endif


    #ifdef COMMON_FLAG_FORCEIT

        else if (strcmpi(p_argv[l_paramNumber], "-forceit") == 0) {
            p_appOptions->IgnoreVersion = TRUE;
        }
        
    #endif

    #ifdef COMMON_FLAG_NOREBOOT

        else if (strcmpi(p_argv[l_paramNumber], "-noreboot") == 0) {
            p_appOptions->NoReboot = 1;
        }

    #endif

    #ifdef COMMON_FLAG_FORCETYPE

        else if (strcmpi(p_argv[l_paramNumber], "-forcetype") == 0) {
            p_appOptions->IgnoreSystem = TRUE;
        }
        
    #endif

    #ifdef COMMON_FLAG_NOPAUSE

        else if (strcmpi(p_argv[l_paramNumber], "-nopause") == 0) {
            p_appOptions->NoPause = TRUE;
        }

    #endif

    #ifdef COMMON_FLAG_WIPEALL

        else if (strcmpi(p_argv[l_paramNumber], "-wipeall") == 0) {
            p_appOptions->WipeAll = TRUE;        
        }

    #endif
    
    #ifdef COMMON_FLAG_WIPECLEAN

        else if (strcmpi(p_argv[l_paramNumber], "-wipeclean") == 0) {
            p_appOptions->WipeClean = TRUE;
        }

    #endif

    #ifdef COMMON_FLAG_VRD

        else if (strcmpi(p_argv[l_paramNumber], "-vrd") == 0) {
            l_fuOptions->VerifyByRead = 1;
        }
            
    #endif

    #ifdef COMMON_FLAG_PFU

        else if (strcmpi(p_argv[l_paramNumber], "-pfu") == 0) {
            l_fuOptions->PreFlashUpdate = 1;
        }

    #endif

    #ifdef COMMON_FLAG_OCV

        else if (strcmpi(p_argv[l_paramNumber], "-ocv") == 0) {
            l_fuOptions->OverwriteCalibrationValues = TRUE;
        }
           
    #endif
    
    #ifdef COMMON_FLAG_SHF

        else if (strcmpi(p_argv[l_paramNumber], "-shf") == 0) {
            l_fuOptions->SharedBIOS = 1;
        }
            
    #endif

    #ifdef COMMON_FLAG_NSHF

        else if (strcmpi(p_argv[l_paramNumber], "-nshf") == 0) {
            l_fuOptions->NotSharedBIOS = 1;
        }
        
    #endif

    #ifdef COMMON_FLAG_NOCOMP
        
        else if (strcmpi(p_argv[l_paramNumber], "-nocomp") == 0) {
            l_fuOptions->NoCompare = 1;
        }
        
    #endif

    #ifdef COMMON_FLAG_NOVERIFY

        else if (strcmpi(p_argv[l_paramNumber], "-noverify") == 0) {
            l_fuOptions->NoVerify = 1;
        }
        
    #endif

    #ifdef COMMON_FLAG_NT

        else if (strcmpi(p_argv[l_paramNumber], "-nt") == 0) {
            l_fuOptions->NoTimeout = 1;
        }
        
    #endif

    #ifdef COMMON_FLAG_FORCESECTIONS

        else if (strcmpi(p_argv[l_paramNumber], "-forcesections") == 0) {
            p_appOptions->IgnoreSpecialSections = TRUE;
        }
        
    #endif


    #ifdef COMMON_FLAG_DISLUSB

        else if (strcmpi(p_argv[l_paramNumber], "-dislusb") == 0) {
            p_appOptions->DisableLegacyUSB = TRUE;
        }
           
    #endif

    #ifdef COMMON_FLAG_DISSMI

        else if (strcmpi(p_argv[l_paramNumber], "-dissmi") == 0) {
            p_appOptions->DisableGlobalSMI = TRUE;
        }
           
    #endif

    #ifdef COMMON_FLAG_BEC

        else if (strcmpi(p_argv[l_paramNumber], "-bec") == 0) {
            SetExitCodeType(EXIT_CODE_BINARY);
        }
           
    #endif

    #ifdef COMMON_FLAG_C

        else if (strcmpi(p_argv[l_paramNumber], "-c") == 0){        
            l_paramNumber++;
            if (l_paramNumber < p_argc)
            {
                strcpy(p_appOptions->ConfigFileName, p_argv[l_paramNumber]); 
            }   
            else
            {
                AppExit(APP_ERR_FLASH_CONFIG_FILE_NOT_SPECIFIED, APP_RESULT_TYPE, p_argv[l_paramNumber - 1]);
            }
        }
        
    #endif

    #ifdef COMMON_FLAG_X

        else if (strcmpi(p_argv[l_paramNumber], "-x") == 0)
        {
            l_paramNumber++;
            if (sscanf(p_argv[l_paramNumber], "%d", &l_flagVal) != 1)
            {
                AppExit(APP_ERR_CAN_NOT_READ_EXIT_TYPE, APP_RESULT_TYPE, NULL);
            }
            if (l_flagVal >= WCB_EXIT_LAST)
            {
                AppExit(APP_ERR_ILLEGAL_EXIT_TYPE, APP_RESULT_TYPE, p_argv[l_paramNumber]);
            }
            else
            {
                l_fuOptions->ExitType = l_flagVal;
            }
        }

    #endif

    #ifdef COMMON_RECEIVE_FLASH_BIN_IMG_FROM_USER

        else if (p_argv[l_paramNumber][0] != '-')
        {
            strcpy(p_appOptions->BinaryFileName, p_argv[l_paramNumber]);           
        }
        
    #endif
   
        else
        {                     
            AppExit(APP_ERR_UNRECOGNIZED_PARAMETER, APP_RESULT_TYPE, p_argv[l_paramNumber]);
        }
    }
    
    return l_fuOptions;
    
}

#ifndef __LINUX__
static char * AttachExeExtension(char *p_fileName)
{   
    char *l_exeFileName = (char*)(malloc(sizeof(char) * FILENAME_MAX));



    strcpy(l_exeFileName, p_fileName);
    strlwr(l_exeFileName);
    if (NULL == strstr(l_exeFileName,".exe"))
    {
        strcpy(l_exeFileName, p_fileName);
        strcat(l_exeFileName, ".exe");
        return l_exeFileName;
    }
    else
    {
        return p_fileName;
    }
    
}
#endif

/*--------------------------------------------------------------------------------------*/
/* Function: OldVersionWarning                                                          */
/*                                                                                      */ 
/* Parameters:   p_flashVer - The flash BIOS version                                    */
/*               p_binVer   - The binary file BIOS version                              */
/*                                                                                      */
/* Returns:      0 - User selected not to update                                        */
/*               Otherwise the user select to update the BIOS                           */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Ask the user to continue on with the update process although           */
/*               a newer/same BIOS already installed on the computer                    */
/*--------------------------------------------------------------------------------------*/

int oldVersionWarning(char *p_flashVer, char *p_binVer)
{
    DisplayCenterMsgBox(MSG_BOX_YES_NO, "Old Version Warning", 400, 0);
    PrintCenterMsgBoxText(0, TEXT_ALIGN_LEFT, "Current BIOS version: %s", p_flashVer);
    PrintCenterMsgBoxText(1, TEXT_ALIGN_LEFT, "You are about to install version: %s", p_binVer);
    PrintCenterMsgBoxText(2, TEXT_ALIGN_LEFT, "Continue with the installation process?");
    return waitForYesNo();
}


