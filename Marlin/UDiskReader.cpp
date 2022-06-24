/**
* UDiskReader.cpp - the reader of UDisk.
* Copyright (c) 2015 CreatBot. All Right Reserved.
* Developer Lyn Lee, 13 June 2016.
*/

#include "UDiskReader.h"
#ifdef UDISKSUPPORT

#include "Marlin.h"
#include "stepper.h"
#include "temperature.h"
#include "language.h"
#include "printcounter.h"
#include "power_loss_recovery.h"

void USBFile::init() {
	memset(&info, 0, sizeof(info));
	memset(filename, 0, sizeof(filename));
	memset(longFilename, 0, sizeof(longFilename));
	cluster = modifyDT = 0;
	filesize = -1;
	isSubDir = isFile = false;
	isRoot = true;
}

void USBFile::set(dir_t* dir) {
	init();

	memcpy(&info, dir, sizeof(info));
	cluster = ((uint32_t)info.firstClusterHigh << 16) | (uint32_t)info.firstClusterLow;
	modifyDT = ((uint32_t)info.lastWriteDate << 16) | (uint32_t)info.lastWriteTime;

	isSubDir = ((info.attributes & MASK_FILE) == ATT_DIRECTORY);
	isFile = ((info.attributes & MASK_FILE) == 0);
	isRoot = false;

	if (isFile)	filesize = info.fileSize;

	char *pos = filename;
	for (uint8_t i = 0; i < 11; i++) {
		if (info.name[i] == ' ') continue;
		if (i == 8) *pos++ = '.';
		*pos++ = info.name[i];
	}
	*pos++ = 0;
}

void USBFile::setLongFilename(vfat_t * vfat, uint8_t n){
	for (uint8_t i = 0; i < n; i++) {
		if ((i < MAX_VFAT_ENTRIES) && (vfat[i].checksum == checkNameSum())) {
			uint8_t seq = i * FILENAME_LENGTH;
			longFilename[seq + 0]	= vfat[i].name1[0] > 0x7F ? 0x2A : vfat[i].name1[0];
			longFilename[seq + 1]	= vfat[i].name1[1] > 0x7F ? 0x2A : vfat[i].name1[1];
			longFilename[seq + 2]	= vfat[i].name1[2] > 0x7F ? 0x2A : vfat[i].name1[2];
			longFilename[seq + 3]	= vfat[i].name1[3] > 0x7F ? 0x2A : vfat[i].name1[3];
			longFilename[seq + 4]	= vfat[i].name1[4] > 0x7F ? 0x2A : vfat[i].name1[4];
			longFilename[seq + 5]	= vfat[i].name2[0] > 0x7F ? 0x2A : vfat[i].name2[0];
			longFilename[seq + 6]	= vfat[i].name2[1] > 0x7F ? 0x2A : vfat[i].name2[1];
			longFilename[seq + 7]	= vfat[i].name2[2] > 0x7F ? 0x2A : vfat[i].name2[2];
			longFilename[seq + 8]	= vfat[i].name2[3] > 0x7F ? 0x2A : vfat[i].name2[3];
			longFilename[seq + 9]	= vfat[i].name2[4] > 0x7F ? 0x2A : vfat[i].name2[4];
			longFilename[seq + 10]	= vfat[i].name2[5] > 0x7F ? 0x2A : vfat[i].name2[5];
			longFilename[seq + 11]	= vfat[i].name3[0] > 0x7F ? 0x2A : vfat[i].name3[0];
			longFilename[seq + 12]	= vfat[i].name3[1] > 0x7F ? 0x2A : vfat[i].name3[1];
		} else {
			break;
		}
	}
}

uint8_t USBFile::checkNameSum(){
	uint8_t len, checkSum = 0; uint8_t *name = info.name;
	for (len = 11; len--;) checkSum = (checkSum & 1 ? 0x80 : 0x00) + (checkSum >> 1) + *name++;
	return checkSum;
}

