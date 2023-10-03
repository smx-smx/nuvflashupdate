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



#ifndef EXEARCHIVELIBRARYINTERFACE_HEADER 
#define EXEARCHIVELIBRARYINTERFACE_HEADER


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                      INCLUDES                                        */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                           APPENDIEX HEADER DEFINITIONS                               */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/ 


#define APPENDIX_HEADER_CODE                     "AHXX" /* Header Code                  */
#define APPENDIX_HEADER_SIZE_OFFSET              -12    /* Size field offset from EOF   */ 
#define APPENDIX_HEADER_NUM_OF_ENTRIES_OFFSET    -8     /* NumOfEntries offset from EOF */




/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                               ERROR CODE DEFINITIONS                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/ 

/* Program */

#define ERROR_ILLEGAL_TARGET_EXE_NAME           -11

#define ERROR_MALLOC_FAILED						-90

/* File */

#define ERROR_READING_FROM_FILE                 -101
#define ERROR_WRITING_TO_FILE                   -102
#define ERROR_OPENING_FILE                      -103
#define ERROR_FILE_MISSING                      -104
#define ERROR_TARGET_DRIVE_IS_FULL              -105



/* Compression */

#define ERROR_COMPRESSING_FILE                  -200



/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                           DATA STRUCTURE DEFINITIONS                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/ 

/*--------------------------------------------------------------------------------------*/ 
/* AppendixEntry                                                                        */
/*                                                                                      */
/*  Name          - Name of the attached file                                           */
/*                  (max length is 12, 13 byte reserved for                             */
/*                  terminating zero)                                                   */ 
/*                                                                                      */
/* IsCompressed   - 0x01 if data inside the executable is compressed                    */
/*                  0x00 otherwise                                                      */
/*                                                                                      */
/* Offset         - offset in bytes from the start of the executable,                   */
/*                  were data is located                                                */
/*                                                                                      */
/* Size           - the size of the data (before compression)                           */
/*                                                                                      */
/* CompressedSize - the size of the data inside the executable                          */
/*--------------------------------------------------------------------------------------*/ 

typedef struct AppendixEntry
{
    unsigned char  Name[13];
	unsigned char  IsCompressed;    
	unsigned int   Offset;
	unsigned long  Size;
	unsigned long  CompressedSize;
	
} AppendixEntry;

typedef enum
{
    
    EXIT_CODE_ANY_VALUE, /* exit code scheme: success - 0, failure - other */
    EXIT_CODE_BINARY     /* exit code scheme: success - 0, failure - 1 */
    
} EXIT_CODE_TYPE;


/*--------------------------------------------------------------------------------------*/
/* AppendixHeader                                                                       */ 
/*                                                                                      */
/* Located at the end of the executable. Holds significant information                  */
/* on the executable appendixes.                                                        */
/*                                                                                      */
/* Size                  - size in bytes of the header                                  */
/*                                                                                      */
/* NumOfSubHeaders       - number of sub headers                                        */
/*                                                                                      */
/* AppedixEntriesArray   - will contain appendixes entries                              */
/*--------------------------------------------------------------------------------------*/

typedef struct AppendixHeader
{
	unsigned short Size;
	unsigned short NumOfEntries;

	AppendixEntry **AppedixEntriesArray;	
	
} AppendixHeader;

