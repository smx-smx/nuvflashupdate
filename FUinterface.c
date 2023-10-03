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

#include "FUInterface.h"



/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                             LOCAL FUNCTION DECLARATION                               */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


static FU_RESULT assignValue(char *p_val,
                             void *p_ptr,
                             int   p_size,
                             FlashConfigurationData *p_flashConfigData);

static FU_RESULT parseProtectedSectors(char *p_val,
                                       void *p_ptr,
                                       int   p_size,
                                       FlashConfigurationData *p_flashConfigData);

static FU_RESULT parseSectors(char *p_val,
                              void *p_ptr,                              /* Not in use */
                              int   p_size,                             /* Not in use */
                              FlashConfigurationData *p_flashConfigData);

static FU_RESULT handleKeyVal(char *p_key, char *p_val, FlashConfigurationData *p_flashConfigData);


static char*     getNextFlashConfig(char *p_configFilePtr, size_t *p_configFileSize) ;

static int       str2num(char *p_str);

static int       log_2(unsigned int p_num);

static int       getKeyword(char *p_keyword, int *p_line, char *p_cfgFilePtr, size_t p_configFileSize);

static void      setKeywordVars(FlashConfigurationData *p_flashConfigData);




static void convertSegmentToSectors(int p_segmentNumber, 
                                       int p_segmentSize,
                                       int *p_startSector,
                                       int *p_endSector,
                                       FlashUpdateOptions *p_flashUpdateOption,
                                       FlashConfigurationData *p_flashConfig);


static void setSectorsProtectionType(int p_startSector,
                                        int p_endSector,
                                        SECTOR_PROTECTION_TYPE p_protectionType,
                                        FlashUpdateOptions *p_flashUpdateOption,
                                        FlashConfigurationData *p_flashConfig);

static int compareBIOSVersions(char *p_firstVer, char *p_secondVer);
static int findString(char *p_srcBuffer, char *p_searchString, int p_srcBufferSize, int p_offset);

static FU_RESULT getBIOSFieldValue(char *p_binaryImageFilePtr,
                                     int  p_searchOffset,
                                     char *p_signature,
                                     char **p_valueStr);

static int parseStringToHex(char * p_str);



/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                  LOCAL VARIABLES                                     */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


static FlashConfigurationData         *m_flashConfigurationsArr[MAX_FLASH_CONFIGURATIONS];
static int                             m_numOfConfigurations = 0;


/* Will hold the correct flash configuration after calling */
/* GetCorrectFlashConfiguration()                          */

static FlashConfigurationData         *m_correctFlashConfiguration = NULL;

#define NUM_OF_KEYWORDS                18


typedef struct {

     char const *keyword;

     FU_RESULT (* func)(char* val,
                        void* ptr,
                        int size,
                        FlashConfigurationData *p_flashConfigData);
     int size;

} keyword_t;


static void * m_keywordVars[NUM_OF_KEYWORDS];


static const keyword_t Keys[NUM_OF_KEYWORDS] = {
    {"FLASH_ID"  ,        assignValue,                      sizeof(int)},
    {"FLASH_SIZE"  ,      assignValue,                      sizeof(int)},
    {"SECTOR_DEF"  ,      parseSectors,                               0},
    {"BLOCK_SIZE"  ,      assignValue,                      sizeof(int)},
    {"PAGE_SIZE"  ,       assignValue,                      sizeof(int)},
    {"PROTECTED_SECTORS", parseProtectedSectors,                     0},
    {"CMD_READ_DEV_ID"  , assignValue,                     sizeof(char)},
    {"CMD_WRITE_STAT_EN", assignValue,                     sizeof(char)},
    {"CMD_WRITE_EN",      assignValue,                     sizeof(char)},
    {"CMD_READ_STAT",     assignValue,                     sizeof(char)},
    {"CMD_WRITE_STAT",    assignValue,                     sizeof(char)},
    {"CMD_PROG",          assignValue,                     sizeof(char)},
    {"CMD_SECTOR_ERASE",  assignValue,                     sizeof(char)},
    {"CMD_BLOCK_ERASE",   assignValue,                     sizeof(char)},
    {"STATUS_BUSY_MASK",  assignValue,                     sizeof(char)},
    {"STATUS_REG_VAL",    assignValue,                     sizeof(char)},
    {"PROG_SIZE",         assignValue,                     sizeof(char)},
    {"READ_DEV_ID_TYPE",  assignValue,                     sizeof(char)},
    } ;






/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                             FUNCTION IMPLEMENTATION                                  */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/* Function: CompareWithFlashContent                                                    */
/*                                                                                      */ 
/* Parameters:   p_filePtr     - A pointer for the input file                           */
/*               p_fileSize    - The size of the file                                   */
/*               p_sameFlag    - 1: same data                                           */
/*                               0: not the same data                                   */
/*               p_comparisonOffset - Compare flash from this offset                    */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Compare the given file to the flash data                               */
/*--------------------------------------------------------------------------------------*/

FU_RESULT CompareWithFlashContent(char *p_filePtr,
                                            size_t p_fileSize,
                                            int *p_sameFlag,
                                            long p_comparisonOffset)
{
    FU_RESULT     l_retVal          = FU_RESULT_OK;
    unsigned long i                 = 0;
    char          *l_flashContent   = NULL;
    size_t        l_sizeToCompare   = 0;

    
    
    PrintText(APP_VERBOSE_PRINT,
              "CompareWithFlashContent. File size: %d , Flash size: %d , Offset: %d\n",
              p_fileSize, m_correctFlashConfiguration->FlashSize, p_comparisonOffset);

    *p_sameFlag = 1;
    if (NULL == p_filePtr)
    {

        return FU_ERR_GIVEN_NULL_POINTER;
    }

    
    if ((m_correctFlashConfiguration->FlashSize * 1024 - p_comparisonOffset) < p_fileSize)
    {
        l_sizeToCompare = m_correctFlashConfiguration->FlashSize * 1024 - p_comparisonOffset;
    }
    else
    {
        l_sizeToCompare = p_fileSize;
    }
    

    /* There is no need to read the entire flash content unlesss the file */
    /* we are about to compare is bigger or equal to the flash size       */

    l_flashContent = (char*)(malloc((sizeof(char) * l_sizeToCompare)));
    l_retVal = ReadBytesFromFlash(p_comparisonOffset, (unsigned char*)l_flashContent, (unsigned long)l_sizeToCompare);
    if (FU_RESULT_OK == l_retVal)
    {    
        for (i = 0; i < l_sizeToCompare; i++)        
        {
            if (l_flashContent[i] != (*(p_filePtr + i)))
            {
                *p_sameFlag = 0;
                free(l_flashContent);
                return l_retVal;
            }
        }    
    }    
    free(l_flashContent);

    
    return l_retVal;    
}

