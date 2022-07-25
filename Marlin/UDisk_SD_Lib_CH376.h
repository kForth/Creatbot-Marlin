/**
 * UDisk_SD_Lib_CH376.h - CH376 Common instruction library
 * Copyright (c) 2016 CreatBot. All Right Reserved.
 * Developer Lyn Lee, 18 March 2016.
 */

#include "Arduino.h"

#ifndef __CH376INC_H__
#define __CH376INC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Hardware characteristics*/
#define	CH376_DAT_BLOCK_LEN	0x40			/* USB single packet, maximum length of data block, default buffer length*/


/* **********************************************************************************************************************/
/* Command code*/
/* Some commands are compatible with CH375 chips, but the possible part of the input data or output data is partially different)*/
/* aCommandOperationOrderIncludes:
A command code (for the serial port method, two synchronous codes need to be needed before the command code),
Several input data (which can be 0),
Generate interrupt notification or several output data (can be 0).
Only CMD01_WR_REQ_DATA command exception, the order includes: a command code, an output data, several input data
Command code naming rules: cmdxy_name
The X and Y of which are numbers, x indicates that the minimum number of data (byte number) is entered, and y indicates the minimum number of output data (byte number).
Some commands can be read and write with data blocks with 0 to multiple bytes. The number of bytes of the data block itself is not included in X or Y above */
/* This file will provide command code format compatible with CH375 chip command code at the same time (that is, after X and Y).*/

/* **********************************************************************************************************************/
/* Main command (manual 1), commonly used*/
#define	CMD01_GET_IC_VER	0x01			/*Get chip and firmware version*/
											/*Output: Version number (bit 7 is 0, bit 6 is 1, bit 5 ~ bit 0 is the version number))*/
											/*The value of CH376 returns the version number is 041H, that is, the version number is 01H*/

#define	CMD21_SET_BAUDRATE	0x02			/*Serial port method: Set the serial communication baud rate (the default baud rate after power -on or reset is 9600bps, selected by D4/D5/D6 pins))*/
											/*Enter: Potter rate division coefficient, baud rate frequency frequency constant*/
											/*Output: operating status (CMD_RET_SUCCCESS or CMD_RET_ABORT, other values indicate that the operation is not completed)*/

#define	CMD00_ENTER_SLEEP	0x03			/*Enter sleep state*/

#define	CMD00_RESET_ALL		0x05			/*Perform hardware reset*/

#define	CMD11_CHECK_EXIST	0x06			/*Test communication interface and working status*/
											/*Enter: Any data*/
											/*Output: Input the data on the position of the data*/

#define	CMD20_CHK_SUSPEND	0x0B			/*Equipment method: Set the way to check the state of the USB bus hanging*/
											/*Input: data 10h, check method*/
											/*00h = Do not check the USB hanging, 04H = check the USB with 50ms as the interval, 05H = 10ms as the interval to check the USB hanging*/

#define	CMD20_SET_SDO_INT	0x0B			/*SPI interface method: Set the interrupt method of SPI's SDO pin*/
											/*Input: data 16h, interrupt method*/
											/*10h = Forbidden SDO pins are used for interrupt output, three -state output prohibited when SCS chip selection is invalid, 90H = SDO pin is also an interrupt request output when the SCS tablet is invalid*/

#define	CMD14_GET_FILE_SIZE	0x0C			/*Host file mode: Get the length of the current file*/
											/*Input: Data 68H*/
											/*Output: The length of the current file (total length of 32 bits, low bytes before)*/

#define	CMD50_SET_FILE_SIZE	0x0D			/*Host file mode: set the current file length*/
											/*Enter: Data 68h, the current file length (total length of 32 bits, low bytes before)*/

#define	CMD11_SET_USB_MODE	0x15			/*Set USB working mode*/
											/*Input: mode code*/
											/*00h = Unprepared device, 01H = enabled device method and use external firmware mode (lack of support), 02H = enabled device method and use built -in firmware mode mode*/
											/*03H = SD card host mode/unprepared host mode, used to manage and access files in SD cards*/
											/*04h = Unexpected hosting method, 05H = enabled hosting method, 06h = enabled hosting method and automatically generate SOF package, 07H = enabled hosting method, and reset the USB bus bus bus*/
											/*Output: operating status (CMD_RET_SUCCCESS or CMD_RET_ABORT, other values indicate that the operation is not completed)*/

#define	CMD01_GET_STATUS	0x22			/*Get the interrupt state and cancel the interrupt request*/
											/*Output: interrupt status*/

#define	CMD00_UNLOCK_USB	0x23			/*Equipment method: Release the current USB buffer*/

#define	CMD01_RD_USB_DATA0	0x27			/*Read the data block from the current USB interrupt endpoint buffer or the receiving buffer of the host endpoint*/
											/*Output: length, data stream*/

#define	CMD01_RD_USB_DATA	0x28			/*Equipment method: Read the data block from the current USB interrupt endpoint buffer and release the buffer, which is equivalent to CMD01_RD_USB_DATA0 + cmd00_unlock_usb*/
											/*Output: length, data stream*/

#define	CMD10_WR_USB_DATA7	0x2B			/*Equipment method: Write data block to send buffer to USB endpoint 2*/
											/*Input: length, data stream*/

#define	CMD10_WR_HOST_DATA	0x2C			/*Write data blocks to send buffer at the end point of the USB host*/
											/*Input: length, data stream*/

#define	CMD01_WR_REQ_DATA	0x2D			/*Write the data block of the request to the internal designated buffer*/
											/*Output: length*/
											/*Input: data stream*/

#define	CMD20_WR_OFS_DATA	0x2E			/*Write the data block to the internal buffer specify the offset address*/
											/*Input: offset, length, data stream*/

#define	CMD10_SET_FILE_NAME	0x2F			/*Host file mode: set the file name of the file to be operated*/
											/*Enter: The string ended with 0 (including the length of the end symbol 0 does not exceed 14 characters)*/



/***********************************************************************************************************************/
/*The main command (manual 1), commonly used, the following commands always generate interrupt notifications at the end of the operation, and there is always no output data*/

#define	CMD0H_DISK_CONNECT	0x30			/*Host file mode/does not support SD card: Check whether the disk is connected*/
											/*Output interruption*/

#define	CMD0H_DISK_MOUNT	0x31			/*Host file mode: initialize disk and test whether the disk is ready*/
											/*Output interruption*/

#define	CMD0H_FILE_OPEN		0x32			/*Host file mode: open the file or directory (folder), or enumerate files and directory (folder)*/
											/*Output interruption*/

#define	CMD0H_FILE_ENUM_GO	0x33			/*Host file mode: Continue to enumerate files and directory (folder)*/
											/*Output interruption*/

#define	CMD0H_FILE_CREATE	0x34			/*Host file mode: new file, if the file already exists, then delete*/
											/*Output interruption*/

#define	CMD0H_FILE_ERASE	0x35			/*Host file mode: Delete files. If you have already opened it, delete it directly, otherwise the file will be opened first and then deleted.*/
											/*Output interruption*/

#define	CMD1H_FILE_CLOSE	0x36			/*Host file mode: Close the currently opened file or directory (folder)*/
											/*Enter: Whether the length of the file is allowed to be updated*/
											/*00h = Forbidden update length, 01H = allowed update length*/
											/*Output interruption*/

