/**
* UDiskReader.h - the reader of UDisk.
* Copyright (c) 2015 CreatBot. All Right Reserved.
* Developer Lyn Lee, 13 June 2016.
*/

#include "UDisk_SD_impl_CH376.h"
#ifdef UDISKSUPPORT

#ifndef U_DISK_READER_H
#define U_DISK_READER_H

#include "UDiskStructs.h"

class USBFile {
public:
	USBFile() { init(); }
	void init();
	void set(dir_t* dir);
	void setLongFilename(vfat_t* vfat, uint8_t n);
	void setLongFilenameFromName83();
	char filename[FILENAME_LENGTH], longFilename[LONG_FILENAME_LENGTH];
	uint32_t cluster, filesize, modifyDT;
	bool isRoot, isSubDir, isFile;
private:
	dir_t info;
	uint8_t checkNameSum();
};

class UDiskReader {
public:
	UDiskReader();
	void init();
	void refresh();

	uint16_t getNr();
	void selectFile(uint16_t index);
	void selectWorkDir();
	void getAbsFilename(char *t);
	void getAbsWorkPath(char *t);

	void ls();
	void setroot();
	void chdir(const char* path);
	void updir();

	void startPrint();
	void pausePrint();
	void stopPrint();
	void release();
  void openAndPrintFile(const char *name);
	void getStatus();
	void printingHasFinished();

	void openFile(char *name, bool read = true, bool replace_current = true);
	void closefile();
	int16_t get();
	void setIndex(long index);
	
	bool UDiskPrintState, UDiskPauseState, UDiskConn, UDiskOK, isDir, isFileOpen, saving, logging;
	char filename[FILENAME_LENGTH], longFilename[LONG_FILENAME_LENGTH];

	bool eof() { return filePos >= fileSize; }
	float percentDoneF() { return (isFileOpen && fileSize) ? (float)filePos / fileSize * 100 : 0; }
	uint32_t getSdPos() { return filePos; }
	
private:
	void getUSBInfo();
	void getCurFile();
	void getFileInfo();
	void setSeek(USBFile &dir);

	static void list_print();
	static void list_count();
	static uint16_t list_get();
	
	USBFile root, *curfile, workDir, workDirParents[MAX_DIR_DEPTH], file;
	uint32_t fileSize;
	uint32_t filePos;
	uint16_t workDirDepth;
};

extern UDiskReader UDisk;

#define IS_UDISK_PRINT			(UDisk.UDiskPrintState)
#define IS_UDISK_PAUSE			(UDisk.UDiskPauseState)
#define IS_UDISK_IDLE				(!IS_UDISK_PRINT && !IS_UDISK_PAUSE)

#define UDISK_STATE					(UDisk.UDiskOK | (UDisk.UDiskConn<<1))
#define IS_UDISK_CONN				((UDISK_STATE & 0x02) == 0x02)
#define IS_UDISK_OK					((UDISK_STATE & 0x03) == 0x03)

#define IS_UDISK_DIR				(UDisk.isDir)
#define IS_UDISK_FILE_OPEN	(UDisk.isFileOpen)

#endif //U_DISK_READER_H
#endif //UDISKSUPPORT