static CH376_UDisk UDiskImpl; 
static uint16_t list_nr;
static dir_t temp;

UDiskReader::UDiskReader(){
	fileSize = 0;
	filePos = 0;
	UDiskPrintState = false;
	UDiskPauseState = false;
	UDiskConn = false;
	UDiskOK = false;
	isDir = false;
	isFileOpen = false;
	saving = false;
	logging = false;
	curfile = nullptr;
	workDirDepth = 0;
	memset(workDirParents, 0, sizeof(workDirParents));
}

void UDiskReader::init(){
	UDiskImpl.init(USB_HOST_READY_NO_SOF);		// set to mode (HOST_NO_SOF)
}

void UDiskReader::getUSBInfo() {
	uint8_t info[37] = { 0 };
	if (UDiskImpl.readBlock(info) == 36){
		SERIAL_ECHO((char*)(&info[8]));
		SERIAL_ECHOLNPGM(MSG_SD_CARD_OK);
	}
}

void UDiskReader::getCurFile() {
	strcpy(filename, curfile->filename);
	strcpy(longFilename, curfile->longFilename);
	isDir = curfile->isSubDir;
}

void UDiskReader::getFileInfo(){
	if (UDiskImpl.dirInfoRead()) {
		if (UDiskImpl.readBlock((uint8_t *)&temp) == sizeof(dir_t)) {
			USBFile tempFile;
			tempFile.set(&temp);
			curfile = &tempFile;
			getCurFile();
		}
	}
}

void UDiskReader::refresh(){
	if (UDiskImpl.IsReady() && UDiskImpl.InterruptHandle()) {
#ifdef UDisk_IMPL_NOT_PNP
		if(!UDiskConn && UDiskImpl.isDeviceState(UDISK_INSERT) && UDiskImpl.IsDiskConnect()){
			UDiskConn = true;
			SERIAL_ECHOLNPGM(MSG_SD_INSERTED);
			return;
		}

		if(UDiskConn){
			if(!UDiskOK && UDiskImpl.diskMount()){
				UDiskImpl.setDeviceState(UDISK_READY);
				getUSBInfo();
				UDiskOK = true;

				if (UDiskPauseState) {
					setSeek(workDir);
					if (UDiskImpl.fileOpen(file.filename)){
						isFileOpen = true;
					} else{
						stopPrint();
					}
				} else{
					setroot();
				}
			}

			if(UDiskImpl.isDeviceState(UDISK_REMOVE)){
				UDiskImpl.setDeviceState(CH376_INIT);
				UDiskConn = false;
				UDiskOK = false;
				SERIAL_ECHOLNPGM(MSG_SD_REMOVED);
			}
		}
#else
		if (UDiskImpl.IsDiskConnect()) {
			if (!UDiskConn) {
				UDiskConn = true;
				SERIAL_ECHOLNPGM(MSG_SD_INSERTED);
				return;
			}

			if (!UDiskOK && UDiskImpl.diskMount()) {
				getUSBInfo();
				UDiskOK = true;
				if (UDiskPauseState) {
					setSeek(workDir);

					if (UDiskImpl.fileOpen(file.filename)){
						isFileOpen = true;
					} else{
						stopPrint();
					}
				}
				else
					setroot();
			}
		} else {
			if (UDiskConn) {
				UDiskConn = false;
				UDiskOK = false;
				SERIAL_ECHOLNPGM(MSG_SD_REMOVED);
			}
		}
#endif
	} else {
#ifdef UDISK_DEBUG
		if (UDiskImpl.getState() == ERR_USB_UNKNOWN) {
			SERIAL_ECHOLNPGM(MSG_CH376_ERR_CONN);
		} else if (UDiskImpl.getState() == ERR_USB_MODE) {
			SERIAL_ECHOLNPGM(MSG_CH376_ERR_MODE);
		} else {
			SERIAL_ECHOLNPGM(MSG_CH376_ERR_UNKNOW);
		}
#endif //UDISK_DEBUG
		if (UDiskConn) {
			UDiskConn = false;
			UDiskOK = false;
			SERIAL_ECHOLNPGM(MSG_SD_REMOVED);
		}
		init();
	}
}