/*--------------------------------------------------------------------------------------*/
/* Function: AddSegmentAsProtectedSectors                                               */
/*                                                                                      */ 
/* Parameters:   p_segmentNumber     - Segment number                                   */
/*               p_segmentSize       - Segment size in KB                               */
/*               p_protectionType    - Sector protection type                           */
/*               p_flashUpdateOption - Flash update options                             */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Mark the given segment as protected                                    */
/*--------------------------------------------------------------------------------------*/

FU_RESULT AddSegmentAsProtectedSectors(int p_segmentNumber,
                                       int p_segmentSize,
                                       SECTOR_PROTECTION_TYPE p_protectionType,
                                       FlashUpdateOptions *p_flashUpdateOption)
{

    FU_RESULT               l_retVal         = FU_RESULT_OK;
    int                     l_startSector    = 0;
    int                     l_endSector      = 0;
    FlashConfigurationData *l_flashConfig = NULL;




    PrintText(APP_VERBOSE_PRINT,
              "Adding protection type %d to segment number %d\n",
              p_protectionType,
              p_segmentNumber);

    l_retVal = GetCorrectFlashConfiguration(p_flashUpdateOption, &l_flashConfig);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    
    convertSegmentToSectors(p_segmentNumber,
                            p_segmentSize,
                            &l_startSector,
       
                            &l_endSector,
                            p_flashUpdateOption,
                            l_flashConfig);
                            
    setSectorsProtectionType(l_startSector,
                             l_endSector,
                             p_protectionType,
                             p_flashUpdateOption,
                             l_flashConfig);

    return l_retVal;
}



/*--------------------------------------------------------------------------------------*/
/* Function: convertSegmentToSectors                                                    */
/*                                                                                      */ 
/* Parameters:   p_segmentNumber     - Segment number                                   */
/*               p_segmentSize       - Segment size in KB                               */
/*               p_startSector       - Will hold the first sector of the segment        */
/*               p_endSector         - Will hold the last sector of the segment         */
/*               p_flashUpdateOption - Flash update options                             */
/*               p_flashConfig       - Flash configuration                              */
/*                                                                                      */
/* Returns:      void                                                                   */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Convert the given segment into sectors according to the given          */
/*               flash configuration                                                    */
/*--------------------------------------------------------------------------------------*/

static void convertSegmentToSectors(int                     p_segmentNumber, 
                                    int                     p_segmentSize,
                                    int                     *p_startSector,
                                    int                     *p_endSector,
                                    FlashUpdateOptions      *p_flashUpdateOption,
                                    FlashConfigurationData  *p_flashConfig)
{

    unsigned long   l_segmentStartAddress = 0;
    unsigned long   l_segmentEndAddress   = 0;

    int             l_currentSector       = 0;
    unsigned long   l_currentAddress      = 0;
    unsigned int    l_currentSectorSize   = 0;

   
     

    PrintText(APP_VERBOSE_PRINT, "Converting segment %d to sectors...\n", p_segmentNumber);
    
    l_segmentStartAddress = p_segmentNumber * p_segmentSize * 1024;
    l_segmentEndAddress   = (p_segmentNumber + 1) * p_segmentSize * 1024;


    /* Finding the first sector of the given segment */
    
    for (l_currentSector = 0; l_currentSector < p_flashConfig->NumOfSectores; l_currentSector++)
    {
        GetSectorInfo(l_currentSector, &l_currentAddress, &l_currentSectorSize);
        if (l_currentAddress + l_currentSectorSize > l_segmentStartAddress)
        {
            *p_startSector = l_currentSector;
            break;
        }        
    }


    /* Finding the last sector of the given segment */
      
    for (l_currentSector = *p_startSector; l_currentSector < p_flashConfig->NumOfSectores; l_currentSector++)
    {
        GetSectorInfo(l_currentSector, &l_currentAddress, &l_currentSectorSize);
        if (l_currentAddress + l_currentSectorSize >= l_segmentEndAddress)
        {
            *p_endSector = l_currentSector;
            break;
        }        
    }
   
}


/*--------------------------------------------------------------------------------------*/
/* Function: setSectorsProtectionType                                                   */
/*                                                                                      */ 
/* Parameters:   p_startSector       - The first sector of the segment                  */
/*               p_endSector         - The last sector of the segment                   */
/*               p_protectionType    - Sector protection type                           */
/*               p_flashUpdateOption - Flash update options                             */
/*               p_flashConfig       - Flash configuration                              */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Mark the given sectors inside the given flash configuration            */
/*               as protected                                                           */
/*--------------------------------------------------------------------------------------*/

static void setSectorsProtectionType(int                     p_startSector,
                                     int                     p_endSector,
                                     SECTOR_PROTECTION_TYPE  p_protectionType,
                                     FlashUpdateOptions      *p_flashUpdateOption,
                                     FlashConfigurationData  *p_flashConfig)
{       
    int l_currentSector = 0;

    


    PrintText(APP_VERBOSE_PRINT,
              "setSectorsProtectionType: Sector %d to Sector %d Protection Type: %d\n",
              p_startSector,
              p_endSector,
              p_protectionType);
     
    if (SECTOR_WRITE_PROTECTED == p_protectionType)
    {        
        for (l_currentSector = p_startSector; l_currentSector <= p_endSector; l_currentSector++)
        {         
            p_flashConfig->FlashWriteProtectedSectors[l_currentSector / 8] = 
                           p_flashConfig->FlashWriteProtectedSectors[l_currentSector / 8] | (1 << (l_currentSector & 0x7));
        }        
    }
    else if (SECTOR_ERASE_WRITE_PROTECTED == p_protectionType)
    {
        for (l_currentSector = p_startSector; l_currentSector <= p_endSector; l_currentSector++)
        {         
            p_flashConfig->FlashProtectedSectors[l_currentSector / 8] = 
                           p_flashConfig->FlashProtectedSectors[l_currentSector / 8] | (1 << (l_currentSector & 0x7));
        } 
    }    
}


