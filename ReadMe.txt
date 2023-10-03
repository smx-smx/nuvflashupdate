-----------------------------------------------------------------------
Nuvoton Technology Corporation - Confidential
Flash Update Source Code for Linux Package
Release Letter
Copyright (c) 2010 Nuvoton Technology Corporation. All Rights Reserved.
-----------------------------------------------------------------------

Flash Update Source Code for Linux to be used with Nuvoton WPC876xL, 
WPCE77nx, NPCE78mx and NPCE79nx Notebook Embedded Controllers (henceforth 
referred to as "EC") Release Letter.

Version:  4.3.2
Date:     July 2010

Please read carefully the included Source Code License Agreement in the file 
Nuvoton_SourceCodeLicenseAgreement.txt.

PACKAGE CONTENTS
----------------
   This zip file contains the following files:

   * *.c, *.h                               - Flash Update source files
   * drv                                    - Directory with IO/Memory driver
   * Flupdate.cfg                           - Flash Update configuration file
   * Howtobuild.txt                         - Guidance to build the project
   * makefile                               - Project's makefile
   * Nuvoton_SourceCodeLicenseAgreement.txt - Source Code License Agreement
   * ReadMe.txt                             - This Release Letter

DESCRIPTION
-----------
   This product is the Nuvoton Flash Update tool for Linux running on x86 system, 
   provided as source code.
   
REQUIREMENTS
------------
   * Operating System:
         Linux. 
         Tested on ubuntu 9.10 kernel 2.6.31.14
    
   * Hardware:
        A platform containing a Nuvoton EC device with appropriate firmware.
        The EC firmware is provided in a separate package. For a detailed description
        of the EC firmware functionality, see its documentation.

ENHANCEMENTS
------------
   * Add Linux 64 bits support

FIXED ISSUES
------------
   None

KNOWN ISSUES
------------
   None

HISTORY
-------
Version 4.3.1
  ENHANCEMENTS :
     * The tool supports both I/O and Memory access.   
     * pcilib is not required in the build process.
Version 4.3.0
  KNOWN ISSUES :
     * The tool currently supports only IO access (Memory access is planned for 
       future releases).