uint16_t UDiskReader::getNr() {
	setSeek(workDir);
	list_nr = 0;
	UDiskImpl.listFile((uint8_t *)(&temp), list_count);
	return list_nr;
}


void UDiskReader::selectFile(uint16_t index){
	setSeek(workDir);
	list_nr = 0;

	UDiskImpl.listFile((uint8_t *)(&temp), list_get, index);

	USBFile tempFile;
	tempFile.set(&temp);

	/***************************** Get longFilename **********************************/
	UDiskImpl.fileOpen(tempFile.filename);
	uint8_t id = UDiskImpl.getfileDirIndex();
	uint32_t curDirInfoLBA = UDiskImpl.getDirInfoLBA();

	vfat_t tempVfats[MAX_VFAT_ENTRIES], tempVfat; uint8_t num = MAX_VFAT_ENTRIES; ZERO(tempVfats);
	while (true) {
		if (id == 0) {

			uint32_t secOffNums = 0, tempLBA;

			if (workDir.isSubDir) {
				setSeek(workDirParents[0]);
				UDiskImpl.fileOpen(workDir.filename);
			} else 	setroot();

			//setSeek(workDir);
			//if (workDir.isSubDir) {
			//	usb.fileOpen(".");
			//}
			//else if(workDir.isRoot) {
			//	usb.fileOpen("/");
			//}

			while (true) {
				if (UDiskImpl.setSecOffset(secOffNums)) {
					UDiskImpl.readBlock((uint8_t *)&tempLBA);
					if (tempLBA == curDirInfoLBA)	break;
					UDiskImpl.setDirInfoLBA(tempLBA);
				} else{
					break;
				}
				secOffNums++;
			}
			if (UDiskImpl.getCurOffSet() == 0) {
				num = 0; break;
			}

			id = DEF_SECTOR_SIZE / sizeof(dir_t);
		}

		if (UDiskImpl.dirInfoRead(--id) && (UDiskImpl.readBlock((uint8_t *)&temp) == sizeof(dir_t)) && ((temp.attributes & MASK_LONG_NAME) == ATT_LONG_NAME)) {
			tempVfat = *((vfat_t*)(&temp));
			uint8_t seq = (tempVfat.sequenceNumber & MASK_LONG_NAME_SEQ);

			if (seq - 1 < MAX_VFAT_ENTRIES) {
				tempVfats[seq - 1] = tempVfat;
			} else	break;

			if (tempVfat.sequenceNumber & MASK_LONG_NAME_END) {
				num = seq;		break;
			}
		} else {
			num = 0;	break;
		}
	}

	if (num) {
		tempFile.setLongFilename(tempVfats, num);
	}
	/*********************************************************************************/

	curfile = &tempFile;
	getCurFile();
}

void UDiskReader::selectWorkDir(){
	curfile = &workDir;
	getCurFile();
}

void UDiskReader::getAbsFilename(char *t) {
	getAbsWorkPath(t);
	if (t[0]) {
		if(strlen(t) > 1)	strcat(t, "/");
		strcat(t, file.filename);
	}
}

void UDiskReader::printFilename(){
  if (isFileOpen) {
    SERIAL_ECHO(file.filename);
    #if ENABLED(LONG_FILENAME_HOST_SUPPORT)
      if (file.longFilename[0]) {
        SERIAL_ECHO(' ');
        SERIAL_ECHO(file.longFilename);
      }
    #endif
  }
  else
    SERIAL_ECHOPGM("(no file)");

  SERIAL_EOL();
}