/*--------------------------------------------------------------------------------------*/
/* Function: GetCorrectFlashConfiguration                                               */
/*                                                                                      */ 
/* Parameters:   p_flashUpdateOptions - Flash update options                            */
/*               p_flashConfig        - Will hold the flash configuration               */
/* Returns:      Correct flash configuration                                            */
/*                                                                                      */ 
/* Side effects: Set m_correctFlashConfiguration with the correct flash configuration   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Scan each configuration in the flash configuration array until         */
/*               it founds the correct one                                              */
/*--------------------------------------------------------------------------------------*/

FU_RESULT GetCorrectFlashConfiguration(FlashUpdateOptions *p_flashUpdateOptions,
                                           FlashConfigurationData **p_flashConfig)
{
    FU_RESULT                 l_retVal       = FU_RESULT_OK;
    int                       i              = 0;
    unsigned int              flashID        = 0;
    



    PrintText(APP_VERBOSE_PRINT, "GetCorrectFlashConfiguration\n");
        
    if (NULL == m_correctFlashConfiguration)
    {
        *p_flashConfig = NULL;
        
        for (i = 0; i < m_numOfConfigurations; i++)
        {
            PrintText(APP_VERBOSE_PRINT,
                      "Trying configuration of flash ID: %x\n",
                      m_flashConfigurationsArr[i]->FlashID);
            
            l_retVal = GetFlashID(m_flashConfigurationsArr[i], &flashID);
            if(FU_RESULT_OK != l_retVal)
            {
                return l_retVal;
            }
            
            if (flashID ==  m_flashConfigurationsArr[i]->FlashID)
            {
                PrintText(APP_VERBOSE_PRINT,
                          "Found configuration for flash ID: %x\n\n",
                          m_flashConfigurationsArr[i]->FlashID);
                
                *p_flashConfig = m_flashConfigurationsArr[i];
                m_correctFlashConfiguration = m_flashConfigurationsArr[i];
                
                break;
            }
            else 
            {
                PrintText(APP_VERBOSE_PRINT,
                          "Found flash ID: %x\n\n",
                          flashID);
            }
        }

        if (NULL == *p_flashConfig)
        {
            l_retVal = FU_ERR_CANT_FIND_CORRECT_FLASH_CONFIGURATION;
        }
    }
    else
    {
        *p_flashConfig = m_correctFlashConfiguration;
    }
    
    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: ConfigureFlashAccess                                                       */
/*                                                                                      */ 
/* Parameters:   p_flashUpdateOptions - Flash Update options                            */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Detect and set the correct flash configuration.                        */
/*               Also mapping the flash memory.                                         */
/*--------------------------------------------------------------------------------------*/

FU_RESULT ConfigureFlashAccess(FlashUpdateOptions *p_flashUpdateOptions)
{
    FlashConfigurationData  *l_flashConfig = NULL;
    FU_RESULT               l_retVal       = FU_RESULT_OK;



    StartingFlashConfiguration();               /* Update the user display              */

    
    l_retVal = GetCorrectFlashConfiguration(p_flashUpdateOptions, &l_flashConfig);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }

    
    if (NULL == l_flashConfig)        
    {
        PrintText(APP_ERROR_PRINT, "ERROR: Unexpected flash ID\n");        
        return FU_ERR_UNEXPECTED_FLASH_ID;
    }
    else
    {     
        
        l_retVal = SetFlashConfiguration(l_flashConfig);
        if (FU_RESULT_OK != l_retVal)
        {
            return l_retVal;
        }
     
        l_retVal = ConfigFlashSharedMemory();                     /* Mapping of physical memory           */  
        if (FU_RESULT_OK != l_retVal)
        {
            return l_retVal;
        }
    }
    
   
    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: parseProtectedSectors                                                      */
/*                                                                                      */ 
/* Parameters:   p_val             - Pointer to value                                   */
/*               p_ptr             - Pointer to the configuration field                 */
/*               p_size            - 0 for string value                                 */         
/*               p_flashConfigData - Flash configuration structure                      */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Assign the protected sectors value to the given flash configuration    */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT parseProtectedSectors(char* p_val,
                                       void* p_ptr,
                                       int   p_size,
                                       FlashConfigurationData *p_flashConfigData)
{
    
    int  i = 0;
    int  j;
    char l_numStr[4];
    int  l_sector;
    int  l_fromSector;
    int  l_toSector;



    
    while(p_val[i])
    {
        l_fromSector = -1;
        j = 0;
        
        while((p_val[i] != ',') && (p_val[i] != 0))
        {
            l_numStr[j++] = p_val[i++];
            
            if (p_val[i] == '-')
            {
                l_numStr[j] = 0;
                l_fromSector = str2num(l_numStr);
                j = 0;
                i++;
            }
            
            if (j > 3)
            {
                PrintText(APP_ERROR_PRINT,
                          "ERROR: in configuration file: \"%s\" is not a valid value for PROTECTED_SECTORS keyword. Aborting.\n",
                          p_val);
                
                return FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES;
            }
        }
        
        if (p_val[i] != 0)
        {
            i++;
        }
        
        l_numStr[j]=0;

        l_toSector = str2num(l_numStr);
        
        if (l_fromSector < 0)
        {
            l_fromSector = l_toSector;
        }

        for (l_sector = l_fromSector; l_sector <= l_toSector; l_sector++)
        {
            
            p_flashConfigData->FlashProtectedSectors[l_sector / 8] = 
                            p_flashConfigData->FlashProtectedSectors[l_sector / 8] | (1 << (l_sector & 0x7));

        }
        
    }


    return FU_RESULT_OK;

}


/*--------------------------------------------------------------------------------------*/
/* Function: assignValue                                                                */
/*                                                                                      */ 
/* Parameters:   p_val             - Pointer to value                                   */
/*               p_ptr             - Pointer to the configuration field                 */
/*               p_size            - 0 for string value                                 */         
/*               p_flashConfigData - Flash configuration structure                      */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Assign a value to given configuration field pointer                    */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT assignValue(char* p_val,
                             void* p_ptr,
                             int   p_size,
                             FlashConfigurationData *p_flashConfigData)
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
                          "ERROR: in configuration file: %s is not a proper hexadecimal number. Aborting.\n",
                          p_val);
                
                return FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES;
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
                          "ERROR: in configuration file: %s is not a proper decimal number. Aborting.\n",
                          p_val);
                
                return FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES;
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
    }

    return FU_RESULT_OK;
}


