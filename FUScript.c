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

#include "FUScript.h"

#ifdef __LINUX__
#define strcmpi strcmp
#endif
/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                             LOCAL FUNCTION DECLARATION                               */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

FU_RESULT           SCR_Init();
FU_RESULT           SCR_ExtractData( );
FU_RESULT           SCR_RunCommands(int p_section);
FU_RESULT           SCR_ReadBSF(char* bsfName);
FU_RESULT           SCR_WriteBSF();
void                SCR_SetCommandsExecution(BOOLEAN p_commandsExecution);
static char*        SCR_GetNextSection(char *p_scrFilePtr, 
                                            size_t *p_scrFileSize, 
                                            const char * sectionName);
static int          SCR_GetKeyword(char *p_keyword, 
                                         int *p_line, 
                                         char *p_scrFilePtr, 
                                         size_t p_scrFileSize,
                                         BOOLEAN *p_isLastParam);
static FU_RESULT    SCR_AssignValue(char* p_val,
                                         void* p_ptr,
                                         int   p_size);

static FU_RESULT    SCR_ParseCommand(CMD_TYPE cmdType, 
                                             char *scrFilePtr, 
                                             size_t scrFileSize, 
                                             CmdParameters *cmdParams);
static BOOLEAN      SCR_LastParameter(char *p_scrFilePtr, size_t length);
static char*        SCR_OpenInputFile(char *p_scrFileName, size_t *p_scrFileSize);
static char *       SCR_MapFileToMemory(char *p_fileName, size_t *p_fileSize);
static FU_RESULT    SCR_ParseTSF(char* tsfName);
static FU_RESULT    SCR_RunSingleCommand(CmdParameters* cmdParams);
static FU_RESULT    SCR_RunIntAssemblyCmd(Int_CmdParams* cmdParams);

#ifdef _64BIT_APP   /* x64 Compiler doesn't support inline assembly so use an external .asm file */
    extern void     IntFunction(unsigned short shortArr[9], unsigned char byteArr[9], char moveToRegArr[9]);
#endif

    
/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                  LOCAL VARIABLES                                     */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/*  Array to hold the parameters of the commands to be executed                         */
/*--------------------------------------------------------------------------------------*/
static CmdParameters SCR_CmdParams[SCR_MAX_NUM_OF_SECTIONS][CMD_PARAMS_ARRAY_SIZE];

/*--------------------------------------------------------------------------------------*/
/*  Keys for the commands and their relevant handling function                          */
/*--------------------------------------------------------------------------------------*/
static const scrKeyword_t SCR_Keys[NUM_OF_SCR_KEYWORDS] = {
    {"MEMW", CMD_MEMW, SCR_ParseCommand, 0},
    {"IOW",  CMD_IOW,  SCR_ParseCommand, 0},       
    {"PCIW", CMD_PCIW, SCR_ParseCommand, 0},        
    {"INT", CMD_INT, SCR_ParseCommand, 0},
    {"DELAY", CMD_DELAY, SCR_ParseCommand, 0},        
    } ;