#define	CMD1H_DIR_INFO_READ	0x37			/*Host file mode: read the directory information of the file*/
											/*Enter: Specify the index number of the directory information structure that needs to be read in the sector*/
											/*The index number range is 00h ~ 0FH, and the index number 0ffh is the file that has been opened.*/
											/*Output interruption*/

#define	CMD0H_DIR_INFO_SAVE	0x38			/*Host file mode: Save the directory information of the file*/
											/*Output interruption*/

#define	CMD4H_BYTE_LOCATE	0x39			/*Host file mode: Move the current file pointer in bytes as a unit*/
											/*Enter: The number of deviation bytes (total length of 32 bits, low bytes before)*/
											/*Output interruption*/

#define	CMD2H_BYTE_READ		0x3A			/*Host file mode: read data blocks from the current position in the byte*/
											/*Enter: The number of bytes requested by the request (total length 16 bits, low bytes before)*/
											/*Output interruption*/

#define	CMD0H_BYTE_RD_GO	0x3B			/*Host file mode: continuing byte reading*/
											/*Output interruption*/

#define	CMD2H_BYTE_WRITE	0x3C			/*Console file mode: Write data blocks to the current position in the units of bytes*/
											/*Enter: The number of bytes written in request (total length 16 bits, low bytes before)*/
											/*Output interruption*/

#define	CMD0H_BYTE_WR_GO	0x3D			/*Host file mode: continuing byte writing*/
											/*Output interruption*/

#define	CMD0H_DISK_CAPACITY	0x3E			/*Host file mode: query disk physical capacity*/
											/*Output interruption*/

#define	CMD0H_DISK_QUERY	0x3F			/*Host file mode: query disk space information*/
											/*Output interruption*/

#define	CMD0H_DIR_CREATE	0x40			/*Console file mode: Create a new directory (folder) and open it, if the directory already exists so directly*/
											/*Output interruption*/

#define	CMD4H_SEC_LOCATE	0x4A			/*Host file mode: Move the current file pointer in the sector*/
											/*Enter: The number of offset sectors (total length of 32 bits, low bytes before)*/
											/*Output interruption*/

#define	CMD1H_SEC_READ		0x4B			/*Host file mode/does not support SD card: Reading data blocks from the current position in the sector*/
											/*Enter: Number of sectors requested to read*/
											/*Output interruption*/

#define	CMD1H_SEC_WRITE		0x4C			/*Console file mode/does not support SD card: Write data blocks in the current location in the sector*/
											/*Enter: Number of sectors requested*/
											/*Output interruption*/

#define	CMD0H_DISK_BOC_CMD	0x50			/*Host/No SD card: command to execute Bulkonly transmission protocols for USB memory*/
											/*Output interruption*/

#define	CMD5H_DISK_READ		0x54			/*Hosting/No SD card: Read the physical sector from USB memory read*/
											/*Enter: LBA physical sector address (total length of 32 bits, low bytes in front), number of sector (01H ~ FFH)*/
											/*Output interruption*/

#define	CMD0H_DISK_RD_GO	0x55			/*Hosting/No SD card: Continue to perform physical sector reading operations of USB memory*/
											/*Output interruption*/

#define	CMD5H_DISK_WRITE	0x56			/*Hosting method/does not support SD cards: to write physical sector to USB memory*/
											/*Enter: LBA physical sector address (total length of 32 bits, low bytes in front), number of sector (01H ~ FFH)*/
											/*Output interruption*/

#define	CMD0H_DISK_WR_GO	0x57			/*Hosting/No SD card: Continue to execute the physical sector writing operation of the USB memory*/
											/*Output interruption*/


/***********************************************************************************************************************/
/*Auxiliary command (manual 2), not very commonly used or to be compatible with CH375 and CH372*/

#define	CMD10_SET_USB_SPEED	0x04			/*Set the USB bus speed, and automatically restore the full speed of 12Mbps when setting the USB work mode at each time CMD11_SET_USB_MODE sets the USB working mode.*/
											/*Input: bus speed code*/
											/*00h = 12Mbps full -speed fullspeed (default value), 01H = 1.5Mbps (only modified frequency), 02H = 1.5Mbps low speed Lowspeed*/

#define	CMD11_GET_DEV_RATE	0x0A			/*Host method: Data rate type of the currently connected USB device*/
											/*Input: Data 07H*/
											/*Output: data rate type*/
											/*Bit 4 is 1. 1.5Mbps low -speed USB device, otherwise it will be 12Mbps full -speed USB device*/

#define	CMD11_GET_TOGGLE	0x0A			/*Get the synchronization state of OUT transactions*/
											/*Input: Data 1AH*/
											/*Output: Synchronous status*/
											/*Bit 4 is 1 and OUT transactions are synchronized, otherwise OUT transactions are not synchronized*/

#define	CMD11_READ_VAR8		0x0A			/*Read the specified 8 -bit file system variable*/
											/*Input: variable address*/
											/*Output Data*/

//#define	CMD11_GET_MAX_LUN	= CMD11_READ_VAR8( VAR_UDISK_LUN )	/*Hosting: Get the maximum of USB memory and the current logic unit number*/

#define	CMD20_SET_RETRY		0x0B			/*Hosting: Set the number of retries of USB transactions operation*/
											/*Input: data 25h, the number of retries*/
											/*When the bit 7 is 0, you do not review it when you receive the NAK.5 ~ bit 0 is the number of retries after timeout*/

#define	CMD20_WRITE_VAR8	0x0B			/*Set the specified 8 -bit file system variable*/
											/*Input: variable address, data*/

//#define	CMD20_SET_DISK_LUN	= CMD20_WRITE_VAR8( VAR_UDISK_LUN )	/*Hosting: Set the current logic unit number of the USB memory*/

#define	CMD14_READ_VAR32	0x0C			/*Read the specified 32 -bit file system variable*/
											/*Input: variable address*/
											/*Output: data (total length of 32 bits, low bytes before)*/

#define	CMD50_WRITE_VAR32	0x0D			/*Set the specified 32 -bit file system variable*/
											/*Input: variable address, data (total length of 32 bits, low bytes before)*/

#define	CMD01_DELAY_100US	0x0F			/*Late 100us (the serial port is not supported)*/
											/*Output: Export 0 during the delay, the end output is not 0 at the end of the delay*/

#define	CMD40_SET_USB_ID	0x12			/*Equipment method: Set USB manufacturer VID and product PID*/
											/*Enter: manufacturer ID low byte, manufacturer ID high byte, product ID low byte, product ID high byte*/

#define	CMD10_SET_USB_ADDR	0x13			/*Set the USB address*/
											/*Input: address value*/

#define	CMD01_TEST_CONNECT	0x16			/*Hosting method/does not support SD card: Check USB device connection status*/
											/*Output: state (USB_INT_CONNECT or USB_INT_DISCONNECT or USB_INT_USB_READY, other values indicate that the operation is not completed)*/

#define	CMD00_ABORT_NAK		0x17			/*Host method: Give up the current NAK's retry*/