void UDiskReader::getAbsWorkPath(char *t){
	uint8_t cnt = 0;
	*t = '/'; t++; cnt++;

	if (workDirDepth > 0) {
		if (workDirDepth > 1) {
			for (uint8_t i = workDirDepth - 1; i > 0; i--) {
				strcpy(t, workDirParents[i - 1].filename);
				size_t len = strlen(workDirParents[i - 1].filename);
				t += len;	cnt += len;
				*t++ = '/'; cnt++;
			}
		}
		strcpy(t, workDir.filename);
		size_t len = strlen(workDir.filename);
		t += len;	cnt += len;
	}
	*t = 0;
	if (cnt >= MAXPATHNAMELENGTH - FILENAME_LENGTH - 1) {
		t[0] = 0;
	}
}



bool list_filter() {
	if (temp.name[0] == NAME_0XE5) temp.name[0] = 0xE5;				//replace the 0x05 to 0xE5
	if (temp.name[0] == NAME_DOT) return false;							//filter out the . and ..
	if ((temp.attributes & ATT_SYSTEM) == ATT_SYSTEM) return false;		//filter out the system file
	if (!((temp.attributes & MASK_FILE) == ATT_DIRECTORY)
		& ((temp.name[8] != 'G') || (temp.name[9] == '~'))) return false;		//filter the gcode only.
	return true;
}

void UDiskReader::list_print(){
	if (list_filter()) {
		USBFile tempFile;
		tempFile.set(&temp);

		if (tempFile.longFilename[0]) {
			SERIAL_ECHOLN(tempFile.longFilename);
		} else {
			SERIAL_ECHOLN(tempFile.filename);
		}
	}
}

void UDiskReader::list_count(){
	if (list_filter()) {
		list_nr++;
	}
}

uint16_t UDiskReader::list_get(){
	return (list_filter()) ? (++list_nr) : (list_nr);
}


void UDiskReader::setSeek(USBFile &dir) {
	closefile();
	UDiskImpl.fileOpen("/");		// must do this, or maybe cann't open file. ()
	UDiskImpl.setStartClus(dir.cluster);
}

void UDiskReader::setroot() {
	UDiskImpl.fileOpen("/");
	root.cluster = UDiskImpl.getRootClus();
	root.filename[0] = '/';

	workDir = root;
	curfile = &workDir;
	getCurFile();
	workDirDepth = 0;
}

void UDiskReader::ls(){
	setSeek(workDir);
	UDiskImpl.listFile((uint8_t *)(&temp), list_print);
	
	/**
	setroot();

	USBFile tempFile; dir_t tempDir; 
	vfat_t tempVfat[MAX_VFAT_ENTRIES]; uint8_t num = 0;
	for (uint8_t i = 0; i < 0x10; i++) {
		if (usb.dirInfoRead(i)) {
			uint8_t d[sizeof(dir_t)+1] = { 0 };
			if (usb.readBlock(d) == sizeof(dir_t)) {
				tempDir = *((dir_t *)d);
				if (tempDir.name[0] == NAME_FREE) break;
				if (tempDir.name[0] == NAME_DELETED) continue;
				if (tempDir.name[0] == NAME_DOT) continue;
				//if (tempDir.name[8] != 'G' ||tempDir.name[9] == '~') continue;

				if ((tempDir.attributes & MASK_LONG_NAME) == ATT_LONG_NAME) {
					vfat_t temp = *((vfat_t*)d);
					uint8_t seq = (temp.sequenceNumber & MASK_LONG_NAME_SEQ);

					if (temp.sequenceNumber & MASK_LONG_NAME_END) {
						num = seq;
					}
					if(seq - 1 < MAX_VFAT_ENTRIES){
						tempVfat[seq - 1] = temp;
					}
					continue;
				}

				tempFile.set(&tempDir);
				if (num) {
					tempFile.setLongFilename(tempVfat, num);
					num = 0;
				}

				if (tempFile.longFilename[0]) {
					SERIAL_ECHOLN(tempFile.longFilename);
				} else {
					SERIAL_ECHOLN(tempFile.filename);
				}

			}
		}
	}
	**/
}