/*--------------------------------------------------------------------------------------*/
/* Function: parseSectors                                                               */
/*                                                                                      */ 
/* Parameters:   p_val             - Pointer to value                                   */
/*               p_ptr             - Pointer to the configuration field                 */
/*               p_size            - 0 for string value                                 */         
/*               p_flashConfigData - Flash configuration structure                      */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Assign sector value to the given flash configuration                   */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT parseSectors(char* p_val,
                              void* p_ptr,         /* Not in use */
                              int   p_size,        /* Not in use */
                              FlashConfigurationData *p_flashConfigData)
{
    int  s             = 0;
    int  i             = 0;
    int  j             = 0;
    char l_numStr[4];
    char l_sizeStr[4];




    p_flashConfigData->NumOfSectores = 0;
    
    while(p_val[i])
    {
        j = 0;
        while(p_val[i] != ':') 
        {
            l_numStr[j++] = p_val[i++];
            if (j > 4)
            {
                PrintText(APP_ERROR_PRINT, 
                          "ERROR: in configuration file: \"%s\" is not a valid value for SECTOR_DEF keyword. Aborting.\n",
                          p_val);
                
                return FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES;
            }
        }
        
        l_numStr[j]=0;
        i++;
        j=0;
        
        while(p_val[i] != 'K')
        {
            l_sizeStr[j++] = p_val[i++];
            
            if (j > 3) {
                PrintText(APP_ERROR_PRINT, 
                          "ERROR: in configuration file: \"%s\" is not a valid value for SECTOR_DEF keyword. Aborting.\n",
                          p_val);

                return FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES;
            }
        }
        
        l_sizeStr[j]=0;
        i++;

        if ((p_val[i] != 0) && (p_val[i++] != ','))
        {
            PrintText(APP_ERROR_PRINT, 
                      "ERROR: in configuration file: \"%s\" is not a valid value for SECTOR_DEF keyword. Aborting.\n",
                      p_val);
            
            return FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES;
        }
        
        p_flashConfigData->FlashSectors[s].num = str2num(l_numStr);
        p_flashConfigData->FlashSectors[s].size = str2num(l_sizeStr);
        
        p_flashConfigData->NumOfSectores = p_flashConfigData->NumOfSectores + 
                                            p_flashConfigData->FlashSectors[s].num;
        s++;        
    }
   
    p_flashConfigData->FlashSectors[s].num = 0;
    p_flashConfigData->FlashSectors[s].size = 0;
 
    return FU_RESULT_OK;

}


/*--------------------------------------------------------------------------------------*/
/* Function: getNextFlashConfig                                                         */
/*                                                                                      */ 
/* Parameters:   p_configFilePtr  - Pointer to the string                               */
/*               p_configFileSize -                                                     */
/*                                                                                      */
/* Returns:      Pointer to the next flash configuration inside the main flash          */
/*               configuration file.                                                    */                      
/*               NULL - In case reached the end of the configuration file               */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Find the next flash configuration inside the flash configuration file  */
/*--------------------------------------------------------------------------------------*/

static char * getNextFlashConfig(char *p_configFilePtr, size_t *p_configFileSize)                                        
{
    char *l_nextConfigPtr    = NULL;
    char *l_nextConfigEndPtr = NULL;





    l_nextConfigPtr = strstr(p_configFilePtr, "[START CONFIG]");

    if (NULL != l_nextConfigPtr)
    {
        l_nextConfigPtr = l_nextConfigPtr + strlen("[START CONFIG]");
        l_nextConfigEndPtr = strstr(l_nextConfigPtr, "[END CONFIG]");
    }

    *p_configFileSize = l_nextConfigEndPtr - l_nextConfigPtr - 1;

        
    return l_nextConfigPtr;    
    
}


/*--------------------------------------------------------------------------------------*/
/* Function: AnalyzeConfigurationFile                                                   */
/*                                                                                      */ 
/* Parameters:   p_configFilePtr  - Pointer to the flash configuration file             */
/*               p_configFileSize - The flash configuration file size                   */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: Set the m_flashConfigurationsArr and m_numOfConfigurations             */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Analyze the given flash configuration file                             */
/*--------------------------------------------------------------------------------------*/