#define	CMD10_SET_ENDP2		0x18			/*Equipment method (non -supported): Set the receiver with USB endpoint 0*/
											/*Enter: Working method*/
											/*Bit 7 is 1 and 6 is a synchronous trigger bit, otherwise synchronous triggers will remain unchanged*/
											/*Bit 3 ~ Bit 0 is transaction response method: 0000-ready ACK, 1110- is busy nak, 1111-error stall*/

#define	CMD10_SET_ENDP3		0x19			/*Equipment method (non -supported): Set the transmitter with USB endpoint 0*/
											/*Enter: Working method*/
											/*Bit 7 is 1 and 6 is a synchronous trigger bit, otherwise synchronous triggers will remain unchanged*/
											/*Bit 3 ~ Bit 0 is transaction response method: 0000 ~ 1000- 位 位, 1110- is busy nak, 1111-error stall*/

#define	CMD10_SET_ENDP4		0x1A			/*Equipment method (non -supported): Set the receiver with USB endpoint 1*/
											/*Enter: Working method*/
											/*Bit 7 is 1 and 6 is a synchronous trigger bit, otherwise synchronous triggers will remain unchanged*/
											/*Bit 3 ~ Bit 0 is transaction response method: 0000-ready ACK, 1110- is busy nak, 1111-error stall*/

#define	CMD10_SET_ENDP5		0x1B			/*Equipment method (non -supported): Set the transmitter with USB endpoint 1*/
											/*Enter: Working method*/
											/*Bit 7 is 1 and 6 is a synchronous trigger bit, otherwise synchronous triggers will remain unchanged*/
											/*Bit 3 ~ Bit 0 is transaction response method: 0000 ~ 1000- 位 位, 1110- is busy nak, 1111-error stall*/

#define	CMD10_SET_ENDP6		0x1C			/*Set the receiver of the USB endpoint 2/host endpoint*/
											/*Enter: Working method*/
											/*Bit 7 is 1 and 6 is a synchronous trigger bit, otherwise synchronous triggers will remain unchanged*/
											/*Bit 3 ~ Bit 0 is the transaction response method: 0000-ready ACK, 1101- but not returning ACK, 1110- is busy nak, 1111-error stall*/

#define	CMD10_SET_ENDP7		0x1D			/*Set the sender of the USB endpoint 2/host endpoint*/
											/*Enter: Working method*/
											/*Bit 7 is 1 and 6 is a synchronous trigger bit, otherwise synchronous triggers will remain unchanged*/
											/*Bit 3 ~ Bit 0 is the transaction response method: 0000-ready ACK, 1101-ready but no need to respond, 1110- is busy nak, 1111-error stall*/

#define	CMD00_DIRTY_BUFFER	0x25			/*Console file mode: Clear internal disk and file buffer*/

#define	CMD10_WR_USB_DATA3	0x29			/*Equipment method (lack of support): Write data blocks to send buffer from USB endpoint 0*/
											/*Input: length, data stream*/

#define	CMD10_WR_USB_DATA5	0x2A			/*Equipment method (lack of support): Write data blocks to send buffer from USB endpoint 1*/
											/*Input: length, data stream*/

/***********************************************************************************************************************/
/*The auxiliary command (manual 2), not very commonly used or to be compatible with CH375 and CH372, the following commands always generate interrupt notifications at the end of the operation, and there is always no output data.*/

#define	CMD1H_CLR_STALL		0x41			/*Host method: control transmission-clear endpoint error*/
											/*Input: end point number*/
											/*Output interruption*/

#define	CMD1H_SET_ADDRESS	0x45			/*Hosting: Control transmission-Set USB address*/
											/*Input: address value*/
											/*Output interruption*/

#define	CMD1H_GET_DESCR		0x46			/*Host method: control transmission-obtain descriptor*/
											/*Input: descriptor type*/
											/*Output interruption*/

#define	CMD1H_SET_CONFIG	0x49			/*Host method: Control transmission-Set up USB configuration*/
											/*Input: configuration value*/
											/*Output interruption*/

#define	CMD0H_AUTO_SETUP	0x4D			/*Host/No SD card: Automatic configuration USB device*/
											/*Output interruption*/

#define	CMD2H_ISSUE_TKN_X	0x4E			/*Hosting: Synchronous token, execution transactions, this command can replace CMD10_SET_ET_ENDP6/CMD10_SET_ET_ET_ET7 + CMD1H_ISSUE_TOKEN*/
											/*Enter: Synchronous logo, transaction attribute*/
											/*The bit 7 of the synchronous logo 7 is the synchronous trigger position of the host endpoint IN. Bit 6 is the synchronous trigger bit of the host endpoint OUT. Bit 5 ~ bit 0 must be 0*/
											/*The lower 4 digits of the transaction attributes are token, and the high 4 digits are endpoint numbers*/
											/*Output interruption*/

#define	CMD1H_ISSUE_TOKEN	0x4F			/* Hosting method: issuing token, execution of transactions, it is recommended to use CMD2H_ISSUE_TKN_X command*/
											/* Enter: transaction attribute*/
											/*		 Low 4 digits are token, 4 digits are end -point numbers */
											/* Output interruption*/

#define	CMD0H_DISK_INIT		0x51			/* Hosting method/does not support SD card: initialized USB memory*/
											/* Output interruption*/

#define	CMD0H_DISK_RESET	0x52			/* Hosting/No SD card: Control transmission-reset USB memory*/
											/* Output interruption*/

#define	CMD0H_DISK_SIZE		0x53			/* Host/No SD card: Get the capacity of USB memory*/
											/* Output interruption*/

#define	CMD0H_DISK_INQUIRY	0x58			/* Host/No SD card: query USB memory characteristics*/
											/* Output interruption*/

#define	CMD0H_DISK_READY	0x59			/* Hosting method/does not support SD cards: check USB memory is ready*/
											/* Output interruption*/

#define	CMD0H_DISK_R_SENSE	0x5A			/* Hosting/No SD card: Check USB memory error*/
											/* Output interruption*/

#define	CMD0H_RD_DISK_SEC	0x5B			/* Host file mode: Read a sector from a disk to the internal buffer*/
											/* Output interruption*/

#define	CMD0H_WR_DISK_SEC	0x5C			/* Console file mode: write data from a sector in the internal buffer to the disk*/
											/* Output interruption*/

#define	CMD0H_DISK_MAX_LUN	0x5D			/* Hosting: Control transmission-Get the maximum logic unit number of USB memory*/
											/* Output interruption*/

/* **********************************************************************************************************************/
/* The following definition is only for the command name format in the Include file compatible with CH375*/