void UDiskReader::chdir(const char* path){
	setSeek(workDir);
	bool open = UDiskImpl.fileOpen(path);
	uint8_t state = UDiskImpl.getState();

	if (open) {
		if (state == USB_INT_SUCCESS) {
			SERIAL_ECHOPGM(MSG_SD_OPEN_FILE_FAIL);
			SERIAL_ECHOLN(path);
			return;
		} else{
			if (UDiskImpl.dirInfoRead()) {
				uint8_t d[sizeof(dir_t) + 1] = { 0 };
				if (UDiskImpl.readBlock(d) == sizeof(dir_t)) {
					if (workDirDepth < MAX_DIR_DEPTH) {
						for (int i = workDirDepth; i--;) workDirParents[i + 1] = workDirParents[i];
						++workDirDepth;
						workDirParents[0] = workDir;
						workDir.set((dir_t *)d);
					}
				}
			}		
			setSeek(workDir);
		}
	}

	/***************** open the multi-level directory *******************
	bool open = usb.fileOpenPath(path);
	uint8_t state = usb.getState();

	// if path is file, open the last dir.
	if (state == USB_INT_SUCCESS) {
		uint8_t endPos = usb.getPathLastDir(path);
		open = usb.fileOpenPath(path, endPos);
		state = usb.getState();
	}

	if (open) {
		if (usb.dirInfoRead()) {
			uint8_t d[sizeof(dir_t) + 1] = { 0 };
			if (usb.readBlock(d) == sizeof(dir_t)) {
				workDir.set((dir_t *)d);
			}

			curfile = &workDir;
			setSeek(workDir);
		}
	} else {
		if (state == ERR_MISS_FILE || state == ERR_MISS_DIR) {
			SERIAL_ECHOLNPGM(MSG_SD_CANT_OPEN_SUBDIR);
		} else if (state == ERR_FOUND_NAME){
			SERIAL_ECHOPGM(MSG_SD_OPEN_FILE_FAIL);
			SERIAL_ECHOLNPGM(path);
		} else {
			SERIAL_ECHOLNPGM(MSG_SD_CANT_OPEN_SUBDIR);
		}
	}
	**********************************************************************/
}

void UDiskReader::updir(){
	if (workDirDepth > 0) {
		--workDirDepth;
		workDir = workDirParents[0];
		for (uint8_t i = 0; i < workDirDepth; i++) {
			workDirParents[i] = workDirParents[i + 1];
		}
	}
	setSeek(workDir);
}

void UDiskReader::startPrint(){
	if (UDiskOK) {
		UDiskPrintState = true;
		UDiskPauseState = false;
	}
}

void UDiskReader::pausePrint(){
	if (UDiskPrintState) {
		UDiskPrintState = false;
		UDiskPauseState = true;
	}
}

void UDiskReader::stopPrint(){
	UDiskPrintState = UDiskPauseState = aborting = false;
  if(isFileOpen) closefile();
}

void UDiskReader::openLogFile(char * const path) {
  logging = true;
  openFile(path, false);
}

void UDiskReader::release(){
	UDiskPrintState = false;
	UDiskOK = false;
}

void UDiskReader::openAndPrintFile(const char *name){
	char cmd[6 + strlen(name) + 1];
	sprintf_P(cmd, PSTR("M6023 %s"), name);
	for(char *c = &cmd[6]; *c; c++)	*c = tolower(*c);
	enqueue_and_echo_command(cmd);
	enqueue_and_echo_commands_P(PSTR("M6024"));
}

void UDiskReader::getStatus(){
	if (UDiskOK) {
		SERIAL_ECHOPGM(MSG_SD_PRINTING_BYTE);
		SERIAL_ECHO(filePos);
		SERIAL_ECHOPGM("/");
		SERIAL_ECHOLN(fileSize);
	} else
		SERIAL_ECHOLNPGM(MSG_SD_NOT_PRINTING);
}