FU_RESULT AnalyzeConfigurationFile(char *p_configFilePtr, size_t p_configFileSize)
{
    FU_RESULT       l_retVal                     = FU_RESULT_OK;    
    int             l_notDone                    = 1;
    int             l_keyLine                    = 0;
    int             l_line                       = 0;   
    char            *l_currentFlashConfigFilePtr = NULL;
    size_t          l_currentFlashConfigFileSize = 0;
    int             i                            = 0;
    char            l_keyword[20];
    char            l_value[80];
    


    
    if (p_configFileSize < MIN_FLASH_CONFIG_FILE_SIZE)
    {
        return FU_ERR_FLASH_CONFIG_FILE_TOO_SMALL;
    }

    m_numOfConfigurations = 0;

    l_currentFlashConfigFilePtr = p_configFilePtr;
    

    while (m_numOfConfigurations < MAX_FLASH_CONFIGURATIONS)
    {

        /* These variable must be reseted for every flash configuration */
        
        l_notDone = 1;
        l_keyLine = 0;
        l_line = 0;       
        getKeyword(NULL, NULL, NULL, 0);
        
            
        l_currentFlashConfigFilePtr = getNextFlashConfig(l_currentFlashConfigFilePtr, &l_currentFlashConfigFileSize);
        

       
        
        
        if (NULL == l_currentFlashConfigFilePtr)
        {
            if (0 == m_numOfConfigurations)
            {
                l_currentFlashConfigFilePtr = p_configFilePtr;
                l_currentFlashConfigFileSize = p_configFileSize;
            }
            else
            {
                break; 
            }
        }


        m_flashConfigurationsArr[m_numOfConfigurations] = (FlashConfigurationData*)
                                                                   (malloc(sizeof(FlashConfigurationData)));

                
          
        for (i = 0; i < SECTORS_BITMAP_ARRAY_SIZE; i++)
        {
            m_flashConfigurationsArr[m_numOfConfigurations]->FlashProtectedSectors[i] = 0;
            m_flashConfigurationsArr[m_numOfConfigurations]->FlashWriteProtectedSectors[i] = 0;
        }
        
        while(l_notDone)
        {   
            
            if (getKeyword(l_keyword, &l_line, l_currentFlashConfigFilePtr, l_currentFlashConfigFileSize))
            {
                l_keyLine = l_line;
                if((getKeyword(l_value,
                                &l_line,
                                l_currentFlashConfigFilePtr,
                                l_currentFlashConfigFileSize) == 0)
                                
                    || (l_keyLine != l_line))
                {
                    
                    PrintText(APP_ERROR_PRINT,
                              "ERROR: in configuration file: keyword %s has no value. Aborting.\n",
                              l_keyword);          

                    return FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES;
                }
                                
                l_retVal = handleKeyVal(l_keyword, l_value, m_flashConfigurationsArr[m_numOfConfigurations]);
                if (FU_RESULT_OK != l_retVal)
                {
                    return l_retVal;
                }
            }
            else
            {
                l_notDone = 0; 
            }           
            
        }



        /* Check some variables */
        
        if (!m_flashConfigurationsArr[m_numOfConfigurations]->FlashBlockSize)
        { 
            /* if block size was not defined set it to be equal to flash size */
            
            m_flashConfigurationsArr[m_numOfConfigurations]->FlashBlockSize = 
                                m_flashConfigurationsArr[m_numOfConfigurations]->FlashSize;

        }

        
        if (m_flashConfigurationsArr[m_numOfConfigurations]->FlashSectors[1].size != 0) { 

            /* More then one sectore size */
            
            if (m_flashConfigurationsArr[m_numOfConfigurations]->FlashBlockSize != 
                m_flashConfigurationsArr[m_numOfConfigurations]->FlashSize)
            {
                PrintText(APP_ERROR_PRINT, 
                          "ERROR: When using different sectors sizes , BLOCK_SIZE must be equal to FLASH_SIZE. Aborting.\n");
                
                return FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES;
            }
            
            m_flashConfigurationsArr[m_numOfConfigurations]->NumOfSectoresInBlock = 
                m_flashConfigurationsArr[m_numOfConfigurations]->NumOfSectores;
            
        }
        else 
        {
            m_flashConfigurationsArr[m_numOfConfigurations]->NumOfSectoresInBlock = 
                m_flashConfigurationsArr[m_numOfConfigurations]->FlashBlockSize /
                m_flashConfigurationsArr[m_numOfConfigurations]->FlashSectors[0].size;
        }
        
        m_flashConfigurationsArr[m_numOfConfigurations]->FlashCommands.page_size = 
            log_2(m_flashConfigurationsArr[m_numOfConfigurations]->FlashPageSize);

        m_numOfConfigurations++;
        l_currentFlashConfigFilePtr = l_currentFlashConfigFilePtr + 10;
        
    }
    
    return FU_RESULT_OK;
}



/*--------------------------------------------------------------------------------------*/
/* Function: getKeyword                                                                 */
/*                                                                                      */ 
/* Parameters:   p_keyword        - Pointer to the returned keyword                     */
/*               p_line           - Pointer to integer that will hold the line          */
/*                                  number in which the the above keyword was found     */
/*                                                                                      */
/*               p_cfgFilePtr     - Flash configuration file                            */
/*               p_configFileSize - Size of the flash configuration file                */
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
/*                  '='  - equal sign                                                   */
/*                  '\n' - end of line                                                  */
/*                                                                                      */
/*               Ignore comments - any character between '#' and the                    */
/*               end of line is consider as a comment.                                  */                  
/*--------------------------------------------------------------------------------------*/

static int getKeyword(char *p_keyword, int *p_line, char *p_cfgFilePtr, size_t p_configFileSize)
{
    char            l_currentChar  = 0;
    int             l_size         = 0;
    int             l_state        = 0;
    static int      l_lineNumber   = 0;
    static size_t   l_bytesRead    = 0;




    
    if (NULL == p_cfgFilePtr)
    {
        /* Reset the function static variables */
        
        l_lineNumber = 0;
        l_bytesRead = 0;
        
        return 0;
    }

    while(l_bytesRead < p_configFileSize ){

        l_currentChar = p_cfgFilePtr[l_bytesRead];        
        l_bytesRead++;
        
        
        if (l_currentChar == 10)
        {
            l_lineNumber++;
        }
        
        if ((l_currentChar == '\t') || (l_currentChar == '='))
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
                    
                    return l_size;
                    
                }
                else
                {
                    p_keyword[l_size++] = l_currentChar;
                }
                
                break;

        }
        
    }
    
    return l_size;
}


/*--------------------------------------------------------------------------------------*/
/* Function: handleKeyVal                                                               */
/*                                                                                      */ 
/* Parameters:   p_key             - Keyword                                            */
/*               p_val             - The new keyword value                              */
/*               p_flashConfigData - Flash configuration                                */         
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Update the right configuration field in the given flash configuration  */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT handleKeyVal(char *p_key, char* p_val, FlashConfigurationData *p_flashConfigData)
{
    FU_RESULT l_retVal = FU_RESULT_OK;
    int       i        = 0;



    setKeywordVars(p_flashConfigData);
        
    for (i = 0; i < NUM_OF_KEYWORDS; i++)
    {
        if (strcmp(Keys[i].keyword, p_key) == 0)
        {
            l_retVal = Keys[i].func(p_val, m_keywordVars[i], Keys[i].size, p_flashConfigData);

            return l_retVal;

        }
    }
    
    PrintText(APP_ERROR_PRINT,
              "ERROR: in configuration file: \"%s\" is not a valid keyword. Aborting.\n",
              p_key);
    
    l_retVal = FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES;
    
    return l_retVal;
    
}



/*--------------------------------------------------------------------------------------*/
/* Function: log_2                                                                       */
/*                                                                                      */ 
/* Parameters:   p_num   - The number                                                   */
/*                                                                                      */
/* Returns:      Number, log of p_num                                                   */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Calculate the log of p_num                                             */
/*--------------------------------------------------------------------------------------*/

static int log_2(unsigned int p_num)
{
    int l_counter = 0;


    
    while (p_num != 0x1)
    {
        p_num = p_num >> 1;
        l_counter ++;
    }
    
    return  l_counter;
}