#define	_NO_CH375_COMPATIBLE_				/*Disable CH375 Compatibility*/
#ifndef	_NO_CH375_COMPATIBLE_
#define	CMD_GET_IC_VER		CMD01_GET_IC_VER
#define	CMD_SET_BAUDRATE	CMD21_SET_BAUDRATE
#define	CMD_ENTER_SLEEP		CMD00_ENTER_SLEEP
#define	CMD_RESET_ALL		CMD00_RESET_ALL
#define	CMD_CHECK_EXIST		CMD11_CHECK_EXIST
#define	CMD_CHK_SUSPEND		CMD20_CHK_SUSPEND
#define	CMD_SET_SDO_INT		CMD20_SET_SDO_INT
#define	CMD_GET_FILE_SIZE	CMD14_GET_FILE_SIZE
#define	CMD_SET_FILE_SIZE	CMD50_SET_FILE_SIZE
#define	CMD_SET_USB_MODE	CMD11_SET_USB_MODE
#define	CMD_GET_STATUS		CMD01_GET_STATUS
#define	CMD_UNLOCK_USB		CMD00_UNLOCK_USB
#define	CMD_RD_USB_DATA0	CMD01_RD_USB_DATA0
#define	CMD_RD_USB_DATA		CMD01_RD_USB_DATA
#define	CMD_WR_USB_DATA7	CMD10_WR_USB_DATA7
#define	CMD_WR_HOST_DATA	CMD10_WR_HOST_DATA
#define	CMD_WR_REQ_DATA		CMD01_WR_REQ_DATA
#define	CMD_WR_OFS_DATA		CMD20_WR_OFS_DATA
#define	CMD_SET_FILE_NAME	CMD10_SET_FILE_NAME
#define	CMD_DISK_CONNECT	CMD0H_DISK_CONNECT
#define	CMD_DISK_MOUNT		CMD0H_DISK_MOUNT
#define	CMD_FILE_OPEN		CMD0H_FILE_OPEN
#define	CMD_FILE_ENUM_GO	CMD0H_FILE_ENUM_GO
#define	CMD_FILE_CREATE		CMD0H_FILE_CREATE
#define	CMD_FILE_ERASE		CMD0H_FILE_ERASE
#define	CMD_FILE_CLOSE		CMD1H_FILE_CLOSE
#define	CMD_DIR_INFO_READ	CMD1H_DIR_INFO_READ
#define	CMD_DIR_INFO_SAVE	CMD0H_DIR_INFO_SAVE
#define	CMD_BYTE_LOCATE		CMD4H_BYTE_LOCATE
#define	CMD_BYTE_READ		CMD2H_BYTE_READ
#define	CMD_BYTE_RD_GO		CMD0H_BYTE_RD_GO
#define	CMD_BYTE_WRITE		CMD2H_BYTE_WRITE
#define	CMD_BYTE_WR_GO		CMD0H_BYTE_WR_GO
#define	CMD_DISK_CAPACITY	CMD0H_DISK_CAPACITY
#define	CMD_DISK_QUERY		CMD0H_DISK_QUERY
#define	CMD_DIR_CREATE		CMD0H_DIR_CREATE
#define	CMD_SEC_LOCATE		CMD4H_SEC_LOCATE
#define	CMD_SEC_READ		CMD1H_SEC_READ
#define	CMD_SEC_WRITE		CMD1H_SEC_WRITE
#define	CMD_DISK_BOC_CMD	CMD0H_DISK_BOC_CMD
#define	CMD_DISK_READ		CMD5H_DISK_READ
#define	CMD_DISK_RD_GO		CMD0H_DISK_RD_GO
#define	CMD_DISK_WRITE		CMD5H_DISK_WRITE
#define	CMD_DISK_WR_GO		CMD0H_DISK_WR_GO
#define	CMD_SET_USB_SPEED	CMD10_SET_USB_SPEED
#define	CMD_GET_DEV_RATE	CMD11_GET_DEV_RATE
#define	CMD_GET_TOGGLE		CMD11_GET_TOGGLE
#define	CMD_READ_VAR8		CMD11_READ_VAR8
#define	CMD_SET_RETRY		CMD20_SET_RETRY
#define	CMD_WRITE_VAR8		CMD20_WRITE_VAR8
#define	CMD_READ_VAR32		CMD14_READ_VAR32
#define	CMD_WRITE_VAR32		CMD50_WRITE_VAR32
#define	CMD_DELAY_100US		CMD01_DELAY_100US
#define	CMD_SET_USB_ID		CMD40_SET_USB_ID
#define	CMD_SET_USB_ADDR	CMD10_SET_USB_ADDR
#define	CMD_TEST_CONNECT	CMD01_TEST_CONNECT
#define	CMD_ABORT_NAK		CMD00_ABORT_NAK
#define	CMD_SET_ENDP2		CMD10_SET_ENDP2
#define	CMD_SET_ENDP3		CMD10_SET_ENDP3
#define	CMD_SET_ENDP4		CMD10_SET_ENDP4
#define	CMD_SET_ENDP5		CMD10_SET_ENDP5
#define	CMD_SET_ENDP6		CMD10_SET_ENDP6
#define	CMD_SET_ENDP7		CMD10_SET_ENDP7
#define	CMD_DIRTY_BUFFER	CMD00_DIRTY_BUFFER
#define	CMD_WR_USB_DATA3	CMD10_WR_USB_DATA3
#define	CMD_WR_USB_DATA5	CMD10_WR_USB_DATA5
#define	CMD_CLR_STALL		CMD1H_CLR_STALL
#define	CMD_SET_ADDRESS		CMD1H_SET_ADDRESS
#define	CMD_GET_DESCR		CMD1H_GET_DESCR
#define	CMD_SET_CONFIG		CMD1H_SET_CONFIG
#define	CMD_AUTO_SETUP		CMD0H_AUTO_SETUP
#define	CMD_ISSUE_TKN_X		CMD2H_ISSUE_TKN_X
#define	CMD_ISSUE_TOKEN		CMD1H_ISSUE_TOKEN
#define	CMD_DISK_INIT		CMD0H_DISK_INIT
#define	CMD_DISK_RESET		CMD0H_DISK_RESET
#define	CMD_DISK_SIZE		CMD0H_DISK_SIZE
#define	CMD_DISK_INQUIRY	CMD0H_DISK_INQUIRY
#define	CMD_DISK_READY		CMD0H_DISK_READY
#define	CMD_DISK_R_SENSE	CMD0H_DISK_R_SENSE
#define	CMD_RD_DISK_SEC		CMD0H_RD_DISK_SEC
#define	CMD_WR_DISK_SEC		CMD0H_WR_DISK_SEC
#define	CMD_DISK_MAX_LUN	CMD0H_DISK_MAX_LUN
#endif

/* **********************************************************************************************************************/
/* Putting, the position definition of the state port (read command port)*/
#ifndef	PARA_STATE_INTB
#define	PARA_STATE_INTB		0x80			/* And the position of the state of the mouth mode 7: interrupt logo, low and effective*/
#define	PARA_STATE_BUSY		0x10			/* And the position of the state port of the mouth 4: busy logo, high and effective*/
#endif

/* **********************************************************************************************************************/
/* Serial port method, guidance synchronization code before operation command*/
#ifndef	SER_CMD_TIMEOUT
#define	SER_CMD_TIMEOUT		32				/* The serial command timeout time, the unit is MS, the interval between the synchronous code and the synchronous code and the command code should be as short as possible.*/
#define	SER_SYNC_CODE1		0x57			/* The first serial synchronization code of the startup operation*/
#define	SER_SYNC_CODE2		0xAB			/* The second serial synchronization code of the startup operation*/
#endif

/* **********************************************************************************************************************/
/* Operating status*/

#ifndef	CMD_RET_SUCCESS
#define	CMD_RET_SUCCESS		0x51			/* The command operation is successful*/
#define	CMD_RET_ABORT		0x5F			/* The command operation failed*/
#endif