/*--------------------------------------------------------------------------------------*/
/*  Boolean to decide whether the commands in SCR_CmdParams needs to be executed        */
/*--------------------------------------------------------------------------------------*/
BOOLEAN SCR_CommandsExecution;

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                             FUNCTION IMPLEMENTATION                                  */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Function: SCR_Init                                                                   */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Inits SCR variables                                                    */
/*--------------------------------------------------------------------------------------*/
FU_RESULT SCR_Init()
{
    FU_RESULT l_retVal = FU_RESULT_OK;
    int i = 0, j = 0;

    PrintText(APP_VERBOSE_PRINT, "SCR: In SCR_Init() \n");

    SCR_CommandsExecution = FALSE;

    /*--------------------------------------------------------------------------------------*/
    /*  Init SCR_CmdParams, set default Mask to 0xFFFFFFFF                                  */
    /*--------------------------------------------------------------------------------------*/
    memset(SCR_CmdParams,0,sizeof(SCR_CmdParams));
    for(i=0;i<SCR_MAX_NUM_OF_SECTIONS;i++)
   {
       for(j=0;j<CMD_PARAMS_ARRAY_SIZE;j++)
       {
            SCR_CmdParams[i][j].PciMemIO_CmdParams.Mask = 0xFFFFFFFF; 
       }       
   }

    return l_retVal;
    
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SCR_SetCommandsExecution                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_commandsExecution - boolean value to be set to the global variable                   */
/*                  SCR_CommandsExecution                                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Sets a value into SCR_CommandsExecution                                                */
/*---------------------------------------------------------------------------------------------------------*/
void SCR_SetCommandsExecution(BOOLEAN p_commandsExecution)
{
    PrintText(APP_VERBOSE_PRINT, "SCR: In SCR_SetCommandsExecution() \n");
    
    SCR_CommandsExecution = p_commandsExecution;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SCR_RunCommands                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_section - the section to be run                                                      */
/*                                                                                                         */
/* Returns:         FU_RESULT                                                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Runs the commands in the provided section                                              */
/*---------------------------------------------------------------------------------------------------------*/
FU_RESULT SCR_RunCommands(int p_section)
{

    FU_RESULT l_retVal = FU_RESULT_OK;
    int       i        = 0;   

    PrintText(APP_VERBOSE_PRINT, "SCR: In SCR_RunCommands() \n");
    
    if(FALSE == SCR_CommandsExecution)
    {
        return l_retVal;
    }
    
    if(p_section >= SCR_MAX_NUM_OF_SECTIONS)
    {
        return FU_ERR_SCR_INVALID_SECTION;
    }


    for(i = 0; i < CMD_PARAMS_ARRAY_SIZE; i++)
    {
        if(SCR_CmdParams[p_section][i].PciMemIO_CmdParams.Cmd > CMD_INVALID_START &&
           SCR_CmdParams[p_section][i].PciMemIO_CmdParams.Cmd < CMD_INVALID_END )
        {           
            l_retVal = SCR_RunSingleCommand(&SCR_CmdParams[p_section][i]);
            if (l_retVal != FU_RESULT_OK)
            {
           /*----------------------------------------------------------------------------------*/
           /* stop execution and report an error if an error occured while executing command                                 */
           /*----------------------------------------------------------------------------------*/
                break;
            }
            
        }
        else
        {
           /*----------------------------------------------------------------------------------------------*/
           /* stop execution upon detection of invalid command. Return value is still FU_RESULT_OK                                            */
           /*----------------------------------------------------------------------------------------------*/
            break;
        }
    }

    return l_retVal;
    
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SCR_RunSingleCommand                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cmdParams - the parameters of the command to be run                                    */
/*                                                                                                         */
/* Returns:         FU_RESULT                                                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Runs one command                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
static FU_RESULT SCR_RunSingleCommand(CmdParameters* cmdParams)
{
    FU_RESULT           l_retVal = FU_RESULT_OK;
    unsigned char       l_byteData;
    unsigned short int  l_wordData;
    unsigned long int   l_dwordData;
    PciMemIO_CmdParams* l_PciMemIOCmdParams = &cmdParams->PciMemIO_CmdParams;

    PrintText(APP_VERBOSE_PRINT, "SCR: In SCR_RunSingleCommand() \n");
    
    /*-----------------------------------------------------------------------------------------------------*/
    /*  Algorithm for calculating the data to be written:                                                  */
    /*  Data to write = (Current Value & ~Mask) | (Input Data & Mask)                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    switch(l_PciMemIOCmdParams->Cmd)
    {        
        case CMD_IOW:
        case CMD_MEMW:            
            switch(l_PciMemIOCmdParams->Width)
            {
                case 1:                   
                    if(CMD_IOW == l_PciMemIOCmdParams->Cmd)
                    {
                        l_byteData = PcIoReadB(l_PciMemIOCmdParams->Address);
                    }
                    else
                    {
                        l_byteData = PcMemReadB(l_PciMemIOCmdParams->Address);
                    }
                    l_byteData = l_byteData & (~((unsigned char)l_PciMemIOCmdParams->Mask)); 
                    l_byteData |= (l_PciMemIOCmdParams->Data & (unsigned char)l_PciMemIOCmdParams->Mask);  
                    if(CMD_IOW == l_PciMemIOCmdParams->Cmd)
                    {
                        PcIoWriteB(l_PciMemIOCmdParams->Address,l_byteData);
                    }
                    else
                    {
                        PcMemWriteB(l_PciMemIOCmdParams->Address,l_byteData);
                    }
                    break;
                case 2:
                    if(CMD_IOW == l_PciMemIOCmdParams->Cmd)
                    {
                        l_wordData = PcIoReadW(l_PciMemIOCmdParams->Address);
                    }
                    else
                    {
                        l_wordData = PcMemReadW(l_PciMemIOCmdParams->Address);
                    }
                    l_wordData = l_wordData & (~((unsigned short)l_PciMemIOCmdParams->Mask)); 
                    l_wordData |= (l_PciMemIOCmdParams->Data & (unsigned short)l_PciMemIOCmdParams->Mask);  
                    if(CMD_IOW == l_PciMemIOCmdParams->Cmd)
                    {
                        PcIoWriteW(l_PciMemIOCmdParams->Address,l_wordData);
                    }
                    else
                    {
                        PcMemWriteW(l_PciMemIOCmdParams->Address,l_wordData);
                    }
                    break;
                case 4:
                    if(CMD_IOW == l_PciMemIOCmdParams->Cmd)
                    {
                        l_dwordData = PcIoReadD(l_PciMemIOCmdParams->Address);
                    }
                    else
                    {
                        l_dwordData = PcMemReadD(l_PciMemIOCmdParams->Address);
                    }
                    l_dwordData = l_dwordData & (~((unsigned long)l_PciMemIOCmdParams->Mask)); 
                    l_dwordData |= (l_PciMemIOCmdParams->Data & (unsigned long)l_PciMemIOCmdParams->Mask);  
                    if(CMD_IOW == l_PciMemIOCmdParams->Cmd)
                    {
                        PcIoWriteD(l_PciMemIOCmdParams->Address,l_dwordData);
                    }
                    else
                    {
                        PcMemWriteD(l_PciMemIOCmdParams->Address,l_dwordData);
                    }
                    break;
                default:
                    break;
            }
            break;
        case CMD_PCIW:
            l_dwordData = ReadPCI(l_PciMemIOCmdParams->Bus, l_PciMemIOCmdParams->Device, l_PciMemIOCmdParams->Function, l_PciMemIOCmdParams->Register);
            l_dwordData = l_dwordData & ~l_PciMemIOCmdParams->Mask; 
            l_dwordData |= (l_PciMemIOCmdParams->Data & l_PciMemIOCmdParams->Mask);  
            WritePCI(l_PciMemIOCmdParams->Bus,l_PciMemIOCmdParams->Device, l_PciMemIOCmdParams->Function, l_PciMemIOCmdParams->Register, l_dwordData);
            break;         
        case CMD_INT:
            l_retVal = SCR_RunIntAssemblyCmd(&cmdParams->Int_CmdParams);
          break;
        case CMD_DELAY:
            SLEEP(cmdParams->Delay_CmdParams.Delay);
            l_retVal = FU_RESULT_OK;
          break;

        default:
            l_retVal = FU_ERR_SCR_INVALID_COMMAND;           
    }

    return l_retVal;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        runIntAssemblyCmd                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cmdParams - the parameters of the command to be run                                    */
/*                                                                                                         */
/* Returns:         FU_RESULT                                                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs the INT command using inline assembly:                           */
/*                    At max, 3 actions of type:        MOV REG,MEMORY                                     */
/*                    and a single interrupt command:   INT 15h                                            */
/*                                                                                                         */
/*                  for every register that should be written the function sets:                           */
/*                      a) '1' to the matching cell in the l_moveToRegisterArr                             */
/*                      b) The value to be written to the matching cell in either                          */
/*                          l_valueShortArr\l_valueByteArr                                                 */
/*                                                                                                         */
/*                  The assembly lines would check the l_moveToregister array and if the value is 1,       */
/*                  write to the matching register the given value.                                        */
/*                                                                                                         */
/*                  A seperation between x64 and Win32\Dos is made,                                        */
/*                  because x64 doesn't support inline assembly                                            */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static FU_RESULT SCR_RunIntAssemblyCmd(Int_CmdParams* cmdParams)
{
    int                 i;
    unsigned short      l_valueShortArr[9] = {0};
    unsigned char       l_valueByteArr[9] = {0};
    char                l_moveToRegisterArr[9] = {0};
    int                 curRegister;

   /*------------------------------------------------------------------------------------------------------*/
   /*  Generate a boolean table indicating which Register should be in the MOV commands                    */
   /*------------------------------------------------------------------------------------------------------*/
    for (i = 0; i < NUM_OF_SCR_INT_REGISTERS; ++i)
    {
        curRegister = cmdParams->RegisterAndValues[i].Register;
        if (ILLEGAL_REGISTER == curRegister)                        /* No more register to process*/    
        {
            break;
        }
        l_moveToRegisterArr[curRegister] = 1;
        l_valueShortArr[curRegister] = cmdParams->RegisterAndValues[i].Value;
        l_valueByteArr[curRegister] = (char) cmdParams->RegisterAndValues[i].Value;        
    }
    
   /*------------------------------------------------------------------------------------------------------*/
   /*  if x64 - use external asm because x64 compiler doesn't support inline assembly.                     */
   /*  Use inline assembly otherwise (win32 \ dos)                                                          */
   /*------------------------------------------------------------------------------------------------------*/
#ifdef _64BIT_APP                                           
        IntFunction(l_valueShortArr, l_valueByteArr, l_moveToRegisterArr);
        return FU_RESULT_OK;
#else

       /*------------------------------------------------------------------------------------------------------*/
       /*  Run assembly code according to the boolean table                                                    */
       /*------------------------------------------------------------------------------------------------------*/
#ifdef __LINUX__
#if 0
        __asm__( "cmp $1, %0 ;"
                 "jne CHECK_AH ;"
                 "mov %%ax, %1 ;"
        "CHECK_AH: ;"
                 "cmp $1, %2 ;"
                 "jne CHECK_AL ;"
                 "mov %%ah, %3 ;"
        "CHECK_AL: ;"  
                 "cmp $1, %4 ;"
                 "jne CHECK_BX ;"
                 "mov %%al, %5 ;"
        "CHECK_BX: ;"
                 "cmp $1, %6 ;"
                 "jne CHECK_BH ;"
                 "mov %%bx, %7 ;"
        "CHECK_BH: ;"
                 "cmp $1, %8 ;"
                 "jne CHECK_BL ;"
                 "mov %%bh, %9 ;"
        "CHECK_BL: ;"
                 "cmp $1, %10 ;"
                 "jne CHECK_CX ;"
                 "mov %%bl, %11 ;"
        "CHECK_CX: ;"
                 "cmp $1, %12 ;"
                 "jne CHECK_CH ;"
                 "mov %%cx, %13 ;"
        "CHECK_CH: ;"
                 "cmp $1, %14 ;"
                 "jne CHECK_CL ;"
                 "mov %%ch, %15 ;"
        "CHECK_CL: ;"  
                 "cmp $1, %16 ;"
                 "jne INTERRUPT ;"
                 "mov %%cl, %17 ;"
        "INTERRUPT: ;"
                 "int $0x15 ;"
           : /* no outputs */
           : "m"((char)l_moveToRegisterArr[0]), "m"((short)l_valueShortArr[0]),   // %0 ,  %1
             "m"((char)l_moveToRegisterArr[1]), "m"((short)l_valueByteArr[1]),   // %2 ,  %3
             "m"((char)l_moveToRegisterArr[2]), "m"((char)l_valueByteArr[2]),     // %4 ,  %5             
             "m"((char)l_moveToRegisterArr[3]), "m"((short)l_valueShortArr[3]), // %6 ,  %7
             "m"((char)l_moveToRegisterArr[4]), "m"((char)l_valueByteArr[4]),    // %8 ,  %9
             "m"((char)l_moveToRegisterArr[5]), "m"((char)l_valueByteArr[5]),    //%10 , %11
             "m"((char)l_moveToRegisterArr[6]), "m"((short)l_valueShortArr[6]), //%12 , %13
             "m"((char)l_moveToRegisterArr[7]), "m"((char)l_valueByteArr[7]),    //%14 , %15
             "m"((char)l_moveToRegisterArr[8]), "m"((char)l_valueByteArr[8])     //%16 , %17         
        );
#endif // 0
#else // DOS
        __asm {         
                    cmp l_moveToRegisterArr[0],1h
                    jne CHECK_AH
                    mov AX, l_valueShortArr[0]
        CHECK_AH:
                    cmp l_moveToRegisterArr[1], 1h
                    jne CHECK_AL
                    mov AH, l_valueByteArr[1]
        CHECK_AL:  
                    cmp l_moveToRegisterArr[2], 1h
                    jne CHECK_BX
                    mov AL, l_valueByteArr[2]
        CHECK_BX:  
                    cmp l_moveToRegisterArr[3], 1h
                    jne CHECK_BH
                    mov BX, l_valueShortArr[3*2]            /* Multiply SHORT address index by 2 */
        CHECK_BH:  
                    cmp l_moveToRegisterArr[4], 1h
                    jne CHECK_BL
                    mov BH, l_valueByteArr[4]
        CHECK_BL:  
                    cmp l_moveToRegisterArr[5], 1h
                    jne CHECK_CX
                    mov BL, l_valueByteArr[5]
        CHECK_CX:  
                    cmp l_moveToRegisterArr[6], 1h
                    jne CHECK_CH
                    mov CX, l_valueShortArr[6*2]            /* Multiply SHORT address index by 2 */
        CHECK_CH:  
                    cmp l_moveToRegisterArr[7], 1h
                    jne CHECK_CL
                    mov CH, l_valueByteArr[7]
        CHECK_CL:  
                    cmp l_moveToRegisterArr[8], 1h
                    jne INTERRUPT
                    mov CL, l_valueByteArr[8]
        INTERRUPT: 
                    int 15h
        };
#endif // DOS

    return FU_RESULT_OK;

#endif // _64BIT_APP

}

/*--------------------------------------------------------------------------------------*/
/* Function: SCR_MapFileToMemory                                                        */
/*                                                                                      */ 
/* Parameters:   p_fileName - File name to be mapped                                    */
/*               p_fileSize - Will hold the file size                                   */
/*                                                                                      */
/* Returns:      Pointer to the file                                                    */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Maps the given file into the memory                                    */
/*--------------------------------------------------------------------------------------*/

static char * SCR_MapFileToMemory(char *p_fileName, size_t *p_fileSize)
{
    FILE*         l_fileHandle = NULL;
    char*         l_filePtr    = NULL;
    size_t        l_fileSize   = 0;
    size_t        l_bytesRead  = 0;
    
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
/* Function: SCR_OpenInputFile                                                          */
/*                                                                                      */ 
/* Parameters:   p_scrFileName - SCR file name                                          */
/*               p_scrFileSize - Will hold the SCR file size                            */
/*                                                                                      */
/* Returns:      Pointer to the SCR file                                                */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Open an SCR file                                                       */
/*--------------------------------------------------------------------------------------*/

static char* SCR_OpenInputFile(char *p_scrFileName, size_t *p_scrFileSize)
{
    char* l_scrFilePtr = NULL;
    FILE* l_fileHandle = NULL;

    PrintText(APP_VERBOSE_PRINT, "SCR: In SCR_OpenInputFile() \n");
    *p_scrFileSize = 0;

    if (NULL != p_scrFileName)
    {
  
        l_scrFilePtr = SCR_MapFileToMemory(p_scrFileName, p_scrFileSize);
        l_fileHandle = fopen(p_scrFileName, "rb");
        if (NULL == l_fileHandle)
        {
            AppExit(APP_ERR_FILE_NOT_EXIST, APP_RESULT_TYPE, p_scrFileName);
        }
        fclose(l_fileHandle);
        
    }

    return l_scrFilePtr;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SCR_ExtractData                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  tsfName - Text Script File name                                                        */
/*                  bsfName - Binary Script File name                                                      */
/*                                                                                                         */
/* Returns:         FU_RESULT                                                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Extracts the data from the given TSF/BSF                                               */
/*---------------------------------------------------------------------------------------------------------*/
FU_RESULT SCR_ExtractData(char* tsfName, char* bsfName )
{
    size_t        l_fileSize;
    char*         l_filePtr;
    FU_RESULT     l_retVal = FU_RESULT_OK;

    PrintText(APP_VERBOSE_PRINT, "SCR: In SCR_ExtractData() \n");
        
    if(NULL != tsfName)
    {
        l_filePtr = SCR_OpenInputFile(tsfName, &l_fileSize);
        l_retVal = SCR_ParseTSF(tsfName);
    }
    else if(NULL != bsfName)
    {
        SCR_ReadBSF(bsfName);
    }

    return l_retVal;
        
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SCR_ReadBSF                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  bsfName - Binary Script File name                                                      */
/*                                                                                                         */
/* Returns:         FU_RESULT                                                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Reads BSF into SCR_CmdParams                                                           */
/*---------------------------------------------------------------------------------------------------------*/
FU_RESULT SCR_ReadBSF(char* bsfName)
{
    FILE          *l_bsfHandle = NULL;
    size_t         l_fileSize  = 0;   
    FU_RESULT      l_retVal    = FU_RESULT_OK;

    PrintText(APP_VERBOSE_PRINT, "SCR: In SCR_ReadBSF() \n");
    
    if( (l_bsfHandle = fopen(bsfName, "rb" )) == NULL )
    {
        return FU_ERR_SCR_FILE_OPEN_ERROR;
    }
   
    l_fileSize = fread(SCR_CmdParams, sizeof(char), sizeof(SCR_CmdParams), l_bsfHandle);
    fclose(l_bsfHandle);
    
    return l_retVal;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SCR_WriteBSF                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  tsfName - Text Script File (TSF) name. The BSF name is derived from the TSF name       */
/*                                                                                                         */
/* Returns:         FU_RESULT                                                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Dumps SCR_CmdParams into a Binary Script File                                          */
/*---------------------------------------------------------------------------------------------------------*/
FU_RESULT SCR_WriteBSF(char* tsfName)
{
    FU_RESULT      l_retVal    = FU_RESULT_OK;
    FILE*          l_bsfHandle = NULL;
    size_t         l_fileSize  = 0;
    char           bsfName[FILENAME_MAX];
    size_t         l_strSize   = 0;
    
    PrintText(APP_VERBOSE_PRINT, "SCR: In SCR_WriteBSF() \n");

    l_strSize = strlen(tsfName);
    
    if(strcmpi(&tsfName[l_strSize-4], ".tsf") != 0)
    {
        return FU_ERR_SCR_INVALID_TSF_NAME_ERROR;
    }
    strcpy(bsfName, tsfName); 
    bsfName[l_strSize-3] = 'b';
    
    if( (l_bsfHandle = fopen(bsfName, "w+" )) == NULL )
    {
        return FU_ERR_SCR_FILE_OPEN_ERROR;
    }

    l_fileSize = fwrite(SCR_CmdParams, sizeof(char), sizeof(SCR_CmdParams), l_bsfHandle);
    fclose(l_bsfHandle);

    return l_retVal;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SCR_GetNextSection                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_scrFilePtr - pointer to SCR file                                                     */
/*                  p_scrFileSize - pointer to SCR file size (output)                                      */
/*                  sectionName -  name of the section to be found                                         */
/*                                                                                                         */
/* Returns:         pointer to the beginning of the section                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns a pointer to the beginning of the provided section                             */
/*---------------------------------------------------------------------------------------------------------*/
static char* SCR_GetNextSection(char *p_scrFilePtr, size_t *p_scrFileSize, const char * sectionName) 
{
    char *l_nextConfigPtr    = NULL;
    char *l_nextConfigEndPtr = NULL;
    char l_startSectionTag[40];
    char l_endSectionTag[40];

    PrintText(APP_VERBOSE_PRINT, "SCR: SCR_GetNextSection() \n");
    
    l_startSectionTag[0] = '\0';
    strcat(l_startSectionTag,"[START ");
    strcat(l_startSectionTag,sectionName);
    strcat(l_startSectionTag,"]");

    l_endSectionTag[0] = '\0';
    strcat(l_endSectionTag,"[END ");
    strcat(l_endSectionTag,sectionName);
    strcat(l_endSectionTag,"]");

    l_nextConfigPtr = strstr(p_scrFilePtr, l_startSectionTag);

    if (NULL != l_nextConfigPtr)
    {
        l_nextConfigPtr = l_nextConfigPtr + strlen(l_startSectionTag);
        l_nextConfigEndPtr = strstr(l_nextConfigPtr, l_endSectionTag);

        if (NULL == l_nextConfigEndPtr)
        {
            return NULL;                                                    /* Couldn't find an end tag */
        }
    }

    *p_scrFileSize = l_nextConfigEndPtr - l_nextConfigPtr;
        
    return l_nextConfigPtr;    
    
}

/*--------------------------------------------------------------------------------------*/
/* Function: getKeyword                                                                 */
/*                                                                                      */ 
/* Parameters:   p_keyword        - Pointer to the returned keyword                     */
/*               p_line           - Pointer to integer that will hold the line          */
/*                                  number in which the the above keyword was found     */
/*                                                                                      */
/*               p_scrFilePtr     - Pointer to the Script file                          */
/*               p_scrFileSize    - Size of the Script file                             */
/*                                                                                      */ 
/* Returns:      Keyword string lengh , 0 means that no keyward was found               */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Read the configuration file and return the next                        */
/*               available keyword/value.                                               */      
/*               A keyword is any string of characters which does not include:          */
/*                  ' '  - space.                                                       */
/*                  ':'  - equal sign                                                   */
/*                  '\n' - end of line                                                  */
/*                                                                                      */
/*               Ignore comments - any character between '#' and the                    */
/*               end of line is consider as a comment.                                  */                  
/*--------------------------------------------------------------------------------------*/
static int SCR_GetKeyword(char *p_keyword, 
                                int *p_line, 
                                char *p_scrFilePtr, 
                                size_t p_scrFileSize,
                                BOOLEAN *p_isLastParam)
{
    char            l_currentChar  = 0;
    int             l_size         = 0;
    int             l_state        = 0;
    static int      l_lineNumber   = 0;
    static size_t   l_bytesRead    = 0;
    
    if (NULL == p_scrFilePtr)
    {
        /* Reset the function static variables */
        
        l_lineNumber = 0;
        l_bytesRead = 0;
        
        return 0;
    }

    while(l_bytesRead < p_scrFileSize ){

        l_currentChar = p_scrFilePtr[l_bytesRead];        
        l_bytesRead++;
        
        
        if (l_currentChar == 10)
        {
            l_lineNumber++;
        }
        
        if ((l_currentChar == '\t') || (l_currentChar == ':'))
        {
            l_currentChar = ' ';
        }
        
        switch (l_state)
        {
            
            case 0:
                
                if (l_currentChar == '#')
                { 
                    l_state = 1;                                      /* skip Comment             */
                } 
                else if ((l_currentChar != ' ') &&
                         (l_currentChar != 10)  &&
                         (l_currentChar != 13))
                {
                    l_size = 1;
                    p_keyword[0] = l_currentChar;
                    l_state = 2;                                      /* get Key word             */
                }
                break;
                
            case 1:                                                   /* skip Comment             */  
            
                if (l_currentChar == 10)
                {                                                     /* wait for end of line     */  
                    l_state = 0;
                }
                break;
                
            case 2:                                                   /* get Key word             */       
            
                if ((l_currentChar == ' ') || 
                    (l_currentChar == 10)  ||
                    (l_currentChar == 13))
                {  
                    
                    /* wait for end of line */
                    
                    l_state = 0;
                    p_keyword[l_size] = '\0';
                    *p_line = l_lineNumber;                    

                    if (l_currentChar == 10)
                    {
                        *p_line = *p_line - 1;
                    }
                    *p_isLastParam = SCR_LastParameter(&p_scrFilePtr[l_bytesRead], (p_scrFileSize-l_bytesRead));                    
                    return l_size;
                    
                }
                else
                {
                    p_keyword[l_size++] = l_currentChar;
                }
                
                break;

        }
        
    }

    *p_isLastParam = SCR_LastParameter(&p_scrFilePtr[l_bytesRead], (p_scrFileSize-l_bytesRead));
    return l_size;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SCR_LastParameter                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  p_scrFilePtr - pointer to Script file                                                  */
/*                  p_length - length of the Script file                                                   */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs...                                                               */
/*---------------------------------------------------------------------------------------------------------*/
static BOOLEAN SCR_LastParameter(char *p_scrFilePtr, size_t p_length)
{
    unsigned int i = 0;
    for(i=0;i<p_length;i++)
    {
        switch(p_scrFilePtr[i])
        {
            case 10:
            case 13:
            case '#':
                return TRUE;
                break;
            case ' ':
            case '\t':
                break;
            default:
                return FALSE;                        
        }
    }
    return FALSE;
}


/*--------------------------------------------------------------------------------------*/
/* Function: assignValue                                                                */
/*                                                                                      */ 
/* Parameters:   p_val             - Pointer to value                                   */
/*               p_ptr             - Pointer to a variable/field                        */
/*               p_size            - 0 for string value                                 */         
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Assign a value to given variable/field pointer                         */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT SCR_AssignValue(char* p_val,
                                      void* p_ptr,
                                      int   p_size)
{
    int          i        = 0;
    int          j        = 0;
    unsigned int l_number = 0;

    if (p_size == 0)
    {

        /* string */
        
        while(p_val[i])
        {
            if(p_val[i] == ',')
            {
                i++;
            }
            else
            {
                ((char *)p_ptr)[j] = p_val[i]- '0';
                j++;
                i++;
            }

        }
        
        return FU_RESULT_OK;

    }
    
    if((p_val[0] == '0') && (p_val[1] == 'x'))
    {
        i = 2;
        
        while(p_val[i])
        {
            if ((p_val[i] >= 'A') && (p_val[i] <= 'F')) 
            {
                l_number = l_number * 16 + (p_val[i] - 'A') + 10;
            }
            else if ((p_val[i] >= 'a') && (p_val[i] <= 'f')) 
            {
                l_number = l_number * 16 + (p_val[i] - 'a') + 10;
            }
            else if ((p_val[i] >= '0') && (p_val[i] <= '9'))
            {
                l_number = l_number * 16 + (p_val[i] - '0');
            }
            else
            {
                PrintText(APP_ERROR_PRINT, 
                          "ERROR: in Script file: %s is not a proper hexadecimal number. Aborting.\n",
                          p_val);
                
                return FU_ERR_SCR_SYNTAX_ERROR;
            }
            
            i++;
        }
    }
    else
    {
        while(p_val[i])
        {
            if ((p_val[i] >= '0') && (p_val[i] <= '9'))
            {
                l_number = l_number * 10 + (p_val[i] - '0');
            }
            else
            {
                PrintText(APP_ERROR_PRINT,
                          "ERROR: in Script file: %s is not a proper decimal number. Aborting.\n",
                          p_val);
                
                return FU_ERR_SCR_SYNTAX_ERROR;
            }
            
            i++;
        }
    }

    switch (p_size){
        case sizeof(char):
            *((unsigned char *)p_ptr) = (unsigned char )l_number;
            break;
        case sizeof(int):
            *((unsigned int *)p_ptr) = (unsigned int)l_number;
            break;
        case sizeof(short):
            *((unsigned short *)p_ptr) = (unsigned short)l_number;
            break;

    }
    return FU_RESULT_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SCR_ParseCommand                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cmdType     - Command type                                                             */
/*                  scrFilePtr  - Pointer to Script file                                                   */
/*                  scrFileSize - Size of Script file                                                      */
/*                  cmdParams   - Pointer to command's parameters (output)                                 */
/*                                                                                                         */
/* Returns:         FU_RESULT                                                                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Parses the command and fills the cmdParams parameters                                  */
/*---------------------------------------------------------------------------------------------------------*/
static FU_RESULT SCR_ParseCommand(CMD_TYPE cmdType, 
                                         char *scrFilePtr, 
                                         size_t scrFileSize, 
                                         CmdParameters *generalCmdParams)
{
    FU_RESULT                           l_retVal     = FU_RESULT_OK;   
    int                                 l_intRetVal  = 0;
    BOOLEAN                             l_notDone    = TRUE;
    BOOLEAN                             l_lastParam  = FALSE;
    int                                 l_line       = 0;   
    int                                 l_lineRef    = 0; // reference to make sure all the parameters are in the same line
    SCR_CMD_PARAMS_STATE                l_state      = SCR_CMD_PARAMS_WIDTH;
    char                                l_value[80];
    int                                 i;
    int                                 l_intReg     = 0;
    Int_CmdParams*        intCmdParams    = &(generalCmdParams->Int_CmdParams);
    Delay_CmdParams*      delayCmdParams  = &(generalCmdParams->Delay_CmdParams);
    PciMemIO_CmdParams*   cmdParams       = &(generalCmdParams->PciMemIO_CmdParams);
    
    PrintText(APP_VERBOSE_PRINT, "SCR: In SCR_ParseCommand() \n");
    cmdParams->Cmd = cmdType;
    if (CMD_INT == cmdParams->Cmd)
    {
      l_state = SCR_CMD_PARAMS_INT_NUM;
    }
    else if (CMD_DELAY == cmdParams->Cmd)
    {
        l_state = SCR_CMD_PARAMS_DELAY;
    }
    
    while(l_notDone)
    {
        switch(l_state)
        {
            case SCR_CMD_PARAMS_WIDTH: /* Width */
                l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);
                if(0 == l_intRetVal || TRUE == l_lastParam)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_lineRef = l_line;
                 switch(l_value[0]) 
                 {
                      case 'b':   cmdParams->Width = 1;       break;
                      case 'w':   cmdParams->Width = 2;       break;
                      case 'd':   cmdParams->Width = 4;       break;
                      default:    return FU_ERR_SCR_SYNTAX_ERROR;
                 }
                 if(CMD_PCIW == cmdType)
                 {
                    l_state = SCR_CMD_PARAMS_BUS;
                 }
                 else
                 {
                     l_state = SCR_CMD_PARAMS_ADDRESS;
                 }
                break;

            case SCR_CMD_PARAMS_BUS:
                l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);         
                if(0 == l_intRetVal || l_line != l_lineRef || TRUE == l_lastParam)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_intRetVal = SCR_AssignValue(l_value, &cmdParams->Bus, sizeof(cmdParams->Bus));
                if(l_intRetVal != FU_RESULT_OK)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_state = SCR_CMD_PARAMS_DEVICE;
                break;

            case SCR_CMD_PARAMS_DEVICE: 
                l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);         
                if(0 == l_intRetVal || l_line != l_lineRef || TRUE == l_lastParam)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_intRetVal = SCR_AssignValue(l_value, &cmdParams->Device, sizeof(cmdParams->Device));
                if(l_intRetVal != FU_RESULT_OK)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }

                l_state = SCR_CMD_PARAMS_FUNCTION;
                break;

            case SCR_CMD_PARAMS_FUNCTION:
                l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);         
                if(0 == l_intRetVal || l_line != l_lineRef || TRUE == l_lastParam)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_intRetVal = SCR_AssignValue(l_value, &cmdParams->Function, sizeof(cmdParams->Function));
                if(l_intRetVal != FU_RESULT_OK)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }                
                l_state = SCR_CMD_PARAMS_REGISTER;
                break;

            case SCR_CMD_PARAMS_REGISTER:
                l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);         
                if(0 == l_intRetVal || l_line != l_lineRef || TRUE == l_lastParam)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_intRetVal = SCR_AssignValue(l_value, &cmdParams->Register, sizeof(cmdParams->Register));
                if(l_intRetVal != FU_RESULT_OK)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_state = SCR_CMD_PARAMS_DATA;
                break;

            case SCR_CMD_PARAMS_ADDRESS:
                l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);         
                if(0 == l_intRetVal || l_line != l_lineRef || TRUE == l_lastParam)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_intRetVal = SCR_AssignValue(l_value, &cmdParams->Address, sizeof(cmdParams->Address));
                if(l_intRetVal != FU_RESULT_OK)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                
                l_state = SCR_CMD_PARAMS_DATA;
                break;

            case SCR_CMD_PARAMS_DATA:
                l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);         
                if(0 == l_intRetVal || l_line != l_lineRef)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_intRetVal = SCR_AssignValue(l_value, &cmdParams->Data, sizeof(cmdParams->Data));
                if(l_intRetVal != FU_RESULT_OK)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                if(TRUE == l_lastParam)
                {
                    l_state = SCR_CMD_PARAMS_DONE;
                }
                else
                {
                    l_state = SCR_CMD_PARAMS_MASK;
                }
                break;

            case SCR_CMD_PARAMS_MASK:
                l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);   
                if(0 == l_intRetVal || l_line != l_lineRef || FALSE == l_lastParam)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_intRetVal = SCR_AssignValue(l_value, &cmdParams->Mask, sizeof(cmdParams->Mask));
                if(l_intRetVal != FU_RESULT_OK)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }                
                l_state = SCR_CMD_PARAMS_DONE;
                break;            

            case SCR_CMD_PARAMS_INT_NUM:
                l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);         
                if(0 == l_intRetVal)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_lineRef = l_line;
                l_intRetVal = SCR_AssignValue(l_value, &intCmdParams->Interrupt, sizeof(intCmdParams->Interrupt));
                if (l_intRetVal != FU_RESULT_OK || intCmdParams->Interrupt != DEFAULT_INT_CMD)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                }
                else
                {
                    if (TRUE == l_lastParam)
                    {
                        for (i = 0; i < NUM_OF_SCR_INT_REGISTERS; ++i)
                        {
                            intCmdParams->RegisterAndValues[i].Register = ILLEGAL_REGISTER;
                        }
                        l_state = SCR_CMD_PARAMS_DONE;
                    }
                    else
                    {
                    l_state = SCR_CMD_PARAMS_REGS_AND_VALUES;
                    }
                }
                break;

            case SCR_CMD_PARAMS_REGS_AND_VALUES:
              for (i = 0; i < NUM_OF_SCR_INT_REGISTERS; ++i)
              {
              /*-------------------------------------------------------------------------------------------*/
              /*                                       Read register                                       */
              /*-------------------------------------------------------------------------------------------*/
                l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);   
                if(0 == l_intRetVal || l_line != l_lineRef || TRUE == l_lastParam || strlen(l_value) != 2)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                switch (l_value[0])
                {
                    case 'A':
                       l_intReg = 0;
                       break;
                    case 'B':
                       l_intReg = 3;
                      break;
                    case 'C':
                       l_intReg = 6;
                      break;
                    default:
                       l_state = SCR_CMD_PARAMS_ERROR;
                       break;
                }
                switch (l_value[1])
                {
                    case 'X':
                       l_intReg += 0;
                       break;
                    case 'H':
                       l_intReg += 1;
                      break;
                    case 'L':
                       l_intReg += 2;
                      break;
                    default:
                       l_state = SCR_CMD_PARAMS_ERROR;
                       break;
                }
                if (l_state != SCR_CMD_PARAMS_ERROR)
                {
                    intCmdParams->RegisterAndValues[i].Register =  l_intReg;

                    /*-----------------------------------------------------------------------------------------*/
                    /*                                       Read value                                        */
                    /*-----------------------------------------------------------------------------------------*/
                    l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);   
                    if(0 == l_intRetVal || l_line != l_lineRef || 
                        (FALSE == l_lastParam && (NUM_OF_SCR_INT_REGISTERS-1) == i) )
                    {
                        l_state = SCR_CMD_PARAMS_ERROR;
                        break;
                    }
                    l_intRetVal = SCR_AssignValue(l_value, &intCmdParams->RegisterAndValues[i].Value, sizeof(intCmdParams->RegisterAndValues[i].Value));
                    if(l_intRetVal != FU_RESULT_OK)
                    {
                        l_state = SCR_CMD_PARAMS_ERROR;
                        break;
                    }

                     if (TRUE == l_lastParam)
                    {
                        for ( i = i+1; i < NUM_OF_SCR_INT_REGISTERS; ++i)
                        {
                          intCmdParams->RegisterAndValues[i].Register = ILLEGAL_REGISTER;
                        }
                        break;
                    }
                }
              }
              if (l_state != SCR_CMD_PARAMS_ERROR)
              {
                  l_state = SCR_CMD_PARAMS_DONE;
              }
              break;

            case SCR_CMD_PARAMS_DELAY:
                l_intRetVal = SCR_GetKeyword(l_value, &l_line, scrFilePtr, scrFileSize, &l_lastParam);         
                if(0 == l_intRetVal || FALSE == l_lastParam)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }
                l_intRetVal = SCR_AssignValue(l_value, &delayCmdParams->Delay, sizeof(delayCmdParams->Delay));
                if(l_intRetVal != FU_RESULT_OK)
                {
                    l_state = SCR_CMD_PARAMS_ERROR;
                    break;
                }

                l_state = SCR_CMD_PARAMS_DONE;
                break;
              
            case SCR_CMD_PARAMS_DONE:
                l_notDone = FALSE;
                break;
                
            case SCR_CMD_PARAMS_ERROR:
                l_retVal = FU_ERR_SCR_SYNTAX_ERROR;
                l_notDone = FALSE;
                break;

            default:
                break;
                
        }
    }
    
    return l_retVal;
        
}


