

OUTPUT_DIR = .
TGT= flupdate

CC=	gcc
LINK=	wlink


CFLAGS  = -g -Wall -O2 -static -c
CEXTRA  = -D__LINUX__
INCLUDE = -I.





all: $(OUTPUT_DIR)/$(TGT)

Main.o: Main.c
	$(CC) $(CFLAGS) $(CEXTRA) $(INCLUDE) Main.c
	
AppDisplay.o: AppDisplay.h FUresult.h
	$(CC) $(CFLAGS) $(CEXTRA) $(INCLUDE) AppDisplay.c

LinuxInterface.o: LinuxInterface.c
	$(CC) $(CFLAGS) $(CEXTRA) $(INCLUDE) LinuxInterface.c

PCInterface.o: PCInterface.c PCInterface.h
	$(CC) $(CFLAGS) $(CEXTRA) $(INCLUDE) PCInterface.c

FUCore.o: FUCore.c FUCore.h
	$(CC) $(CFLAGS) $(CEXTRA) $(INCLUDE) FUCore.c

FlashAccess.o: FlashAccess.c FlashAccess.h
	$(CC) $(CFLAGS) $(CEXTRA) $(INCLUDE) FlashAccess.c
    
FUinterface.o: FUinterface.c FUInterface.h
	$(CC) $(CFLAGS) $(CEXTRA) $(INCLUDE) FUinterface.c

FUScript.o: FUScript.c FUScript.h
	$(CC) $(CFLAGS) $(CEXTRA) $(INCLUDE) FUScript.c

App.o: App.c App.h FUresult.h ErrorCodes.h
	$(CC) $(CFLAGS) $(CEXTRA) $(INCLUDE) App.c
	
FUresult.o: FUresult.c FUresult.h
	$(CC) $(CFLAGS) $(CEXTRA) $(INCLUDE) FUresult.c

ErrorCodes.o: ErrorCodes.c ErrorCodes.h
	$(CC) $(CFLAGS) $(CEXTRA) $(INCLUDE) ErrorCodes.c
	
OBS	= App.o AppDisplay.o FlashAccess.o LinuxInterface.o PCInterface.o FUCore.o FUinterface.o FUScript.o Main.o FUresult.o ErrorCodes.o
 

$(OUTPUT_DIR)/$(TGT): $(OBS)
	$(CC) $(OBS) -o $(OUTPUT_DIR)/$(TGT)
	bash ./drv_make.sh

copy:
	cp	../DOSapp/AppDisplay.c		.
	cp	../DOSapp/AppDisplay.h		.
	cp	../DOSapp/Main.c		.

	cp	../Common/All/App.c		. 
	cp	../Common/All/App.h		. 
	cp	../Common/All/Flupdate.cfg	. 

	cp	../Common/Flupdate/Flupdcom.h	.

	cp	../FUcore/PCInterface.c		.
	cp	../FUcore/PCInterface.h		.
	cp	../FUcore/FUCore.c		.
	cp	../FUcore/FUCore.h		.
	cp	../FUcore/FlashAccess.c		.
	cp	../FUcore/FlashAccess.h		.
	cp	../FUcore/FUinterface.c		.
	cp	../FUcore/FUInterface.h		.
	cp	../FUcore/FUScript.c		.
	cp	../FUcore/FUScript.h		.
	cp	../FUcore/FUresult.c		.
	cp	../FUcore/FUresult.h		.
	cp	../FUcore/OSUtils.h		.

	cp	../lib/include/ErrorCodes.c	.
	cp	../lib/include/ErrorCodes.h	.

	cp	../lib/exe_archive.h		.


clean:
	rm *.o
	rm $(OUTPUT_DIR)/$(TGT)

.c.o:
	$(CC) $(CFLAGS) $(CEXTRA) -c $*.c