/* **********************************************************************************************************************/
/* USB interrupt status*/

#ifndef	USB_INT_EP0_SETUP

/* The following status code is interruption of special events. If the USB bus is enabled by CMD20_CHK_SUSPEND to hang a check, then the interrupt state of the USB bus hanging and sleep wake -up must be processed.*/
#define	USB_INT_USB_SUSPEND	0x05			/* USB bus hanging event*/
#define	USB_INT_WAKE_UP		0x06			/* Wake up from sleep*/

/* The following status code 0xh is used for USB device methods*/
/* You only need to process in the built -in firmware mode: usb_int_ep1_out, usb_int_ep1_in, USB_INT_EP2_OUT, USB_INT_EP2_IN*/
/* Bit 7-bit 4 is 0000*/
/* Bit 3-bit 2 indicates the current transaction, 00 = out, 10 = in, 11 = setup*/
/* Bit 1-bit 0 indicates the current endpoint, 00 = endpoint 0, 01 = endpoint 1, 10 = endpoint 2, 11 = USB bus reset*/
#define	USB_INT_EP0_SETUP	0x0C			/* Setup of USB endpoint 0*/
#define	USB_INT_EP0_OUT		0x00			/* USB endpoint 0 out*/
#define	USB_INT_EP0_IN		0x08			/* USB endpoint 0 in*/
#define	USB_INT_EP1_OUT		0x01			/* Out of USB endpoint 1*/
#define	USB_INT_EP1_IN		0x09			/* USB endpoint 1 in*/
#define	USB_INT_EP2_OUT		0x02			/* Out of USB endpoint 2*/
#define	USB_INT_EP2_IN		0x0A			/* USB endpoint 2 in*/
/* Usb_int_bus_reset 0x0000xx11b * / / * USB bus reset*/
#define	USB_INT_BUS_RESET1	0x03			/* USB bus reset*/
#define	USB_INT_BUS_RESET2	0x07			/* USB bus reset*/
#define	USB_INT_BUS_RESET3	0x0B			/* USB bus reset*/
#define	USB_INT_BUS_RESET4	0x0F			/* USB bus reset*/

#endif

/* The following status code 2xH-3XH is used in the communication failure code for the USB host mode*/
/* Bit 7-bit 6 to 00*/
/* Bit 5 is 1*/
/* Bit 4 indicates whether the currently received data packet synchronizes*/
/* Bit 3-bit 0 indicator causes the Answer of USB device when the communication fails: 0010 = ACK, 1010 = Nak, 1110 = Stall, 0011 = data0, 1011 = data1, xx00 = timeout timeout*/
/* Usb_int_ret_ack 0x001x0010B * / / * Error: For IN transaction, return ACK*/
/* Usb_int_ret_nak 0x001x1010B * / / * Error: Return to Nak*/


/* The following status code 1xh is used for the operating status code of the USB host method*/
#ifndef	USB_INT_SUCCESS
#define	USB_INT_SUCCESS		0x14			/* USB transaction or transmission operation successfully*/
#define	USB_INT_CONNECT		0x15			/* The USB device connection event may be detected, which may be a new connection or reoperation after disconnecting*/
#define	USB_INT_DISCONNECT	0x16			/* The USB device is disconnected from the USB device*/
#define	USB_INT_BUF_OVER	0x17			/* The data transmitted by USB is wrong or the data overflows too much.*/
#define	USB_INT_USB_READY	0x18			/* USB device has been initialized (the USB address has been assigned)*/
#define	USB_INT_DISK_READ	0x1D			/* USB memory request data read out*/
#define	USB_INT_DISK_WRITE	0x1E			/* USB memory request data writing*/
#define	USB_INT_DISK_ERR	0x1F			/* USB memory operation failed*/
#endif

/* The following status code is used for the file system error code in the host file mode*/
#ifndef	ERR_DISK_DISCON
#define	ERR_DISK_DISCON		0x82			/* The disk has not been connected, and the disk may have been disconnected*/
#define	ERR_LARGE_SECTOR	0x84			/* The sector of the disk is too large, only 512 bytes of each sector support*/
#define	ERR_TYPE_ERROR		0x92			/* The type of disk partition does not support it. It only supports FAT12/FAT16/BIGDOS/FAT32. It needs to be re -partitioned by the disk management tool*/
#define	ERR_BPB_ERROR		0xA1			/* The disk has not been formatted or the parameter error is required to be formatted by the default parameter by Windows*/
#define	ERR_DISK_FULL		0xB1			/* Disk files are too full, there are too few remaining spaces or no longer, you need to organize disk*/
#define	ERR_FDT_OVER		0xB2			/* There are too many files in the directory (folder), there is no idle directory item, the number of files in FAT12/FAT16 should be less*/
#define	ERR_FILE_CLOSE		0xB4			/* The file has been closed, if it is needed, the file should be reopened*/
#define	ERR_OPEN_DIR		0x41			/* Directory (folder) of the specified path is opened*/
#define	ERR_MISS_FILE		0x42			/* The files of the specified path are not found, it may be the file name error*/
#define	ERR_FOUND_NAME		0x43			/* Searched the matching file name, or required to open the directory (folder), but the actual results opened the file*/

/* The following file system error code is used for file system subroutines*/
#define	ERR_MISS_DIR		0xB3			/* A sub -directory (folder) that specifies the path is not found, it may be the name of the directory name*/
#define	ERR_LONG_BUF_OVER	0x48			/* Long file buffer overflow*/
#define	ERR_LONG_NAME_ERR	0x49			/* There is no corresponding long file name or a long file name error of the short file name*/
#define	ERR_NAME_EXIST		0x4A			/* The short file of the same name already exists, it is recommended to re -generate another short file name*/
#endif

/* **********************************************************************************************************************/
/* The following status code is used for disk and file status in the host file mode, var_disk_status*/
#ifndef	DEF_DISK_UNKNOWN
#define	DEF_DISK_UNKNOWN	0x00			/* Not initialized, unknown state*/
#define	DEF_DISK_DISCONN	0x01			/* The disk is not connected or has been disconnected*/
#define	DEF_DISK_CONNECT	0x02			/* The disk has been connected, but the disk has not been initialized or cannot be recognized*/
#define	DEF_DISK_MOUNTED	0x03			/* The disk has been initialized, but the file system or file system has not been analyzed.*/
#define	DEF_DISK_READY		0x10			/* The file system that has been analyzed has been analyzed and can support*/
#define	DEF_DISK_OPEN_ROOT	0x12			/* The root directory has been opened, and it must be closed after use. Pay attention to the fat12/fat16 root directory is the fixed length*/
#define	DEF_DISK_OPEN_DIR	0x13			/* Have opened the sub -directory (folder)*/
#define	DEF_DISK_OPEN_FILE	0x14			/* Already open the file*/
#endif

/* **********************************************************************************************************************/
/* Common definition of file system*/

#ifndef	DEF_SECTOR_SIZE
#define	DEF_SECTOR_SIZE		512				/* U disk or SD card default physical sector size*/
#endif