void UDiskReader::printingHasFinished(){
	planner.synchronize();
	closefile();
	if(UDiskPauseState)	UDiskPauseState = false;
	UDiskPrintState = false;
	if (SD_FINISHED_STEPPERRELEASE) {
		enqueue_and_echo_commands_P(PSTR(SD_FINISHED_RELEASECOMMAND));
	}
	print_job_timer.stop();
	if(print_job_timer.duration() > 60)
		enqueue_and_echo_commands_P(PSTR("M31"));
}


void UDiskReader::openFile(char * name, bool read, bool replace_current){
	if (!UDiskOK)	return;

	uint8_t doing = 0;
	if (isFileOpen) {
		if (!replace_current) {
			//TODO - subcall
		}
		else 
      		doing = 1;
	}
	else 
		doing = 2;

	if (doing) {
		SERIAL_ECHO_START();
		SERIAL_ECHOPGM("Now ");
		serialprintPGM(doing == 1 ? PSTR("doing") : PSTR("fresh"));
		SERIAL_ECHOLNPAIR(" file: ", name);
	}

	stopPrint();

	char *fname = name;
	char *dirname_start, *dirname_end;
	if (name[0] == '/') {
		setroot();
		dirname_start = &name[1];
		while (dirname_start != NULL) {
			dirname_end = strchr(dirname_start, '/');
			if (dirname_end != NULL && dirname_end > dirname_start) {
				char subdirname[FILENAME_LENGTH];
				strncpy(subdirname, dirname_start, dirname_end - dirname_start);
				subdirname[dirname_end - dirname_start] = 0;
				//SERIAL_ECHOLN(subdirname);
				chdir(subdirname);
				dirname_start = dirname_end + 1;
			} else {
				fname = dirname_start;
				break;
			}
		}
		if (!fname[0]) return;
	} else {
		setSeek(workDir);
	}

	if (read) {
		if (UDiskImpl.fileOpen(fname)) {
			getFileInfo();
			file = *curfile;
			isFileOpen = true;
			fileSize = curfile->filesize;

			SERIAL_ECHOPGM(MSG_SD_FILE_OPENED);
			SERIAL_ECHO(fname);
			SERIAL_ECHOPGM(MSG_SD_SIZE);
			SERIAL_ECHOLN(fileSize);
			filePos = 0;

			SERIAL_ECHOLNPGM(MSG_SD_FILE_SELECTED);

		} else {
			SERIAL_ECHOPGM(MSG_SD_OPEN_FILE_FAIL);
			SERIAL_ECHO(fname);
			SERIAL_ECHOLNPGM(".");
		}
	} else {
		if (UDiskImpl.fileOpen(fname)) {
			getFileInfo();
			file = *curfile;
			isFileOpen = true;
			fileSize = curfile->filesize;

			SERIAL_ECHOPGM(MSG_SD_FILE_OPENED);
			SERIAL_ECHO(fname);
			SERIAL_ECHOPGM(MSG_SD_SIZE);
			SERIAL_ECHOLN(fileSize);
			filePos = 0;

			SERIAL_ECHOLNPGM(MSG_SD_FILE_SELECTED);

		} else {
			SERIAL_ECHOPGM(MSG_SD_OPEN_FILE_FAIL);
			SERIAL_ECHO(fname);
			SERIAL_ECHOLNPGM(".");
		}
	}

}

void UDiskReader::removeFile(char * const name) {
  	if (!UDiskOK) return;
	stopPrint();
	openFile(name);
	if (UDiskImpl.fileErase(name)) {
		SERIAL_PROTOCOLPGM("File deleted:");
		SERIAL_PROTOCOLLN(name);
		filePos = 0;
	}
	else {
		SERIAL_PROTOCOLPGM("Deletion failed, File: ");
		SERIAL_PROTOCOL(name);
		SERIAL_PROTOCOLCHAR('.');
	}
}