/*---------------------------------------------------------------------------------------------------------*/
/* AppendixesCmmandLineInfo                                                                                */
/*                                                                                                         */
/* Information that was parsed from the command line                                                       */
/* This struct only refers to the flags: coh, cmsg, o                                                      */
/*                                                                                                         */
/* updateHelpFile       - Indicate if the readme should be updated                                         */
/*                                                                                                         */
/* updateMessages       - Indicate if the messages file should be updated                                  */
/*                                                                                                         */
/* updateExecutable     - Indicate if an output executable filename was given                              */
/*                                                                                                         */
/* l_messagesFile       - The name of the message file                                                     */
/*                                                                                                         */
/* l_helpFile           - The name of the readme file                                                      */
/*                                                                                                         */
/* l_targetExeFileName  - The name of the target executable file                                           */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct AppendixesCmmandLineInfo
{
    int       updateHelpFile;                                
    int       updateMessages;                                 
    int       updateExecutable;                               
    char      messagesFile[FILENAME_MAX];
    char      helpFile[FILENAME_MAX];
    char      targetExeFileName[FILENAME_MAX];

} AppendixesCmmandLineInfo;


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/*                                FUNCTION DECLERATIONS                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------*/
/* Function: AddAppendixFromRAMToExe                                                    */
/*                                                                                      */ 
/* Parameters:   p_exeFileName          - Executable file name                          */
/*                                                                                      */
/*               p_removePrevAppendixes - 1 ->Removed previous appendixes               */
/*                                        0 ->Keep previous appendixes                  */
/*																						*/
/*				 p_fileToAddPtr         - Pointer to the file we want to add (in RAM)	*/
/*																						*/                                                 
/*               p_fileNameToAdd        - The name of the file we want to add           */
/*																						*/
/*				 p_fileToAddSize		- Size of the file we want to add				*/
/*																						*/
/*               p_compress             - 0 -> Without compression                      */
/*                                        1 -> Comoress and add                         */ 
/*                                                                                      */ 
/* Returns:      1 on success                                                           */
/*               otherwise 0                                                            */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Add a file from RAM to an executable, with or without compression      */
/*--------------------------------------------------------------------------------------*/

int AddAppendixFromRAMToExe(unsigned char *p_exeFileName,
							int p_removePrevAppendixes,
							char *p_fileToAddPtr,
							char *p_fileNameToAdd,
							size_t p_fileToAddSize,
							int p_compress);



/*--------------------------------------------------------------------------------------*/
/* Function: AddAppendixesToExe                                                         */
/*                                                                                      */ 
/* Parameters:   p_exeFileName          - Executable file name                          */
/*                                                                                      */
/*               p_removePrevAppendixes - 1 ->Removed previous appendixes               */
/*                                        0 ->Keep previous appendixes                  */
/*                                                                                      */
/*               p_filesToAdd           - Files to add                                  */
/*               p_numOfFiles           - Number of files to add                        */
/*                                                                                      */ 
/*               p_compress             - 0 -> Without compression                      */
/*                                        1 -> Comoress and add                         */ 
/*                                                                                      */ 
/* Returns:      1 on success                                                           */
/*               otherwise 0                                                            */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Add files to an executable, with or without compression                */
/*--------------------------------------------------------------------------------------*/

int AddAppendixesToExe(unsigned char *p_exeFileName,
                       int p_removePrevAppendixes,
                       char **p_filesToAdd,
                       int p_numOfFiles,
                       int p_compress);


/*--------------------------------------------------------------------------------------*/
/* Function: ExtractAppendixFromExe                                                     */
/*                                                                                      */ 
/* Parameters:   p_exeFileName    - Executable file name                                */
/*               p_appendixEntry  - Appendix entry to be extract                        */
/*               p_targetFile     - Extract appendix to this file name                  */
/*                                  if NULL-> extract to RAM                            */
/*                                                                                      */ 
/* Returns:      When extracting to RAM-> Returns a pointer to extracted                */
/*               file at the RAM.                                                       */
/*               Otherwise return NULL                                                  */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Extract an appendix from the executable, including                     */
/*               uncompress the appendix(when appendix compressed)                      */
/*--------------------------------------------------------------------------------------*/

 char* ExtractAppendixFromExe(char *p_exeFileName,
                              AppendixEntry *p_appendixEntry,
                              char *p_targetFile);                             




/*--------------------------------------------------------------------------------------*/
/* Function: LoadAppendixHeaderFromExe                                                  */
/*                                                                                      */ 
/* Parameters:   p_exeFileName    - Executable file name                                */
/*               p_appendixHeader - Appendix header will be load into this              */
/*                                  pointer                                             */
/*                                                                                      */
/* Returns:      1 on success                                                           */
/*               otherwise 0                                                            */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Open the give executable and load the appendix header                  */
/*--------------------------------------------------------------------------------------*/

int LoadAppendixHeaderFromExe(unsigned char *p_exeFileName,
                              AppendixHeader *p_appendixHeader);




/*--------------------------------------------------------------------------------------*/
/* Function: GetAppendixEntrySize                                                       */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      Appendix Entry size                                                    */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Retrieve appendix header entry size                                    */
/*--------------------------------------------------------------------------------------*/

int GetAppendixEntrySize();