/*--------------------------------------------------------------------------------------*/
/* Function: SCR_ParseTSF                                                               */
/*                                                                                      */ 
/* Parameters:   tsfName  - The TSF file to parse                                       */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: Set the SCR_CmdParams[][]                                              */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Analyze the given flash configuration file                             */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT SCR_ParseTSF(char* tsfName)
{
    FU_RESULT         l_retVal                     = FU_RESULT_OK;    
    int               l_notDone                    = 1;
    int               l_keyLine                    = 0;
    int               l_line                       = 0;   
    int               cmdParamsArrIndex            = 0;
    char*             sections[]                   = {"STAGE BEFORE FU", "STAGE AFTER FU"};
    int               i = 0, j = 0;
    char              l_keyword[20];
    char*             l_currentScrFilePtr;
    size_t            l_currentScrFileSize;
    BOOLEAN           l_lastParam;
    char*             p_scrFilePtr;

    PrintText(APP_VERBOSE_PRINT, "SCR: In SCR_ParseTSF() \n");

    p_scrFilePtr = SCR_OpenInputFile(tsfName,&l_currentScrFileSize);
    
    for(i = 0;i<SCR_MAX_NUM_OF_SECTIONS;i++)
    {
        l_notDone = 1;
        l_keyLine = 0;
        l_line = 0;       
        SCR_GetKeyword(NULL, NULL, NULL, 0, &l_lastParam);
        cmdParamsArrIndex = 0;            

        l_currentScrFilePtr = SCR_GetNextSection(p_scrFilePtr,&l_currentScrFileSize,sections[i]);

        if(l_currentScrFilePtr == NULL)
        {
            return FU_ERR_SCR_INVALID_SECTION;
        }
            
        
        while(l_notDone)
        {   
            
            if (SCR_GetKeyword(l_keyword, &l_line, l_currentScrFilePtr, l_currentScrFileSize, &l_lastParam))
            {
                for (j = 0;
                    j < NUM_OF_SCR_KEYWORDS && cmdParamsArrIndex < CMD_PARAMS_ARRAY_SIZE; 
                    j++)
                {
                    if (strcmp(SCR_Keys[j].keyword, l_keyword) == 0)
                    {
                        l_retVal = SCR_Keys[j].func(SCR_Keys[j].cmdType,
                                                   l_currentScrFilePtr,
                                                   l_currentScrFileSize, 
                                                   &SCR_CmdParams[i][cmdParamsArrIndex++]);

                        if(FU_RESULT_OK != l_retVal)
                        {
                            return l_retVal;
                        }
                        else
                        {
                            break;                               /* No need to check following keywords.    */
                        }
                    }
                }
                if (NUM_OF_SCR_KEYWORDS == j)
                {                                               /* Didn't find matching command keyword,    */
                    return FU_ERR_SCR_INVALID_COMMAND;          /* Terminate parsing.                       */
                }
            }
            else
            {
                l_notDone = 0; 
            }           
            
        }
    }

   return l_retVal;

}


