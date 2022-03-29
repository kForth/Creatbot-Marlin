#ifndef U_DISK_STRUCTS_H
#define U_DISK_STRUCTS_H

#ifdef UDISKSUPPORT

#define PACKED __attribute__((__packed__))

#define FILENAME_LENGTH 13
#define MAX_VFAT_ENTRIES (3)
#define LONG_FILENAME_LENGTH (FILENAME_LENGTH*MAX_VFAT_ENTRIES+1)

#define MAX_DIR_DEPTH 3
#define MAXPATHNAMELENGTH (FILENAME_LENGTH*MAX_DIR_DEPTH + MAX_DIR_DEPTH + 1)

// name
#define NAME_0XE5			0x05
#define NAME_DELETED		0xE5
#define NAME_FREE			0x00
#define NAME_DOT			0x2E

// attributes
#define ATT_READ_ONLY		0x01
#define ATT_HIDDEN			0x02
#define ATT_SYSTEM			0x04
#define ATT_LABEL			0x08
#define ATT_DIRECTORY		0x10
#define ATT_ARCHIVE			0x20
#define ATT_LONG_NAME		0x0F

#define MASK_LONG_NAME		0x3F
#define MASK_LONG_NAME_SEQ	0x1F
#define MASK_LONG_NAME_END	0x40
#define MASK_FILE			(ATT_DIRECTORY | ATT_LABEL)

typedef struct directoryEntry {
	uint8_t name[11];				// short 8.3 name.
	uint8_t attributes;				// the attributes of dir.
	uint8_t reservedNT;				// no use.
	uint8_t creationTimeTenths;		// value range is 0-199,  unit is 10 ms.
	uint16_t creationTime;			// bits 15-11 is hours, range is 0-24;
									// bits 10-5 is minutes, range is 0-59;
									// bits 4-0 is second, range is 0-29, uint is 2s.
	uint16_t creationDate;			// bits 15-9 is year, range is 0-127, (add the 1980, range is 1980-2107);
									// bits 8-5 is month, range is 1-12;
									// bits 4-0 is day, range is 1-31.
	uint16_t lastAccessDate;		// last access date.
	uint16_t firstClusterHigh;		// the high byte of first cluster.
	uint16_t lastWriteTime;			// last write time.
	uint16_t lastWriteDate;			// last write date.
	uint16_t firstClusterLow;		// the low byte of first cluster.
	uint32_t fileSize;				// the file's size.
}  PACKED dir_t;

typedef struct directoryVFATEntry {
	uint8_t sequenceNumber;			// bit 6 is the end of long filename. bits 4-0 is the index of long filename.
	uint16_t name1[5];				// UTF-16, first block of the long filename.
	uint8_t attributes;				// always is 0x0F.
	uint8_t reservedNT1;			// no use.
	uint8_t checksum;				// the checksum of short name.
	uint16_t name2[6];				// UTF-16, second block of the long filename.
	uint16_t reservedNT2;			// no use.
	uint16_t name3[2];				// UTF-16, third block of the long filename.
 } PACKED vfat_t;

struct fileEntry {
	dir_t file;
	
	fileEntry* upDir;
};

#endif //UDISKSUPPORT

#endif // U_DISK_STRUCTS_H