#ifndef	DEF_WILDCARD_CHAR
#define	DEF_WILDCARD_CHAR	0x2A			/* Path names of the path name '*'*/
#define	DEF_SEPAR_CHAR1		0x5C			/* The separator of the path name '\'*/
#define	DEF_SEPAR_CHAR2		0x2F			/* The separator of the path name '/'*/
#define	DEF_FILE_YEAR		2004			/* Default file date: 2004*/
#define	DEF_FILE_MONTH		1				/* Default file date: January*/
#define	DEF_FILE_DATE		1				/* Default file date: 1 day*/
#endif

#define ATTR_DIRECTORY		//By LYN (defined in usbStructs.h)
#ifndef	ATTR_DIRECTORY

/* FAT Data Area File Catalog Information*/
	typedef struct _FAT_DIR_INFO {
		uint8_t		DIR_Name[11];					/* 00h, file name, a total of 11 bytes, filling the blank in shortcomings*/
		uint8_t		DIR_Attr;						/* 0bh, file attribute, refer to the later description*/
		uint8_t		DIR_NTRes;						/* 0CH*/
		uint8_t		DIR_CrtTimeTenth;				/* 0dh, the time of the file creation, count in 0.1 seconds in unit count*/
		uint16_t	DIR_CrtTime;					/* 0eh, the time of file creation*/
		uint16_t	DIR_CrtDate;					/* 10h, the date of file creation*/
		uint16_t	DIR_LstAccDate;					/* 12h, the date of the latest access operation*/
		uint16_t	DIR_FstClusHI;					/* 14H*/
		uint16_t	DIR_WrtTime;					/* 16h, file modification time, refer to the previous macro_file_time*/
		uint16_t	DIR_WrtDate;					/* 18H, file modification date, refer to the previous macro_file_date*/
		uint16_t	DIR_FstClusLO;					/* 1AH*/
		uint32_t	DIR_FileSize;					/* 1ch, file length*/
	} FAT_DIR_INFO, *P_FAT_DIR_INFO;				/* 20H*/

/* File attribute*/
/* File attribute uINT8_T*/
/* Bit0 Bit1 Bit2 Bit3 Bit4 Bit5 Bit6 Bit7*/
/* Unseen only in hidden scrolls*/
/* Read the Tibetan Tibetan Bidding File*/
#define ATTR_READ_ONLY			0x01		/* The file is read only attributes*/
#define ATTR_HIDDEN				0x02		/* The file is the implicit attribute*/
#define ATTR_SYSTEM				0x04		/* The file is the system attribute*/
#define ATTR_VOLUME_ID			0x08		/* Label*/
#define ATTR_DIRECTORY			0x10		/* Subtraction (folder)*/
#define ATTR_ARCHIVE			0x20		/* The file is the archive attribute*/
#define ATTR_LONG_NAME			( ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID )	/* Long file name attribute*/
#define ATTR_LONG_NAME_MASK		( ATTR_LONG_NAME | ATTR_DIRECTORY | ATTR_ARCHIVE )

/* File Time UINT16_T*/
/* Time = (Hour<<11) + (Minute<<5) + (Second>>1)*/
#define MAKE_FILE_TIME( h, m, s )	( (h<<11) + (m<<5) + (s>>1) )	/* Generate a specified time -time file time data*/

/* File Date UINT16_T*/
/* Date = ((Year-1980)<<9) + (Month<<5) + Day*/
#define MAKE_FILE_DATE( y, m, d )	( ((y-1980)<<9) + (m<<5) + d )	/* Generate file date data for the specified year, month, month and day*/

#define LONE_NAME_MAX_CHAR		(255*2)		/* The most character number/byte number of the long file name*/
#define LONG_NAME_PER_DIR		(13*2)		/* The number/byte number of the long file name in each file directory information structure*/

#endif

/* **********************************************************************************************************************/
/* SCSI command and data input and output structure*/

#ifndef	SPC_CMD_INQUIRY

/* SCSI command code*/
#define SPC_CMD_INQUIRY			0x12
#define SPC_CMD_READ_CAPACITY	0x25
#define SPC_CMD_READ10			0x28
#define SPC_CMD_WRITE10			0x2A
#define SPC_CMD_TEST_READY		0x00
#define SPC_CMD_REQUEST_SENSE	0x03
#define SPC_CMD_MODESENSE6		0x1A
#define SPC_CMD_MODESENSE10		0x5A
#define SPC_CMD_START_STOP		0x1B

/* Command block of Bulkonly protocol*/
	typedef struct _BULK_ONLY_CBW {
		uint32_t	CBW_Sig;
		uint32_t	CBW_Tag;
		uint8_t		CBW_DataLen0;				/* 08h, input: data transmission length, the valid value for input data is 0 to 48, and the valid value for output data is 0 to 33*/
		uint8_t		CBW_DataLen1;
		uint16_t	CBW_DataLen2;
		uint8_t		CBW_Flag;					/* 0CH, input: transmission direction and other logos, the bit 7 is 1 input data, the digit is 0, the output data or no data*/
		uint8_t		CBW_LUN;
		uint8_t		CBW_CB_Len;					/* 0eh, input: the length of the command block, the valid value is 1 to 16*/
		uint8_t		CBW_CB_Buf[16];				/* 0fh, input: command block, this buffer is up to 16 bytes*/
	} BULK_ONLY_CBW, *P_BULK_ONLY_CBW;			/* The command block of the BULKONLY protocol, enter the CBW structure*/

/* Return data of inquiry command*/
	typedef struct _INQUIRY_DATA {
		uint8_t	DeviceType;						/* 00h, device type*/
		uint8_t	RemovableMedia;					/* 01H, bit 7 is 1 instructions for mobile storage*/
		uint8_t	Versions;						/* 02H, protocol version*/
		uint8_t	DataFormatAndEtc;				/* 03h, specify the return data format*/
		uint8_t	AdditionalLength;				/* 04h, the length of the follow -up data*/
		uint8_t	Reserved1;
		uint8_t	Reserved2;
		uint8_t	MiscFlag;						/* 07h, some control signs*/
		uint8_t	VendorIdStr[8];					/* 08h, manufacturer information*/
		uint8_t	ProductIdStr[16];				/* 10h, product information*/
		uint8_t	ProductRevStr[4];				/* 20H, product version*/
	} INQUIRY_DATA, *P_INQUIRY_DATA;			/* 24h*/

/* Request Sense command back data*/
	typedef struct _SENSE_DATA {
		uint8_t	ErrorCode;						/* 00h, error code and effective position*/
		uint8_t	SegmentNumber;
		uint8_t	SenseKeyAndEtc;					/* 02h, main key code*/
		uint8_t	Information0;
		uint8_t	Information1;
		uint8_t	Information2;
		uint8_t	Information3;
		uint8_t	AdditSenseLen;					/* 07h, the length of the follow -up data*/
		uint8_t	CmdSpecInfo[4];
		uint8_t	AdditSenseCode;					/* 0CH, additional key code*/
		uint8_t	AddSenCodeQual;					/* 0dh, detailed additional key code*/
		uint8_t	FieldReplaUnit;
		uint8_t	SenseKeySpec[3];
	} SENSE_DATA, *P_SENSE_DATA;				/* 12H*/

#endif

/* **********************************************************************************************************************/
/* Data input and output structure in the host file mode*/

#define MAX_FILE_NAME_LEN	//By LYN (defined in usbStructs.h)
#ifndef	MAX_FILE_NAME_LEN

#define MAX_FILE_NAME_LEN		(13+1)			/* The maximum length of the file name, the maximum length is 1 root directory symbol+8 main file name+1 decimal point+3 type name+ending character = 14*/

/* Input data and output data of the command*/
	typedef union _CH376_CMD_DATA {
		struct {
			uint8_t	mBuffer[MAX_FILE_NAME_LEN];
		} Default;

		INQUIRY_DATA	DiskMountInq;			/* Return: The return data of the Inquiry command*/
												/* CMD0H_DISK_MOUNT: initialize disk and test whether the disk is ready, when the first execution*/

		FAT_DIR_INFO	OpenDirInfo;			/* Return: Meeting file directory information*/
												/* CMD0H_FILE_OPEN: enumerated files and directory (folder)*/

		FAT_DIR_INFO	EnumDirInfo;			/* Return: Meeting file directory information*/
												/* Cmd0h_file_enum_go: Continue to enumerate files and directory (folder)*/

		struct {
			uint8_t		mUpdateFileSz;			/* Enter parameters: Whether the length of the file is allowed, 0 is prohibited from updating length*/
		} FileCLose;							/* Cmd1h_file_close: Close the currently opened file*/

		struct {
			uint8_t		mDirInfoIndex;			/* Enter parameters: Specify the index number of the directory information structure that needs to be read in the sector, and 0FFH is the file that has been opened.*/
		} DirInfoRead;							/* Cmd1H_DIR_INFO_READ: Read the directory information of the file*/

		union {
			uint32_t	mByteOffset;			/* Enter parameter: The number of bytes of the offset byte, the offset of the byte (total length of 32 digits, low bytes before)*/
			uint32_t	mSectorLba;				/* Return: The absolute linear sector code corresponding to the current file pointer, 0ffffffffH has reached the end of the file (total length of 32 bits, low bytes before)*/
		} ByteLocate;							/* Cmd4h_byte_locate: Move the current file pointer with bytes*/

		struct {
			uint16_t	mByteCount;				/* Enter parameters: request the number of bytes read (total length 16 bits, low bytes before)*/
		} ByteRead;								/* Cmd2h_byte_read: Reading data blocks from the current position in bytes*/

		struct {
			uint16_t	mByteCount;				/* Enter parameters: request the number of bytes written (total length 16 bits, low bytes before)*/
		} ByteWrite;							/* CMD2H_BYTE_WRITE: Write data blocks in the current position in the units of bytes*/

		union {
			uint32_t	mSectorOffset;			/* Enter parameter: The number of displacement sectors, the offset of the sector (total length of 32 bits, low bytes before)*/
			uint32_t	mSectorLba;				/* Return: The absolute linear sector code corresponding to the current file pointer, 0ffffffffH has reached the end of the file (total length of 32 bits, low bytes before)*/
		} SectorLocate;							/* CMD4H_SEC_LOCATE: Move the current file pointer with the sector*/

		struct {
			uint8_t		mSectorCount;			/* Enter parameter: request the number of sectors to read*/
												/* Return: the number of sectors allowed to be read*/
			uint8_t		mReserved1;
			uint8_t		mReserved2;
			uint8_t		mReserved3;
			uint32_t	mStartSector;			/* Return: The starting absolute linear sector code of the sector block that allows read (total length of 32 bits, low bytes before)*/
		} SectorRead;							/* CMD1H_SEC_READ: Read the data block from the current position in the sector*/

		struct {
			uint8_t		mSectorCount;			/* Enter parameter: request the number of sectors written in request*/
												/* Return: The number of sectors allowed to be written*/
			uint8_t		mReserved1;
			uint8_t		mReserved2;
			uint8_t		mReserved3;
			uint32_t	mStartSector;			/* Return: The starting absolute linear sector code for the writing of the scores (total length of 32 bits, low bytes before)*/
		} SectorWrite;							/* CMD1H_SEC_WRITE: Write data blocks in the current location in the sector*/

		struct {
			uint32_t	mDiskSizeSec;			/* Return: The total number of sectors of the entire physical disk (total length of 32 bits, low bytes in front)*/
		} DiskCapacity;							/* CMD0H_DISK_CAPACITY: Query disk physical capacity*/

		struct {
			uint32_t	mTotalSector;			/* Return: The total number of current logic disks (total length of 32 bits, low bytes in front)*/
			uint32_t	mFreeSector;			/* Return: The remaining sectors of the current logic disk (total length of 32 bits, low bytes before)*/
			uint8_t		mDiskFat;				/* Return: FAT type of current logic disk, 1-fat12,2-fat16,3-fat32*/
		} DiskQuery;							/* Cmd_diskQuery, query disk information*/

		BULK_ONLY_CBW	DiskBocCbw;				/* Enter parameter: CBW command structure*/
												/* CMD0H_DISK_BOC_CMD: The command to execute the Bulkonly transmission protocol for the USB memory*/

		struct {
			uint8_t		mMaxLogicUnit;			/* Return: the maximum logic unit number of USB memory*/
		} DiskMaxLun;							/* CMD0H_DISK_MAX_LUN: Control transmission-Get the maximum logic unit number of USB memory*/

		INQUIRY_DATA	DiskInitInq;			/* Return: The return data of the Inquiry command*/
												/* CMD0H_DISK_INIT: initialized USB memory*/

		INQUIRY_DATA	DiskInqData;			/* Return: The return data of the Inquiry command*/
												/* CMD0H_DISK_INQUIRY: Query USB memory characteristics*/

		SENSE_DATA		ReqSenseData;			/* Return: Return data of the Request Sense command*/
												/* CMD0H_DISK_R_SENSE: Check the USB memory error*/

		struct {
			uint32_t	mDiskSizeSec;			/* Return: The total number of sectors of the entire physical disk (total length of 32 bits, high bytes before)*/
		} DiskSize;								/* CMD0H_DISK_SIZE: Get the capacity of the USB memory*/

		struct {
			uint32_t	mStartSector;			/* Enter parameters: LBA sector address (total length of 32 bits, low bytes before)*/
			uint8_t		mSectorCount;			/* Enter parameter: request the number of sectors to read*/
		} DiskRead;								/* CMD5H_DISK_READ: Read data block from USB memory (in the sector)*/

		struct {
			uint32_t	mStartSector;			/* Enter parameters: LBA sector address (total length of 32 bits, low bytes before)*/
			uint8_t		mSectorCount;			/* Enter parameter: request the number of sectors written in request*/
		} DiskWrite;							/* CMD5H_DISK_WRITE: Write data blocks to USB memory (in the sector)*/
	} CH376_CMD_DATA, *P_CH376_CMD_DATA;

#endif

/* **********************************************************************************************************************/
/* The address of the file system variable in the host file mode*/

#ifndef	VAR_FILE_SIZE

	/* 8 -bit/single byte variable*/