/*--------------------------------------------------------------------------------------*/
/* Function: str2num                                                                    */
/*                                                                                      */ 
/* Parameters:   p_str - Pointer to the string                                          */
/*                                                                                      */
/* Returns:      Number                                                                 */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Convert a string representing a decimal number into an integer         */
/*--------------------------------------------------------------------------------------*/

static int str2num(char *p_str)
{
    char    *l_tmpStr = p_str;
    int     l_number  = 0;



    while(*l_tmpStr)
    {
        if ((*l_tmpStr >= '0') && (*l_tmpStr <= '9'))
        {
                l_number = l_number * 10 + (*l_tmpStr - '0');
        }
        else
        {
            PrintText(APP_ERROR_PRINT, 
                      "ERROR: in configuration file: %s is not a proper decimal number. Aborting.\n",
                      p_str);
            
            return FU_ERR_CONFIG_FILE_CONTAINS_WRONG_VALUES;
        }
        l_tmpStr++;
    }
    
    return l_number;
}


/*--------------------------------------------------------------------------------------*/
/* Function: setKeywordVars                                                             */
/*                                                                                      */ 
/* Parameters:   p_flashConfigData  - Flash configuration                               */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */ 
/* Side effects: Set m_keywordVars                                                      */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Set the current configuration keywords to work with the given          */
/*               configuration structure                                                */
/*--------------------------------------------------------------------------------------*/

static void setKeywordVars(FlashConfigurationData *p_flashConfigData)
{
    m_keywordVars[0] = &(p_flashConfigData->FlashID);
    m_keywordVars[1] = &(p_flashConfigData->FlashSize);
    m_keywordVars[2] = &(p_flashConfigData->FlashSectors);
    m_keywordVars[3] = &(p_flashConfigData->FlashBlockSize);
    m_keywordVars[4] = &(p_flashConfigData->FlashPageSize);
    m_keywordVars[5] = &(p_flashConfigData->FlashProtectedSectors);
    m_keywordVars[6] = &(p_flashConfigData->FlashCommands.read_device_id);
    m_keywordVars[7] = &(p_flashConfigData->FlashCommands.write_status_enable);
    m_keywordVars[8] = &(p_flashConfigData->FlashCommands.write_enable);
    m_keywordVars[9] = &(p_flashConfigData->FlashCommands.read_status_reg);
    m_keywordVars[10] = &(p_flashConfigData->FlashCommands.write_status_reg);
    m_keywordVars[11] = &(p_flashConfigData->FlashCommands.page_program);
    m_keywordVars[12] = &(p_flashConfigData->FlashCommands.sector_erase);
    m_keywordVars[13] = &(p_flashConfigData->FlashBlockErase);
    m_keywordVars[14] = &(p_flashConfigData->FlashCommands.status_busy_mask);
    m_keywordVars[15] = &(p_flashConfigData->FlashCommands.status_reg_val);
    m_keywordVars[16] = &(p_flashConfigData->FlashCommands.program_unit_size);
    m_keywordVars[17] = &(p_flashConfigData->FlashCommands.read_dev_id_type);
}


/*--------------------------------------------------------------------------------------*/
/* Function: GetDefaultFlashUpdateOptions                                               */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      Flash update default options                                           */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Get the default flash update options                                   */
/*--------------------------------------------------------------------------------------*/

FlashUpdateOptions * GetDefaultFlashUpdateOptions()
{
    
    FlashUpdateOptions *l_flashUpdateOptions = NULL;



    l_flashUpdateOptions = (FlashUpdateOptions*)(malloc(sizeof(FlashUpdateOptions)));
    
    l_flashUpdateOptions->BinaryImageFilePtr    = NULL;
    l_flashUpdateOptions->BinaryImageFileSize   = 0;


    l_flashUpdateOptions->ExitType              = WCB_EXIT_NORMAL;
    l_flashUpdateOptions->SharedMemoryInterface = SHM_MEM_FWH; 
    l_flashUpdateOptions->NumOfBytesToDownload  = 0;
    l_flashUpdateOptions->UseProtectionWindow   = 0;   
    l_flashUpdateOptions->Verbose               = 0;
    l_flashUpdateOptions->SkipConfig            = 1;
    l_flashUpdateOptions->NotSharedBIOS         = 0;
    l_flashUpdateOptions->SharedBIOS            = 0;
    l_flashUpdateOptions->NoCompare             = 0;
    l_flashUpdateOptions->NoVerify              = 0;    
    l_flashUpdateOptions->NoTimeout             = 0;
    l_flashUpdateOptions->BaseAddress           = 0;
    l_flashUpdateOptions->UseUserBaseAddress    = 0;
    l_flashUpdateOptions->VerifyByRead          = 0;
    l_flashUpdateOptions->PreFlashUpdate        = 0;
    l_flashUpdateOptions->RemoveFlashProtection = 0;
    l_flashUpdateOptions->FlashOffset           = 0;
    l_flashUpdateOptions->OverwriteCalibrationValues = 0;
    l_flashUpdateOptions->GenerateBSF                = 0;
    l_flashUpdateOptions->RunBSF                     = 0;
    
    return l_flashUpdateOptions;
}






/*--------------------------------------------------------------------------------------*/
/* Function: compareBIOSVersions                                                        */
/*                                                                                      */ 
/* Parameters:   p_firstVer   - The first BIOS version string (null terminated string)  */
/*               p_secondVer  - The second BIOS version string (null terminated string) */
/*                                                                                      */
/* Returns:     0               - If the two versions are equal                         */
/*              Positive number - In case the first version is bigger                   */
/*              Negetive number - In case the second version is bigger                  */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Checks which of the given BIOS version is newer                        */
/*--------------------------------------------------------------------------------------*/

static int compareBIOSVersions(char *p_firstVer, char *p_secondVer)
{
    return strcmp(p_firstVer, p_secondVer);
}


/*--------------------------------------------------------------------------------------*/
/* Function: findString                                                                 */
/*                                                                                      */ 
/* Parameters:   p_srcBuffer     - The search Buffer                                    */
/*               p_searchString  - The string we want to find                           */
/*               p_srcBufferSize - Source buffer size                                   */
/*               p_offset        - Start the search from this offset                    */
/*                                                                                      */
/* Returns:      -1 in case string not exist in the source buffer                       */
/*               else the string offset inside the source buffer                        */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Search for the given string inside the given buffer from the given     */
/*               offset                                                                 */
/*--------------------------------------------------------------------------------------*/

static int findString(char *p_srcBuffer, char *p_searchString, int p_srcBufferSize, int p_offset)
{    
    int             l_offset    = -1;   
    int             l_foundFlag = 0;
    int             i           = 0;
    int             j           = 0;
    


    l_foundFlag = 0;
    for (i = p_offset; (i < p_srcBufferSize) && !l_foundFlag; i++)
    {
        if (p_srcBuffer[i] == p_searchString[0])
        {
            l_foundFlag = 1;
            l_offset    = i;
            for (j = 1; (j < (int)(strlen(p_searchString))) && (l_foundFlag) && (i + j < p_srcBufferSize); j++)
            {                 
                if (p_srcBuffer[i + j] != p_searchString[j])
                {                  
                    l_foundFlag = 0;
                    l_offset    = -1;
                }
            }
        }
    }

    return l_offset;
}


/*--------------------------------------------------------------------------------------*/
/* Function: IsFlashBIOSNewer                                                           */
/*                                                                                      */ 
/* Parameters:   p_binaryImageFilePtr - Pointer to the binary image file                */
/*               p_binVer             - If specified then it will hold the binary file  */
/*                                      BIOS version                                    */
/*               p_flashVer           - If specified then it will hold the flash BIOS   */
/*                                      version                                         */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Check if the BIOS on the flash is newer as the one in the binary       */
/*               file.                                                                  */
/*--------------------------------------------------------------------------------------*/

FU_RESULT IsFlashBIOSNewer(char *p_binaryImageFilePtr, char **p_binVer, char **p_flashVer)
{
    FU_RESULT l_retVal           = FU_RESULT_OK; 
    char      *l_flashVer        = NULL;
    char      *l_binVer          = NULL;
    char      *l_searchOffsetStr = NULL;
    int        l_searchOffset    = 0;




    /* Finding the binary file BIOS model and version fields offset */

    l_retVal = getBIOSFieldValue(p_binaryImageFilePtr, 0x50 ,BIOS_MODEL_OFFSET_SIGNATURE, &l_searchOffsetStr);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }

    /* In case there is no offset field we need to search the model and offset between 0x50 and 0x100 */
    
    if (l_searchOffsetStr)
    {
        l_searchOffset = parseStringToHex(l_searchOffsetStr);
    }
    else
    {
        l_searchOffset = 0x50;
    }
    PrintText(APP_VERBOSE_PRINT,
              "Binary file BIOS version offset (string, number): %s , 0x%x\n",
              l_searchOffsetStr,
              l_searchOffset);
        
    l_retVal = getBIOSFieldValue(p_binaryImageFilePtr, l_searchOffset, BIOS_VERSION_SIGNATURE, &l_binVer);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    PrintText(APP_VERBOSE_PRINT,
              "Binary file BIOS version: %s\n",
              l_binVer);
        

    /* Finding flash BIOS model and version fields offset */
            
    l_retVal = getBIOSFieldValue(NULL, 0x50 ,BIOS_MODEL_OFFSET_SIGNATURE, &l_searchOffsetStr);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }

    /* In case there is no offset field we need to search the model and offset between 0x50 and 0x100 */
    
    if (l_searchOffsetStr)
    {
        l_searchOffset = parseStringToHex(l_searchOffsetStr);
    }
    else
    {
        l_searchOffset = 0x50;
    }

    PrintText(APP_VERBOSE_PRINT,
              "Flash BIOS version offset (string, number): %s , 0x%x\n",
              l_searchOffsetStr,
              l_searchOffset);
        
    l_retVal = getBIOSFieldValue(NULL, l_searchOffset,BIOS_VERSION_SIGNATURE, &l_flashVer);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    PrintText(APP_VERBOSE_PRINT,
          "Flash BIOS version: %s\n",
          l_flashVer);
    
    /* Ic case there is no version field in both flash and bin ignore the check (return success) */
    if (l_binVer == NULL && l_flashVer == NULL)
    {
        return FU_RESULT_OK;
    }

    if (l_binVer == NULL)
    {
        l_retVal = FU_ERR_MISSING_BIN_FILE_BIOS_VERSION;
    }
    else if (l_flashVer == NULL)
    {
        l_retVal = FU_ERR_MISSING_FLASH_BIOS_VERSION;
    }
    else if (compareBIOSVersions(l_flashVer, l_binVer) > 0)    
    {
        l_retVal = FU_RESULT_OLDER_BIN_FILE_BIOS;
    }
    else
    {
        l_retVal = FU_RESULT_OK;
    }
    

    if (p_binVer != NULL && l_binVer != NULL)
    {
        *p_binVer = (char *)(malloc(sizeof(char) * strlen(l_binVer) + 1));
        strcpy(*p_binVer, l_binVer);
    }

    if (p_flashVer != NULL && l_flashVer != NULL)
    {
        *p_flashVer = (char *)(malloc(sizeof(char) * strlen(l_flashVer) + 1));
        strcpy(*p_flashVer, l_flashVer);
    }


    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: IsSameBIOSModel                                                            */
/*                                                                                      */ 
/* Parameters:   p_binaryImageFilePtr - Pointer to the binary image file                */
/*                                                                                      */
/*               p_binModel           - Will hold the binary file BIOS model (Optional) */
/*               p_flashModel         - Will hold the flash BIOS model (Optional)       */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Check if the BIOS on the flash and the one in the binary file are      */
/*               from the same model                                                    */
/*--------------------------------------------------------------------------------------*/