/*--------------------------------------------------------------------------------------*/
/* Function: GetAppendixHeaderInfoSize                                                  */
/*                                                                                      */ 
/* Parameters:   None                                                                   */
/*                                                                                      */
/* Returns:      Appendix Header information size                                       */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Retrieve appendix header information size                              */
/*--------------------------------------------------------------------------------------*/

int GetAppendixHeaderInfoSize();





/*--------------------------------------------------------------------------------------*/
/* Function: IsExeContainAppendixHeader                                                 */
/*                                                                                      */ 
/* Parameters:   p_exeFileName    - Executable file name                                */
/*                                                                                      */
/* Returns:      1 if header exist                                                      */
/*               otherwise 0                                                            */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Check if the given executable contain an appendix header               */
/*--------------------------------------------------------------------------------------*/

int IsExeContainAppendixHeader(unsigned char *p_exeFileName);





/*--------------------------------------------------------------------------------------*/
/* Function: WriteAppendixHeaderToExe                                                   */
/*                                                                                      */ 
/* Parameters:   p_exeFileName    - Executable file name                                */
/*               p_appendixHeader - Appendix header to write                            */
/*                                                                                      */
/* Returns:      1 on success                                                           */
/*               otherwise 0                                                            */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Writing/Updating appendix header in the given executable               */
/*--------------------------------------------------------------------------------------*/

int WriteAppendixHeaderToExe(unsigned char *p_exeFileName,
                             AppendixHeader *p_appendixHeader);




/*--------------------------------------------------------------------------------------*/
/* Function: RemoveExeAppendixes                                                        */
/*                                                                                      */ 
/* Parameters:   p_exeFileName    - Executable file name                                */
/*                                                                                      */ 
/* Returns:      1 on success                                                           */
/*               otherwise 0                                                            */
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Check to see if the given executable contain appendixes,               */
/*               remove them (including the executable appendix header)                 */
/*--------------------------------------------------------------------------------------*/

int RemoveExeAppendixes(unsigned char *p_exeFileName);



/*--------------------------------------------------------------------------------------*/
/* Function: ErrorHandle                                                                */
/*                                                                                      */ 
/* Parameters:   p_errorNumber - Error code as defined above                            */
/*                                                                                      */
/*               p_customPrint - Select error print type                                */
/*							     0 -> Text depands on the error code                    */
/*							     1 -> Print the error code with error text              */
/*                                                                                      */
/*			     p_errorText   - Error text to be printed                               */
/*                                                                                      */
/* Returns:      None                                                                   */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Handle error event. In case of a negetive error number will exit       */
/*               the program                                                            */ 
/*--------------------------------------------------------------------------------------*/

void ErrorHandle(int p_errorNumber, int p_customPrint, char *p_errorText, ...);

/*-----------------------------------------------------------------------------------------------*/
/* Function: SetExitCodeType                                                                 */
/*                                                                                               */ 
/* Parameters:   p_exitCodeType                                                                  */
/*                                                                                               */
/* Returns:      None                                                                            */ 
/*                                                                                               */ 
/* Side effects: Set m_exitCodeType to be p_exitCodeType                                         */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Sets the exit code type of the application - any value or binary                */
/*-----------------------------------------------------------------------------------------------*/

void SetExitCodeType(EXIT_CODE_TYPE p_exitCodeType);

/*-----------------------------------------------------------------------------------------------*/
/* Function: GetExitCodeType                                                                 */
/*                                                                                               */ 
/* Parameters:   None                                                                            */
/*                                                                                               */
/* Returns:      None                                                                            */ 
/*                                                                                               */ 
/* Side effects: None                                                                            */
/*                                                                                               */ 
/* Description:                                                                                  */ 
/*               Returns m_exitCodeType                                                          */
/*-----------------------------------------------------------------------------------------------*/

EXIT_CODE_TYPE GetExitCodeType();

/*--------------------------------------------------------------------------------------*/
/* Function: DecompressFileToRAM                                                        */
/*                                                                                      */ 
/* Parameters:   p_srcFileName      - File to be decompressed                           */
/*               p_startOffset      - Start to decompress from this offset              */ 
/*               p_size             - Size to be decompressed                           */
/*               p_unCompressedSize - File size before compression                      */
/*                                                                                      */
/* Returns:      Pointer to the uncompressed file at the RAM                            */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */
/*               Decompress the given file to RAM                                       */ 
/*--------------------------------------------------------------------------------------*/