void UDiskReader::closefile() {
	UDiskImpl.fileClose();
	isFileOpen = false;
	saving = logging = false;
}

int16_t UDiskReader::get(){
	uint8_t get_char;

	filePos = UDiskImpl.getOffset();

	get_char = UDiskImpl.readByte();
	if (UDiskImpl.getOffset() == 1) {
		filePos = 1;
	}
	
	return (int16_t)get_char;
}

void UDiskReader::setIndex(long index){
	filePos = index;
	UDiskImpl.setOffset(index);
}

// #if ENABLED(POWER_LOSS_RECOVERY)

// 	char job_recovery_file_name[4] = "bin";

// 	void UDiskReader::openJobRecoveryFile(const bool read) {
// 		if (!UDiskOK) return;
// 		if (recoveryFileisOpen) return;

// 		recoveryFileisOpen = UDiskImpl.fileOpen(job_recovery_file_name);
// 		//jobRecoveryFile.open(&root, job_recovery_file_name, read ? O_READ : O_CREAT | O_WRITE | O_TRUNC | O_SYNC)
// 		if(!recoveryFileisOpen) {
// 			SERIAL_PROTOCOLPAIR(MSG_SD_OPEN_FILE_FAIL, job_recovery_file_name);
// 			SERIAL_PROTOCOLCHAR('.');
// 			SERIAL_EOL();
// 		}
// 		else if (!read)
// 			SERIAL_PROTOCOLLNPAIR(MSG_SD_WRITE_TO_FILE, job_recovery_file_name);
// 		else
// 			SERIAL_PROTOCOLLNPAIR(MSG_SD_FILE_OPENED, job_recovery_file_name);
// 		recoveryFileisOpen = true;
// 	}

//   	void UDiskReader::closeJobRecoveryFile() {
// 		UDiskImpl.fileClose(); // jobRecoveryFile.close();
// 		recoveryFileisOpen = false;
// 	}

// 	bool UDiskReader::jobRecoverFileExists() {
// 		const bool exists = UDiskImpl.fileOpen(job_recovery_file_name);  // jobRecoveryFile.open(&root, job_recovery_file_name, O_READ);
// 		if (exists) UDiskImpl.fileClose();  // jobRecoveryFile.close();
// 		return exists;
// 	}

// 	int16_t UDiskReader::saveJobRecoveryInfo() {
// 		UDiskImpl.setOffset(0); //jobRecoveryFile.seekSet(0);
// 		UDiskImpl.writeBlock((uint8_t*)&job_recovery_info, sizeof(job_recovery_info));
// 		// const int16_t ret = jobRecoveryFile.write(&job_recovery_info, sizeof(job_recovery_info));
// 		// #if ENABLED(DEBUG_POWER_LOSS_RECOVERY)
// 		// 	if (ret == -1) SERIAL_PROTOCOLLNPGM("Power-loss file write failed.");
// 		// #endif
// 		// return ret;
// 	}

// 	int16_t UDiskReader::loadJobRecoveryInfo() {
// 		return UDiskImpl.readBlock(&job_recovery_info, sizeof(job_recovery_info));
// 		// return jobRecoveryFile.read(&job_recovery_info, sizeof(job_recovery_info));
// 	}

// 	void UDiskReader::removeJobRecoveryFile() {
// 		job_recovery_info.valid_head = job_recovery_info.valid_foot = job_recovery_commands_count = 0;
// 		if (jobRecoverFileExists()) {
// 		closefile();
// 		removeFile(job_recovery_file_name);
// 		#if ENABLED(DEBUG_POWER_LOSS_RECOVERY)
// 			SERIAL_PROTOCOLPGM("Power-loss file delete");
// 			serialprintPGM(jobRecoverFileExists() ? PSTR(" failed.\n") : PSTR("d.\n"));
// 		#endif
// 		}
// 	}

// #endif // POWER_LOSS_RECOVERY

#endif	//UDISKSUPPORT