FU_RESULT IsSameBIOSModel(char *p_binaryImageFilePtr, char **p_binModel, char **p_flashModel)
{
    FU_RESULT  l_retVal          = FU_RESULT_OK; 
    char      *l_flashModel      = NULL;
    char      *l_binModel        = NULL;
    int        l_searchOffset    = 0;
    char      *l_searchOffsetStr = NULL;




    /* Finding the binary file BIOS model and version fields offset */

    l_retVal = getBIOSFieldValue(p_binaryImageFilePtr, 0x50 ,BIOS_MODEL_OFFSET_SIGNATURE, &l_searchOffsetStr);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }

    /* In case there is no offset field we need to search the model and offset between 0x50 and 100 */
    
    if (l_searchOffsetStr)
    {
        l_searchOffset = parseStringToHex(l_searchOffsetStr);
    }
    else
    {
        l_searchOffset = 0x50;
    }
    PrintText(APP_VERBOSE_PRINT,
              "Binary file BIOS model offset (string, number): %s , 0x%x\n",
              l_searchOffsetStr,
              l_searchOffset);
        
    l_retVal = getBIOSFieldValue(p_binaryImageFilePtr, l_searchOffset, BIOS_MODEL_SIGNATURE, &l_binModel);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    PrintText(APP_VERBOSE_PRINT,
              "Binary file BIOS model: %s\n",
              l_binModel);        


    /* Finding flash BIOS model and version fields offset */
    
    l_retVal = getBIOSFieldValue(NULL, 0x50 ,BIOS_MODEL_OFFSET_SIGNATURE, &l_searchOffsetStr);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }

    /* In case there is no offset field we need to search the model and offset between 0x50 and 100 */
    
    if (l_searchOffsetStr)
    {
        l_searchOffset = parseStringToHex(l_searchOffsetStr);
    }
    else
    {
        l_searchOffset = 0x50;
    }    
    PrintText(APP_VERBOSE_PRINT,
              "Flash BIOS model offset (string, number): %s , 0x%x\n",
              l_searchOffsetStr,
              l_searchOffset);
        
    l_retVal = getBIOSFieldValue(NULL, l_searchOffset, BIOS_MODEL_SIGNATURE, &l_flashModel);
    if (FU_RESULT_OK != l_retVal)
    {
        return l_retVal;
    }
    PrintText(APP_VERBOSE_PRINT,
              "Flash BIOS model: %s\n",
              l_flashModel);     
    

    if (l_binModel == NULL && l_flashModel == NULL)
    {
        l_retVal = FU_RESULT_NO_BIOS_MODEL;
    }
    else if (l_binModel == NULL)
    {        
        l_retVal = FU_ERR_MISSING_BIN_FILE_BIOS_MODEL;
    }
    else if (l_flashModel == NULL)
    {
        l_retVal = FU_ERR_MISSING_FLASH_BIOS_MODEL;
    }
    else if (strcmp((char*)l_binModel, (char*)l_flashModel) != 0)
    {
        l_retVal = FU_ERR_DIFFERENT_BIOS_MODEL;
    }
    else
    {
        l_retVal = FU_RESULT_OK;
    }
    
    if (p_binModel != NULL && l_binModel != NULL)
    {
        *p_binModel = (char *)(malloc(sizeof(char) * strlen(l_binModel) + 1));
        strcpy(*p_binModel, l_binModel);
    }

    if (p_flashModel != NULL && p_flashModel != NULL)
    {
        *p_flashModel = (char *)(malloc(sizeof(char) * strlen(l_flashModel) + 1));
        strcpy(*p_flashModel, l_flashModel);
    }


    return l_retVal;
}



/*--------------------------------------------------------------------------------------*/
/* Function: getBIOSFieldValue                                                          */
/*                                                                                      */ 
/* Parameters:   p_binaryImageFilePtr - A pointer to the binary image file or NULL      */
/*                                      to read from the flash BIOS                     */
/*               p_searchOffset       - Search offset                                   */
/*               p_signature          - The BIOS header field signature we want to read */
/*               p_valueStr           - Will hold the given field value or NULL in case */
/*                                      the field doesn't exist                         */
/*                                                                                      */
/* Returns:      FU_RESULT                                                              */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Read the value of a BIOS field from the given offset location. Search  */
/*               between offset to (offset + 0x50)                                      */
/*--------------------------------------------------------------------------------------*/

static FU_RESULT getBIOSFieldValue(char *p_binaryImageFilePtr,
                                     int  p_searchOffset,
                                     char *p_signature,
                                     char **p_valueStr)
{
    
    FU_RESULT       l_retVal    = FU_RESULT_OK; 
    int             i           = 0;
    int             l_offset    = 0;
    int             l_size      = 0;
    char            l_buf[0x50];
 


   
    if (NULL == p_binaryImageFilePtr)
    {
        /* Getting the flash BIOS version */

        l_retVal = ReadBytesFromFlash(p_searchOffset, (unsigned char *)l_buf, 0x50);
        if (FU_RESULT_OK != l_retVal)
        {
            return l_retVal;
        }
    }
    else
    {
        /* Getting the binary file BIOS version */
        
        for (i = 0; i < 0x50; ++i)
        {
            l_buf[i] = p_binaryImageFilePtr[p_searchOffset + i];
        }
    }
    
    l_offset = findString((char *)l_buf, p_signature, 0x50, 0);
    if (l_offset < 0)
    {
        /* Field not found */
        
        *p_valueStr = NULL;
       
    }
    else
    {
        /* Need to find end of field (marked with $) */

        l_offset = l_offset + (int)strlen(p_signature);
        l_size = findString((char *)l_buf, "$", 0x50, l_offset);
        if (l_size < 0)
        {
            /* End of field not found */

            *p_valueStr = NULL;          
        }
        else
        {             
            l_size = l_size - l_offset;            
            *p_valueStr = (char *)(malloc(sizeof(char) * (l_size + 1)));
            for (i = 0; i < l_size ; i++)
            {
                (*p_valueStr)[i] = l_buf[l_offset + i];
            }            
            (*p_valueStr)[l_size] = 0;
        }
        
    }

    
    return l_retVal;
}


/*--------------------------------------------------------------------------------------*/
/* Function: parseStringToHex                                                           */
/*                                                                                      */ 
/* Parameters:   p_str - A pointer to the string                                        */
/*                                                                                      */
/* Returns:      The value of the string                                                */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Convert the given (hex base)string to a number                         */
/*--------------------------------------------------------------------------------------*/

static int parseStringToHex(char * p_str)
{
    int l_result = 0;
    int i = 0;



    for (i = 0; p_str != NULL && p_str[i] != 0 ; ++i)
    {        
        if (p_str[i] <= '9' && p_str[i] >= '0')
        {
            l_result = (l_result * 16) + p_str[i] - '0';
        }
        else if (p_str[i] <= 'f' && p_str[i] >= 'a')
        {
            l_result = (l_result * 16) + p_str[i] - 'a' + 10;
        }
        else if (p_str[i] <= 'F' && p_str[i] >= 'A')
        {
            l_result = (l_result * 16) + p_str[i] - 'A' + 10;
        }
        else
        {
            break;
        }
    }
    
    return l_result;
}