char *DecompressFileToRAM(char *p_srcFileName,
						  unsigned long p_startOffset,
                          unsigned long p_size,
					      unsigned long p_unCompressedSize);
                          



/*--------------------------------------------------------------------------------------*/
/* Function: DecompressFileToFile                                                       */
/*                                                                                      */ 
/* Parameters:   p_srcFileName      - File to be decompressed                           */
/*               p_startOffset      - Start to decompress from this offset              */
/*               p_size             - Size to be decompressed                           */
/*               p_unCompressedSize - File size before compression                      */
/*			     p_targetFileName   - Uncompressed output file name                     */
/*                                                                                      */
/* Returns:      None                                                                   */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Decompress the given file into the target file                         */ 
/*--------------------------------------------------------------------------------------*/

void DecompressFileToFile(char *p_srcFileName,
						  unsigned long p_startOffset,
                          unsigned long p_size,
					      unsigned long p_unCompressedSize,
                          char *p_targetFileName);



/*--------------------------------------------------------------------------------------*/
/* Function: CompressFile                                                               */
/*                                                                                      */ 
/* Parameters:   p_srcFileName    - File to be compressed                               */
/*               p_targetFileName - Compressed output file name                         */
/*                                                                                      */
/* Returns:      None                                                                   */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Compress the given file into the target file                           */ 
/*--------------------------------------------------------------------------------------*/

void CompressFile(char *p_srcFileName, char *p_targetFileName);




/*--------------------------------------------------------------------------------------*/
/* Function: CompressFileFromRAM                                                        */
/*                                                                                      */ 
/* Parameters:   p_srcFilePtr     - File to compresse									*/
/*				 p_srcFileSize    - Source file size	                                */
/*               p_targetFileName - Compressed output file name                         */
/*                                                                                      */
/* Returns:      None                                                                   */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Compress the given file into the target file                           */ 
/*--------------------------------------------------------------------------------------*/

void CompressFileFromRAM(char *p_srcFilePtr, unsigned long p_srcFileSize, char *p_targetFileName);



/*--------------------------------------------------------------------------------------*/
/* Function: CompressFileFromRAMToRam                                                   */
/*                                                                                      */ 
/* Parameters:   p_srcFilePtr     - File to compresse									*/
/*				 p_srcFileSize    - Source file size	                                */
/*               p_compressedSize - Will contain the file size after compression        */
/*                                                                                      */
/* Returns:      Pointer to the compressed file at the RAM                              */ 
/*                                                                                      */ 
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Compress the given file (in RAM) into the RAM                          */ 
/*--------------------------------------------------------------------------------------*/

char* CompressFileFromRAMToRAM(char *p_srcFilePtr, unsigned long p_srcFileSize, unsigned long *p_compressedSize);



/*--------------------------------------------------------------------------------------*/
/* Function: GetFileSizeX                                                               */
/*                                                                                      */ 
/* Parameters:   p_fileName    - Target file name                                       */
/*                                                                                      */
/* Returns:      Size of the target file                                                */
/*                                                                                      */                                                                                      
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Calculating the given file size                                        */
/*--------------------------------------------------------------------------------------*/

unsigned long GetFileSizeX(char *p_fileName);



/*--------------------------------------------------------------------------------------*/
/* Function: WriteIntToFile                                                             */
/*                                                                                      */ 
/* Parameters:   p_fileHandle    - Target file handle                                   */
/*               p_numberToWrite - Number we want to write                              */
/*                                                                                      */
/* Returns:      Written number                                                         */
/*                                                                                      */                                                                                      
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Writing 4 bytes integer to the current position                        */
/*               in the given file.                                                     */
/*               Writing is always at the end of the file                               */
/*--------------------------------------------------------------------------------------*/

int WriteIntToFile(FILE *p_fileHandle, int p_numberToWrite);




/*--------------------------------------------------------------------------------------*/
/* Function: WriteShortToFile                                                           */
/*                                                                                      */ 
/* Parameters:   p_fileHandle    - Target file handle                                   */
/*               p_numberToWrite - Number we want to write                              */
/*                                                                                      */
/* Returns:      Written number                                                         */
/*                                                                                      */                                                                                      
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Writing 2 bytes short to the current position                          */
/*               in the given file.                                                     */ 
/*               Writing is always at the end of the file.                              */
/*--------------------------------------------------------------------------------------*/

