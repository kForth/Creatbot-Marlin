/**
* UDisk_SD_impl_CH376.h - 基于CH376的USB实现
* Copyright (c) 2016 CreatBot. All Right Reserved.
* Developer Lyn Lee, 18 March 2016.
*/

#include "MarlinConfig.h"
#ifdef UDISKSUPPORT

#include "UDisk_SD_Lib_CH376.h"

#if ENABLED(USE_WATCHDOG)
  #include "watchdog.h"
#endif

#ifndef __UDISK_SD_IMPL_CH376_H__
#define __UDISK_SD_IMPL_CH376_H__

#define CH376_SPI				//定义CH376连接方式是SPI通讯
//#define CH376_UART			//定义CH376连接方式是异步串口通讯
//#define CH376_PART			//定义CH376连接方式是并行通讯

#if (defined(CH376_SPI)&&defined(CH376_UART)) || (defined(CH376_SPI)&&defined(CH376_PART)) || (defined(CH376_PART)&&defined(CH376_UART)) //检查ch376的连接方式是否冲突
#error "You can't set two connect mode for CH376. "
#endif


/* USB 工作模式 */
#define USB_DEV_NO_READY					0x00
#define USB_DEV_READY_EXT_FW			0x01
#define USB_DEV_READY_SELF_FW			0x02
#define SD_HOST										0x03
#define USB_HOST_NO_READY					0x04
#define USB_HOST_READY_NO_SOF			0x05
#define USB_HOST_READY_AUTO_SOF		0x06
#define USB_HOST_READY_RESET			0x07



#define USB_READ						//Allow USB reading operation
#define USB_WRITE						//Allow USB writing operation

#define ERR_USB_UNKNOWN			0xFA	//未知连接错误
#define ERR_USB_MODE				0xFB	//初始化模式错误

#define USB_INT_WAIT_TIMEOUT			2000		//(ms)  获取中断超时时间
#define USB_INT_CANCEL_TIMEOUT		3500		//(ms)	处理中断超时时间 (应该小于4S, 看门狗是4S，大于4S可能导致启动看门狗)
#define USB_MODE_SET_TIMEOUT			1000		//(ms)	模式设置超时时间

#ifdef CH376_SPI

/* SoftWareMode SPI Pin */
//#define	CH376_SPI_SCS			SS
//#define	CH376_SPI_SDI			MOSI
//#define	CH376_SPI_SDO			MISO
//#define	CH376_SPI_SCK			SCK

/* SPI 速率 */
#define SPI_SPEED_DIV4			0x00		// F_CPU/4
#define SPI_SPEED_DIV16			0x01		// F_CPU/16	
#define SPI_SPEED_DIV64			0x02		// F_CPU/64
#define SPI_SPEED_DIV128		0x03		// F_CPU/128
#define SPI_SPEED_DIV2			0x04		// F_CPU/2
#define SPI_SPEED_DIV8			0x05		// F_CPU/8
#define SPI_SPEED_DIV32			0x06		// F_CPU/32
#define SPI_SPEED_DISABLE		0xFF		// Disable

/* SPI 特殊针脚 */
#define SPI_RST_PIN				78
#define SPI_BUSY_PIN			5
#define SPI_INT_PIN				49


class MCU_CH376 {
public:
	MCU_CH376() { _state = DEF_DISK_UNKNOWN; _spiRate = SPI_SPEED_DISABLE;}
	void spi_init(uint8_t rate = SPI_SPEED_DIV8);
	void spi_end();
	void setState(uint8_t state) { _state = state; /* SERIAL_PROTOCOL_F(_state, HEX); SERIAL_EOL; */ }
	uint8_t getState() { return _state; }
	void setSpiRate(uint8_t spiRateId) { _spiRate = spiRateId; spiRateInit(_spiRate); }

	void sendCmd(const uint8_t cmd);
	void sendCmd(const uint8_t cmd, uint8_t argn, ...);
	void sendData(const uint8_t* data, const uint8_t length);
	void sendByte(const uint8_t b);
	uint8_t recvByte();
	uint32_t recvWord();
	void recvData(uint8_t* data, const uint8_t length);
	void endCmd();
	
	bool check_exist();																							//测试通讯接口
	void set_sdo_int(bool allow = true);														//是否设置SPI的SDO引脚为中断方式
	bool set_usb_mode(uint8_t mode);																//设置工作模式
	bool queryInterrupt();																					//查询是否有中断产生
	uint8_t get_int_state();																					//获取中断状态并取消中断请求

#ifdef USB_READ
	uint8_t readBlock(uint8_t* buf);																	//从当前缓冲区读取数据块,返回长度
#endif

#ifdef USB_WRITE
	uint8_t requestBlock(uint8_t* buf);															//向内部指定缓冲区写入请求的数据块,返回长度
	void writeBlock(uint8_t* buf, uint8_t len);											//向USB主机端点的发送缓冲区写入数据块
	void writeOffsetBlock(uint8_t* buf, uint8_t ofs, uint8_t len);	//向内部缓冲区指定偏移地址写入数据块
#endif

#ifdef UDISK_DEBUG
	uint8_t getVer();																									//获取CH376固件版本
	void sleep();																										//进入低功耗睡眠模式
	void hreaset();																									//执行硬件复位
#endif //UDISK_DEBUG


private:
	uint8_t _state;
	uint8_t _spiRate;

	uint8_t spiRecv();
	void spiSend(uint8_t b);
	void spiRead(uint8_t * buf, uint16_t nbyte);
	void spiWrite(const uint8_t * buf, uint16_t nbyte);
	void spiRateInit(uint8_t rateId);
	void disableChipSelect();
	void enableChipSelect();
};

#elif defined CH376_UART
	//TODO CH376_UART
#elif defined CH376_PART
	//TODO CH376_PART
#endif



enum hardwareState { CH376_NO_INIT, CH376_INIT, UDISK_INSERT, UDISK_READY, UDISK_REMOVE };

class CH376_UDisk :public MCU_CH376 {
public:
	CH376_UDisk() { deviceState = CH376_NO_INIT; TimeoutTime = 0; }
	void setDeviceState(hardwareState state) { deviceState = state; }
	bool isDeviceState(hardwareState state) { return (deviceState == state); }

	void init(const uint8_t mode, const uint8_t rstPin = SPI_RST_PIN, const uint8_t busyPin = SPI_BUSY_PIN, const uint8_t intPin = SPI_INT_PIN);
	bool IsReady();  //Is CH376 initialization?
	bool IsDiskConnect(); //Detect whether Udisk is connected
	bool diskMount();  //Initialization Udisk
	bool InterruptHandle();	 //Udisk interrupt processing handle


#if defined(USB_READ) || defined(USB_WRITE)

#ifdef USB_READ
	bool fileOpen(const char* name);  //Open the file or directory
	bool fileOpenPath(const char* name, uint8_t stopPos = 0xFF);  //Open multi -level files or directory
	uint8_t getPathLastDir(const char* name);  //Get the offset of the last level of the path
	void listFile(uint8_t *buf, uint16_t index);  //Enumeration file or directory
	void listFile(uint8_t *buf, void(*callback)());  //Enumeration file or directory
	void listFile(uint8_t * buf, uint16_t(*callback)(), uint16_t index);  //Enumeration file or directory

	uint32_t get_file_size();  //Get the current file size

	uint8_t readByte();  //Read a byte
#endif

#ifdef USB_WRITE
	bool fileCreate(const char* name);  //Create a file
	bool fileCreatePath(const char* name);  //Create multi -level files
	bool dirCreate(const char* name);  //Create a directory
	bool dirCreatePath(const char* name);  //Create a multi -level directory
	bool fileErase(const char* name);  //Delete files or directory

	void set_file_size(uint32_t size);  //Set the current file size
#endif

	bool fileClose(bool updateSize = false);  //Turn off the currently opened file or directory

	bool dirInfoRead();  //Read the current directory information
	bool dirInfoRead(uint8_t index);  //Read the directory information of the specified index number in the sector

	bool setOffset(uint32_t index);  //Set the current file pointer offset
	uint32_t getOffset();  //Get the current file pointer offset

	bool setSecOffset(uint32_t index);  //Set the sector offset

	void setDirInfoLBA(uint32_t lba);  //Set directory information LBA
	uint32_t getDirInfoLBA();  //Get the directory information LBA

	uint32_t getCurOffSet();  //Get the current pointer offset

	uint32_t getRootClus();  //Get the root cluster number
	void setStartClus(uint32_t cluster);  //Set the start cluster number
	uint32_t getStartClus();  //Get the start cluster number
	uint8_t getSecPerClus();  //Get the number of sectors per sector
	uint8_t getfileDirIndex();  //Get the directory information index

#endif

private:
	void setTimeout(uint32_t waitTime);  //Set the action timeout time
	bool checkTimeout();  //Detect whether the action timeout

	uint8_t waitForInterrupt();  //Waiting to return to interrupt
	void setRoot();  //Set to the roots
	void setToCurDir();  //Set to the current directory
	void setFileName(const char* name);  //Set the file name to be operated

	void writeVar8(uint8_t var, uint8_t data);  
	uint8_t readVar8(uint8_t var);  
	void writeVar32(uint8_t var, uint32_t data);  
	uint32_t readVar32(uint8_t var);  

	hardwareState deviceState;  
	uint32_t TimeoutTime;  
};

#endif // __UDISK_SD_IMPL_CH376_H__

#endif // UDISKSUPPORT