#define	VAR_SYS_BASE_INFO	0x20			/* Basic information of the current system*/
											/* Bit 6 is used to indicate the subclass-code subclass-code of the USB storage device.*/
											/* Bit 5 is used to indicate the USB configuration status of the USB device method and the USB device connection state under the USB host mode*/
											/* Under the USB device method, the bit 5 is 1 for the USB configuration, and the bit 5 is not configured yet*/
											/* Under the USB host method, the bit 5 is 1 in USB ports that there are USB devices, and 5 -bit 0. The USB port has no USB device*/
											/* Bit 4 is used to indicate the lock state of the buffer area under the USB device.*/
											/* Other positions, retention, please do not modify*/
#define	VAR_RETRY_TIMES		0x25			/* The number of retries of USB transactions*/
											/* When the bit 7 is 0, you do not review it when you receive the NAK. The bit 7 is 1 and 6 is 0. When the NAK receives NAK, the unlimited review (you can use CMD_ABORT_NAK command to give up and try).Repeat up to 3 seconds when nak*/
											/* Bit 5 ~ Bit 0 is the number of retries after timeout*/
#define	VAR_FILE_BIT_FLAG	0x26			/* Picks in the host file mode*/
											/* Bit 1 and position 0, logic disk FAT file system logo, 00-FAT12, 01-FAT16, 10-FAT32, 11-illegal*/
											/* Position 2, whether the FAT table data in the current buffer area is modified, 0-not modified, 1-has been modified*/
											/* Position 3, the file length needs to be modified, the current file is added data, 0-without additional modification, 1-additional additional needs to be modified*/
											/* Other positions, retention, please do not modify*/
#define	VAR_DISK_STATUS		0x2B			/* Disk and file status in the host file mode*/
#define	VAR_SD_BIT_FLAG		0x30			/* SD card position logo in the host file mode*/
											/* Bit 0, SD card version, 0-only supports SD first version, 1-support SD second version*/
											/* Bit 1, automatic recognition, 0-SD card, 1-MMC card*/
											/* Bit 2, automatic recognition, 0-standard capacity SD card, 1-large capacity SD card (HC-SD)*/
											/* Bit 4, ACMD41 command timeout*/
											/* Bit 5, CMD1 command timeout*/
											/* Bit 6, CMD58 command timeout*/
											/* Other positions, retention, please do not modify*/
#define	VAR_UDISK_TOGGLE	0x31			/* Synchronous signs of Bulk-IN/BULK-OUT endpoints of USB storage devices*/
											/* Position 7, Bulk-in endpoint synchronous logo*/
											/* Synchronous signs of bull-in endpoint*/
											/* Bit 5 ~ bit 0, must be 0*/
#define	VAR_UDISK_LUN		0x34			/* Logic unit number of USB storage device*/
											/* Position 7 ~ bit 4, current logic unit number of USB storage device, CH376 initializes USB storage device, defaults to access 0#logic unit*/
											/* Bit 3 ~ bit 0, the maximum logical unit number of USB storage device, plus 1 is equal to the number of logic units*/
#define	VAR_SEC_PER_CLUS	0x38			/* The number of sectors of the logic disk*/
#define	VAR_FILE_DIR_INDEX	0x3B			/* The index number of the current file directory information in the sector*/
#define	VAR_CLUS_SEC_OFS	0x3C			/* The current file pointer shifts in the sector in the cluster, and 0xff is pointing to the end of the file, and the cluster ends*/

	/* 32 -bit/4 byte variable*/
#define	VAR_DISK_ROOT		0x44			/* For FAT16 disk to occupy the number of sectors at the root directory, the starting cluster number of the FAT32 disk is the root directory (total length of 32 bits, low bytes before)*/
#define	VAR_DSK_TOTAL_CLUS	0x48			/* The total number of logic disks (total length of 32 bits, low bytes before)*/
#define	VAR_DSK_START_LBA	0x4C			/* The start of the logic disk absolute sector code LBA (total length of 32 bits, low bytes before)*/
#define	VAR_DSK_DAT_START	0x50			/* The start of the data area of the logic disk (total length of 32 bits, low bytes before)*/
#define	VAR_LBA_BUFFER		0x54			/* LBA (total length of 32 bits, low bytes in front) corresponding to the data buffer of the disk data buffer zone)*/
#define	VAR_LBA_CURRENT		0x58			/* The current reading and writing disk starts with LBA address (total length of 32 bits, low bytes before)*/
#define	VAR_FAT_DIR_LBA		0x5C			/* The sector LBA address of the current file directory information (total length of 32 bits, low bytes before)*/
#define	VAR_START_CLUSTER	0x60			/* The starting cluster number (total length of 32 bits, low bytes before)*/
#define	VAR_CURRENT_CLUST	0x64			/* The current cluster number of the current file (total length of 32 bits, low bytes before)*/
#define	VAR_FILE_SIZE		0x68			/* The length of the current file (total length of 32 bits, low bytes in front)*/
#define	VAR_CURRENT_OFFSET	0x6C			/* The current file pointer, the bytes of the current reading and writing position (total length of 32 bits, low bytes before)*/

#endif

/* **********************************************************************************************************************/
/* Common USB definition*/

/* USB's package label pid, the host method may be used*/
#ifndef	DEF_USB_PID_SETUP
#define	DEF_USB_PID_NULL	0x00			/* Keep PID, unfarished definition*/
#define	DEF_USB_PID_SOF		0x05
#define	DEF_USB_PID_SETUP	0x0D
#define	DEF_USB_PID_IN		0x09
#define	DEF_USB_PID_OUT		0x01
#define	DEF_USB_PID_ACK		0x02
#define	DEF_USB_PID_NAK		0x0A
#define	DEF_USB_PID_STALL	0x0E
#define	DEF_USB_PID_DATA0	0x03
#define	DEF_USB_PID_DATA1	0x0B
#define	DEF_USB_PID_PRE		0x0C
#endif

/* USB request type, external firmware mode may be used*/
#ifndef	DEF_USB_REQ_TYPE
#define	DEF_USB_REQ_READ	0x80			/* Control reading operation*/
#define	DEF_USB_REQ_WRITE	0x00			/* Control writing operation*/
#define	DEF_USB_REQ_TYPE	0x60			/* Control request type*/
#define	DEF_USB_REQ_STAND	0x00			/* Standard request*/
#define	DEF_USB_REQ_CLASS	0x20			/* Equipment claim*/
#define	DEF_USB_REQ_VENDOR	0x40			/* Manufacturer request*/
#define	DEF_USB_REQ_RESERVE	0x60			/* Reserve request*/
#endif

/* USB standard device request, REQUESTTYPE 6 -bit 5 = 00 (Standard), external firmware mode may be used*/
#ifndef	DEF_USB_GET_DESCR
#define	DEF_USB_CLR_FEATURE	0x01
#define	DEF_USB_SET_FEATURE	0x03
#define	DEF_USB_GET_STATUS	0x00
#define	DEF_USB_SET_ADDRESS	0x05
#define	DEF_USB_GET_DESCR	0x06
#define	DEF_USB_SET_DESCR	0x07
#define	DEF_USB_GET_CONFIG	0x08
#define	DEF_USB_SET_CONFIG	0x09
#define	DEF_USB_GET_INTERF	0x0A
#define	DEF_USB_SET_INTERF	0x0B
#define	DEF_USB_SYNC_FRAME	0x0C
#endif

/* **********************************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