int WriteShortToFile(FILE *p_fileHandle, short p_numberToWrite);




/*--------------------------------------------------------------------------------------*/
/* Function: ReadIntFromFile                                                            */
/*                                                                                      */ 
/* Parameters:   p_fileHandle    - Target file handle                                   */
/*                                                                                      */
/* Returns:      Integer read                                                           */
/*                                                                                      */                                                                                      
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Reading 4 bytes integer from the current position                      */
/*			     inside the given file                                                  */
/*--------------------------------------------------------------------------------------*/

int ReadIntFromFile(FILE *p_fileHandle);




/*--------------------------------------------------------------------------------------*/
/* Function: ReadShortFromFile                                                          */
/*                                                                                      */ 
/* Parameters:   p_fileHandle    - Target file handle                                   */
/*                                                                                      */
/* Returns:      Byte read                                                              */
/*                                                                                      */                                                                                      
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Reading 2 bytes short from the current position                        */
/*			     inside the given file                                                  */
/*--------------------------------------------------------------------------------------*/

short ReadShortFromFile(FILE *p_fileHandle);




/*--------------------------------------------------------------------------------------*/
/* Function: CutFileToNewSize                                                           */
/*                                                                                      */ 
/* Parameters:   p_exeFileName - Target file handle                                     */ 
/*               p_cutOffset   - Start of cut                                           */
/*               p_newSize     - Size to cut                                            */
/*                                                                                      */
/* Returns:      File size after cut                                                    */
/*                                                                                      */                                                                                      
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Cut the given file to the new size from the given offset               */                                     
/*--------------------------------------------------------------------------------------*/

int CutFileToNewSize(char *p_exeFileName,
                     size_t       p_cutOffset,
                     size_t       p_newSize);



/*--------------------------------------------------------------------------------------*/
/* Function: GetFileNameFromPath                                                        */
/*                                                                                      */ 
/* Parameters:   p_target         - The target buffer                                   */
/*               p_path           - The path                                            */
/*               p_targetBufSize  - Target buffer size minus 1                          */
/*                                                                                      */
/* Returns:      None                                                                   */
/*                                                                                      */                                                                                      
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Extracting the file name from the given path                           */                                     
/*--------------------------------------------------------------------------------------*/

void GetFileNameFromPath(unsigned char *p_target, char *p_path, int p_targetBufSize);




/*--------------------------------------------------------------------------------------*/
/* Function: AppendFileToExe                                                            */
/*                                                                                      */ 
/* Parameters:   p_exeFileName      - Executable file name                              */
/*               p_appendixFileName - File to append                                    */
/*                                                                                      */
/* Returns:      Appended file offset                                                   */
/*                                                                                      */                                                                                      
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Appending a file to the end of the executable                          */                                     
/*--------------------------------------------------------------------------------------*/

long AppendFileToExe(char *p_exeFileName, char *p_appendixFileName);



/*--------------------------------------------------------------------------------------*/
/* Function: AppendFileFromRAMToExe                                                     */
/*                                                                                      */ 
/* Parameters:   p_exeFileName      - Executable file name                              */
/*               p_appendixFilePtr	- Pointer to the file we want to append             */
/*               p_appendixFileSize	- The size of the file we want to append            */
/*                                                                                      */
/* Returns:      Appended file offset                                                   */
/*                                                                                      */                                                                                      
/* Side effects: None                                                                   */
/*                                                                                      */ 
/* Description:                                                                         */ 
/*               Appending a file to the end of the executable                          */                                     
/*--------------------------------------------------------------------------------------*/

long AppendFileFromRAMToExe(char *p_exeFileName,
							char *p_appendixFilePtr,
							unsigned long p_appendixFileSize);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        updateMessagesAndHelpFile                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cmdLineInfo -      Command line parameters regarding the message and help file         */
/*                  p_srcExeFileName - The executable full path                                            */
/*                  p_targetPath -     A temp directory                                                    */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine updates the readme & messages files if needed and                         */
/*                      generates                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int updateMessagesAndHelpFile(char *p_targetPath, char *p_srcExeFileName, AppendixesCmmandLineInfo* cmdLineInfo);

char * getAppendixFilePtr(char *p_exeFileName, size_t *p_fileSize, int p_appendixNumber);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif

