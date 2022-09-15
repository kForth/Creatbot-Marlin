/**
 * UDisk_SD_impl_CH376.cpp - 基于CH376的USB实现
 * Copyright (c) 2016 CreatBot. All Right Reserved.
 * Developer Lyn Lee, 18 March 2016.
 */

#include "UDisk_SD_impl_CH376.h"
#ifdef UDISKSUPPORT

//引入不定参数类型
#include <stdarg.h>

#ifdef CH376_SPI
//以下是基于硬件链接的SPI方式通讯

#ifdef USB_INT
  #undef SPI_INT_PIN
  #define SPI_INT_PIN USB_INT
#endif
#ifdef USB_RST
  #undef SPI_RST_PIN
  #define SPI_RST_PIN USB_RST
#endif
#ifdef USB_BZ
  #undef SPI_BUSY_PIN
  #define SPI_BUSY_PIN USB_BZ
#endif

#if SPI_USE_BUSY_PIN
  #define BUSY_CHECK  do{}while(READ(SPI_BUSY_PIN));
#else
  #define SPI_BUSY_DELAY  2
  #define BUSY_CHECK  delayMicroseconds(SPI_BUSY_DELAY)
#endif


//#define USE_SPI_LIB

#ifdef USE_SPI_LIB
#include <SPI.h>
static bool initialized = false;
#else
// SPI一个字节传输完成
#define SPI_TRANSFER_DONE loop_until_bit_is_set(SPSR, SPIF)
#endif

// SPI 接收一个字节
uint8_t MCU_CH376::spiRecv(){
#ifdef USE_SPI_LIB
  return SPI.transfer(0xFF);
#else
  if(_spiRate == SPI_SPEED_DISABLE) return 0xFF;
  SPDR = 0xFF;
  //SPDR = 0x00;
  SPI_TRANSFER_DONE;
  return SPDR;
#endif
}

// SPI 发送一个字节
void MCU_CH376::spiSend(uint8_t b){
#ifdef USE_SPI_LIB
  SPI.transfer(b);
#else
  if(_spiRate == SPI_SPEED_DISABLE) return;
  SPDR = b;
  SPI_TRANSFER_DONE;
#endif
}

// SPI 接收指定长度的数据
void MCU_CH376::spiRead(uint8_t* buf, uint16_t nbyte){
#ifdef USE_SPI_LIB
#else
  if(nbyte-- == 0) return;
  SPDR = 0xFF;
  for(uint16_t i = 0;i < nbyte;i++){
    SPI_TRANSFER_DONE;
    buf[i] = SPDR;
    SPDR = 0xFF;
  }
  SPI_TRANSFER_DONE;
  buf[nbyte] = SPDR;
#endif
}

// SPI 发送指定长度的数据
void MCU_CH376::spiWrite(const uint8_t* buf, uint16_t nbyte){
#ifdef USE_SPI_LIB
#else
  if(nbyte-- == 0) return;
  for(uint16_t i = 0;i < nbyte;i++){
    SPI_TRANSFER_DONE;
    SPDR = buf[i];
  }
  SPI_TRANSFER_DONE;
#endif
}

// SPI 传输速率初始化
void MCU_CH376::spiRateInit(uint8_t rateId){
#ifdef USE_SPI_LIB
  if (rateId == SPI_SPEED_DISABLE){
    SPI.end();
    initialized = false;
    return;
  }
  SPI.setClockDivider(_spiRate);
  SPI.setDataMode(SPI_MODE3);
#else
  if(rateId == SPI_SPEED_DISABLE){
    SPCR &= ~_BV(SPE);                                  // 设置SPI不可用
    return;
  }

  SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA);  // 设置SPI可用，设置主机模式，设置时序模式为Mode3 (即空闲时为高电平，时钟上升沿采集数据)

  SPCR = (SPCR & ~0x03) | (rateId & 0x03);              // 时钟速率选择为1-0位，故掩码为0x03
  SPSR = (SPSR & ~0x01) | ((rateId >> 2) & 0x01);       // 时钟速率两倍为第0位，故掩码为0x01
#endif
}

// SPI 禁用片选
void MCU_CH376::disableChipSelect(){
#ifdef _FASTIO_ARDUINO_H
  WRITE(USB_SS, HIGH);
#else
  PORTB |= _BV(PB0);                //设置SS为高电平
#endif
}

// SPI 允许片选
void MCU_CH376::enableChipSelect(){
  spiRateInit(_spiRate);
#ifdef _FASTIO_ARDUINO_H
  WRITE(USB_SS, LOW);
#else
  PORTB &= ~_BV(PB0);               //设置SS为低电平
#endif
}

// SPI 初始化
void MCU_CH376::spi_init(uint8_t rate){
#ifdef USE_SPI_LIB
  if(!initialized){
    SPI.begin();
    initialized = true;
  }
#else
#ifdef _FASTIO_ARDUINO_H
  SET_OUTPUT(USB_SS);
  SET_OUTPUT(USB_SCK);
  SET_OUTPUT(USB_MOSI);
  SET_INPUT(USB_MISO);
#else
  DDRB = _BV(DDB0) | _BV(DDB1) | _BV(DDB2);  //设置SS(DDB0))，SCK(DDB1)，MOSI(DDB2)为输出模式,MISO(DDB3)为输入模式
#endif
#endif

  disableChipSelect();
  setSpiRate(rate);
}

// SPI 终止
void MCU_CH376::spi_end(){
  setState(DEF_DISK_UNKNOWN);
  disableChipSelect();
  setSpiRate(SPI_SPEED_DISABLE);
}

// 发送单个命令
void MCU_CH376::sendCmd(const uint8_t cmd){
  enableChipSelect();
  spiSend(cmd);
  BUSY_CHECK;
}

// 发送带参数的命令，其中argn为参数个数
void MCU_CH376::sendCmd(const uint8_t cmd, uint8_t argn, ...){
  enableChipSelect();
  spiSend(cmd);
  BUSY_CHECK;

  if(argn < 1)  return;

  va_list ap;
  va_start(ap, argn);

  argn--;
  spiSend(va_arg(ap, int));
  BUSY_CHECK;

  while(argn--){ spiSend(va_arg(ap, int)); }
  va_end(ap);
}

// 发送指定长度的数据
void MCU_CH376::sendData(const uint8_t * data, const uint8_t length){
  int n = length, i = 0;
  while(n--){
    spiSend(data[i++]);
  }
}

// 发送一个字节
void MCU_CH376::sendByte(const uint8_t b){
  spiSend(b);
}

// 接收一个字节
uint8_t MCU_CH376::recvByte(){
  uint8_t result = spiRecv();
  BUSY_CHECK;
  return result;
}

// 接收四个字节
uint32_t MCU_CH376::recvWord(){
  uint32_t result = spiRecv();
  BUSY_CHECK;
  result |= ( (uint16_t)spiRecv() << 8 | (uint32_t)spiRecv() << 16 | (uint32_t)spiRecv() << 24 );
  return result;
}

// 接收指定长度的数据
void MCU_CH376::recvData(uint8_t *data, const uint8_t length){
  int n = length;
  while(n--){
    *data = spiRecv();
    if(n) data++;
  }
}

// 命令传输结束
void MCU_CH376::endCmd(){
  disableChipSelect();
}

// 测试通讯接口
bool MCU_CH376::check_exist(){
  uint8_t testData = random(0xFF);
  sendCmd(CMD11_CHECK_EXIST, 1, testData);
  uint8_t result = recvByte();
  endCmd();

  if(testData + result == 0xFF) return true;
  return false;
}

// 是否设置SPI的SDO引脚为中断方式
void MCU_CH376::set_sdo_int(bool allow){
  sendCmd(CMD20_SET_SDO_INT, 2, 0x16, allow ? 0x90 : 0x10);
  endCmd();
}

// 设置工作模式
bool MCU_CH376::set_usb_mode(uint8_t mode){
  sendCmd(CMD11_SET_USB_MODE, 1, mode);
  delayMicroseconds(20);  // dealy 20us.
  uint8_t result = recvByte();
  endCmd();

  setState(result);

  if(result == CMD_RET_SUCCESS) return true;
  else if(result == CMD_RET_ABORT) return false;
  else return false;
}

//查询是否有中断产生
bool MCU_CH376::queryInterrupt(){
#if SPI_USE_INT_PIN
  return READ(SPI_INT_PIN) ? false : true;
#else
  return (PORTB & _BV(PB3)) ? false : true;   //SDO
#endif
}

// 获取中断状态并取消中断请求
uint8_t MCU_CH376::get_int_state(){
  uint8_t intState;
  sendCmd(CMD01_GET_STATUS);
  intState = recvByte();
  endCmd();

  setState(intState);
  return intState;
}

#ifdef USB_READ
// 从当前缓冲区读取数据块,返回长度
uint8_t MCU_CH376::readBlock(uint8_t* buf){
  sendCmd(CMD01_RD_USB_DATA0);
  uint8_t n = recvByte();
  recvData(buf, n);                   //or spiRead(buf,n);
  endCmd();
  return n;
}
#endif //USB_READ

#ifdef USB_WRITE
// 向USB主机端点的发送缓冲区写入数据块
void MCU_CH376::writeBlock(uint8_t * buf, uint8_t len){
  sendCmd(CMD10_WR_HOST_DATA, 1, len);
  sendData(buf, len);                 //or spiWrite(buf, len);
  endCmd();
}

// 向内部缓冲区指定偏移地址写入数据块
void MCU_CH376::writeOffsetBlock(uint8_t * buf, uint8_t ofs, uint8_t len){
  sendCmd(CMD20_WR_OFS_DATA, 2, ofs, len);
  sendData(buf, len);                 //or spiWrite(buf, len);
  endCmd();
}

// 向内部指定缓冲区写入请求的数据块,返回长度
uint8_t MCU_CH376::requestBlock(uint8_t * buf){
  sendCmd(CMD01_WR_REQ_DATA);
  uint8_t n = recvByte();
  sendData(buf, n);                   //or spiWrite(buf, n);
  endCmd();
  return n;
}
#endif // USB_WRITE

#ifdef UDISK_DEBUG
uint8_t MCU_CH376::getVer(){

  uint8_t ver;
  sendCmd(CMD01_GET_IC_VER);
  ver = recvByte() & ~(0x40);
  endCmd();
  return ver;
}

void MCU_CH376::sleep(){
  sendCmd(CMD00_ENTER_SLEEP);
  endCmd();
}

void MCU_CH376::hreaset(){
  sendCmd(CMD00_RESET_ALL);
  endCmd();
  delayMicroseconds(35000);  // delay 35ms.
}
#endif //UDISK_DEBUG

void CH376_UDisk::init(const uint8_t mode, const uint8_t rstPin, const uint8_t busyPin, const uint8_t intPin){

#ifdef _FASTIO_ARDUINO_H
  #if SPI_USE_RST_PIN
    OUT_WRITE(SPI_RST_PIN, LOW);
  #endif
  #if SPI_USE_BUSY_PIN
    SET_INPUT(SPI_BUSY_PIN);
  #endif
  #if SPI_USE_INT_PIN
    SET_INPUT_PULLUP(SPI_INT_PIN);
  #endif
    UNUSED(rstPin);
    UNUSED(busyPin);
    UNUSED(intPin);
#else
  pinMode(rstPin, OUTPUT);
  pinMode(busyPin, INPUT);
  pinMode(intPin, INPUT);

  digitalWrite(rstPin, LOW);
  digitalWrite(busyPin, LOW);
  digitalWrite(intPin, HIGH);
#endif

  spi_init();

  setDeviceState(CH376_NO_INIT);        // 设置模块未就绪

  if(!check_exist()){                   // 检测不到芯片
    setState(ERR_USB_UNKNOWN);
    return;
  }

#if SPI_USE_INT_PIN
  set_sdo_int(false);                   // 设置中断方式为中断引脚
#else
  set_sdo_int(true);                    // 设置中断方式为SDO引脚
#endif

  if(set_usb_mode(USB_HOST_READY_NO_SOF)){
    setState(USB_INT_SUCCESS);
  }else{
    setState(ERR_USB_MODE);             // 模式设置出错
    return;
  }

  if(queryInterrupt()){                 // 检测到中断信号
    get_int_state();                    // 取消中断信号
  }

  if(queryInterrupt()){                 // 中断信号仍然存在(持续低电平)
    return;
  }else{
    sendCmd(CMD0H_DISK_CONNECT);        // 发送中断需求指令(检测链接指令),检测中断针脚有效性
    endCmd();
    if(queryInterrupt()){               // 中断信号存在,表示响应有效
      get_int_state();
      if(set_usb_mode(mode)){
        setDeviceState(CH376_INIT);     // 模块就绪
      }
    }
  }
}

// CH376初始化是否完成
bool CH376_UDisk::IsReady(){
  return !isDeviceState(CH376_NO_INIT);
}

// 检测UDisk是否连接
bool CH376_UDisk::IsDiskConnect(){
  if(queryInterrupt()) get_int_state();

  sendCmd(CMD0H_DISK_CONNECT);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// 初始化UDisk
bool CH376_UDisk::diskMount(){
  sendCmd(CMD0H_DISK_MOUNT);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// UDisk中断处理句柄
bool CH376_UDisk::InterruptHandle(){
  if(queryInterrupt()){                    // 检测到中断发生
    setTimeout(USB_INT_CANCEL_TIMEOUT);    // 设置中断处理超时时间

    while(queryInterrupt()){
      if(checkTimeout()) return false;  // 中断处理超时

      uint8_t intState = get_int_state();               // 获取中断并取消中断状态

      if(intState == USB_INT_CONNECT){                  // 检测到UDisk插入
        setTimeout(USB_MODE_SET_TIMEOUT);               // 设置模式处理超时
        while(!set_usb_mode(USB_HOST_READY_RESET)){     // 设置为磁盘重置模式
          if(checkTimeout()){
            return false;
          }
        }

        setTimeout(USB_MODE_SET_TIMEOUT);               // 设置模式处理超时
        while(!set_usb_mode(USB_HOST_READY_AUTO_SOF)){  // 设置为磁盘自动SOF模式
          if(checkTimeout()){
            return false;
          }
        }

        if(queryInterrupt()){
          setTimeout(USB_INT_CANCEL_TIMEOUT);    // 设置中断处理超时时间

          //U盘插入之后,其他中断都无视
          //(从NO_SOF模式切换到AUTO_SOF 会重新产生 USB_INT_CONNECT 中断)
          while(queryInterrupt()){
            if(checkTimeout()){
              return false;
            }
            get_int_state();
          }
          setDeviceState(UDISK_INSERT);
        }

      }else if(intState == USB_INT_DISCONNECT){         // 检测到UDisk拔出
        setTimeout(USB_MODE_SET_TIMEOUT);               // 设置模式处理超时
        while(!set_usb_mode(USB_HOST_READY_NO_SOF)){    // 设置为磁盘关闭SOF模式
          if(checkTimeout()){
            return false;
          }
        }
        setDeviceState(UDISK_REMOVE);

        if(queryInterrupt()){
          setTimeout(USB_INT_CANCEL_TIMEOUT);    // 设置中断处理超时时间
        }

      }else{
        //TODO 其他中断??
        //SERIAL_ECHOLN((int)intState);
      }

    }
  }
  return true;
}

#if defined(USB_READ) || defined(USB_WRITE)
#ifdef USB_READ
// 打开文件或目录
bool CH376_UDisk::fileOpen(const char* name){
  for(char *c = (char*)name;*c;c++){ *c = toupper(*c); }
  setFileName(name);
  //if (name[0] == DEF_SEPAR_CHAR1 || name[0] == DEF_SEPAR_CHAR2) setRoot();
  sendCmd(CMD0H_FILE_OPEN);
  endCmd();

  uint8_t state = waitForInterrupt();

  return (state == ERR_OPEN_DIR || state == USB_INT_SUCCESS) ? true : false;
}

// 打开多级文件或目录
bool CH376_UDisk::fileOpenPath(const char * name, uint8_t stopPos){
  uint8_t start, end;
  start = 0; end = 1;
  bool isSuccess = false;
  while(true){
    if(!name[start]) return isSuccess;

    for(;name[end] != DEF_SEPAR_CHAR1 && name[end] != DEF_SEPAR_CHAR2 && name[end] != 0;end++);
    if(name[end]) end++;
    else end = 0;

    isSuccess = fileOpen(&name[start]);
    if(end && end != stopPos){
      if(getState() != ERR_OPEN_DIR){
        if(getState() == USB_INT_SUCCESS) setState(ERR_FOUND_NAME);
        else if(getState() == ERR_MISS_FILE) setState(ERR_MISS_DIR);
        break;
      }
      start = end;
    }else return isSuccess;
  }
  return false;
}

// 获取路径最后一级目录的偏移
uint8_t CH376_UDisk::getPathLastDir(const char * name){
  uint8_t *p = (uint8_t*)name;
  while(*p){ p++; }
  while(*p != DEF_SEPAR_CHAR1 && *p != DEF_SEPAR_CHAR2 && p != (uint8_t*)name){ p--; }
  if(p != (uint8_t*)name) p++;
  return (p - (uint8_t*)name);
}

// 枚举文件或目录
void CH376_UDisk::listFile(uint8_t* buf, uint16_t index){
  fileClose();
  setFileName("*");
  sendCmd(CMD0H_FILE_OPEN);
  endCmd();

  uint8_t state;
  uint16_t count = 0;
  while(true){
    state = waitForInterrupt();
    if(state == USB_INT_DISK_READ){
      readBlock(buf);
      if(count++ > index) break;
      sendCmd(CMD0H_FILE_ENUM_GO);
      endCmd();
    }else break;
  }
}

// 枚举文件或目录
void CH376_UDisk::listFile(uint8_t *buf, void (*callback)()){
  fileClose();
  setFileName("*");
  sendCmd(CMD0H_FILE_OPEN);
  endCmd();

  uint8_t state;
  while(true){
    state = waitForInterrupt();

    if(state == USB_INT_DISK_READ){
      readBlock(buf);
      sendCmd(CMD0H_FILE_ENUM_GO);
      endCmd();
      callback();
    }else break;
  }
}

// 枚举文件或目录
void CH376_UDisk::listFile(uint8_t *buf, uint16_t (*callback)(), uint16_t index){
  fileClose();
  setFileName("*");
  sendCmd(CMD0H_FILE_OPEN);
  endCmd();

  uint8_t state;
  while(true){
    state = waitForInterrupt();

    if(state == USB_INT_DISK_READ){
      readBlock(buf);
      if(callback() == (index + 1)) break;
      sendCmd(CMD0H_FILE_ENUM_GO);
      endCmd();
    }else break;
  }
}

// 获取当前文件大小
uint32_t CH376_UDisk::get_file_size(){
  uint32_t size;
  sendCmd(CMD14_GET_FILE_SIZE, 1, 0x68);
  size = recvWord();
  endCmd();
  return size;
}

// 读下一个字节
uint8_t CH376_UDisk::readByte(){
  //while (true) {
  sendCmd(CMD2H_BYTE_READ, 2, 0x01, 0x00);
  endCmd();
  waitForInterrupt();
  if(getState() == USB_INT_DISK_READ){
    uint8_t buf;
    if(readBlock(&buf) == 1){
      sendCmd(CMD0H_BYTE_RD_GO);
      endCmd();
      waitForInterrupt();
      if(getState() == USB_INT_SUCCESS){
        //SERIAL_ECHO(buf);
        return buf;
      }
    }
  }
  return 0xFF;
  //}
}

#endif //USB_READ

#ifdef USB_WRITE
// 创建文件
bool CH376_UDisk::fileCreate(const char * name){
  setFileName(name);
  sendCmd(CMD0H_FILE_CREATE);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// 创建多级文件
bool CH376_UDisk::fileCreatePath(const char * name){
  uint8_t pos = getPathLastDir(name);

  if (pos && !fileOpenPath(name, pos)) return false;
  return fileCreate(&name[pos]);
}

// 创建目录
bool CH376_UDisk::dirCreate(const char * name){
  setFileName(name);
  sendCmd(CMD0H_DIR_CREATE);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// 创建多级目录
bool CH376_UDisk::dirCreatePath(const char * name){
  return false;
}

// 删除文件或目录
bool CH376_UDisk::fileErase(const char * name){
  fileClose();
  setFileName(name);
  sendCmd(CMD0H_FILE_ERASE);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// 设置当前文件大小
void CH376_UDisk::set_file_size(uint32_t size){
  sendCmd(CMD50_SET_FILE_SIZE, 5, 0x68, (uint8_t)size & 0xFF, (uint8_t)(size >> 8) & 0xFF, (uint8_t)(size >> 16) & 0xFF, (uint8_t)(size >> 24) & 0xFF);
  endCmd();
}
#endif //USB_WRITE

// 关闭当前打开的文件或目录
bool CH376_UDisk::fileClose(bool updateSize){
  sendCmd(CMD1H_FILE_CLOSE, 1, (uint8_t)updateSize);
  endCmd();
  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// 读取当前目录信息
bool CH376_UDisk::dirInfoRead(){
  sendCmd(CMD1H_DIR_INFO_READ, 1, 0xFF);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// 读取扇区内指定索引号的目录信息
bool CH376_UDisk::dirInfoRead(uint8_t index){
  sendCmd(CMD1H_DIR_INFO_READ, 1, index);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// 设置当前文件指针偏移
bool CH376_UDisk::setOffset(uint32_t index){
  sendCmd(CMD4H_BYTE_LOCATE, 4, (uint8_t)index & 0xFF, (uint8_t)(index >> 8) & 0xFF, (uint8_t)(index >> 16) & 0xFF, (uint8_t)(index >> 24) & 0xFF);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// 获取当前文件指针偏移
uint32_t CH376_UDisk::getOffset(){
  return readVar32(VAR_CURRENT_OFFSET);
}

// 设置扇区偏移
bool CH376_UDisk::setSecOffset(uint32_t index){
  sendCmd(CMD4H_SEC_LOCATE, 4, (uint8_t)index & 0xFF, (uint8_t)(index >> 8) & 0xFF, (uint8_t)(index >> 16) & 0xFF, (uint8_t)(index >> 24) & 0xFF);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// 设置目录信息LBA
void CH376_UDisk::setDirInfoLBA(uint32_t lba){
  writeVar32(VAR_FAT_DIR_LBA, lba);
}

// 获取目录信息LBA
uint32_t CH376_UDisk::getDirInfoLBA(){
  return readVar32(VAR_FAT_DIR_LBA);
}

// 获取当前指针偏移
uint32_t CH376_UDisk::getCurOffSet(){
  return readVar32(VAR_CURRENT_OFFSET);
}

// 获取根簇号
uint32_t CH376_UDisk::getRootClus(){
  return readVar32(VAR_DISK_ROOT);
}

// 设置起始簇号
void CH376_UDisk::setStartClus(uint32_t cluster){
  writeVar32(VAR_START_CLUSTER, cluster);
}

// 获取起始簇号
uint32_t CH376_UDisk::getStartClus(){
  return readVar32(VAR_START_CLUSTER);
}

// 获取每簇扇区数
uint8_t CH376_UDisk::getSecPerClus(){
  return readVar8(VAR_SEC_PER_CLUS);
}

// 获取目录信息索引
uint8_t CH376_UDisk::getfileDirIndex(){
  return readVar8(VAR_FILE_DIR_INDEX);
}

#endif // USB_READ || USB_WRITE

// 设置动作超时时间
void CH376_UDisk::setTimeout(uint32_t waitTime){
#if ENABLED(USE_WATCHDOG)
  watchdog_reset();      //与U盘的通讯时间可能较长，导致主loop循环过慢，引起看门狗启动；故此时应该重置看门狗。
#endif
  TimeoutTime = millis() + waitTime;
}

//检测动作是否超时
bool CH376_UDisk::checkTimeout(){
  if(PENDING(TimeoutTime, millis())){
    setDeviceState(CH376_NO_INIT);  //模块未就绪
    spi_end();                      //关闭底层通讯，节省MCU资源
    ;                               //TODO 可能是CH376死机，需要硬件复位
    return true;
  }else{
    return false;
  }
}

// 等待返回中断
uint8_t CH376_UDisk::waitForInterrupt(){
  setTimeout(USB_INT_WAIT_TIMEOUT);    // 设置等待中断超时
  while(!queryInterrupt()){
    if(checkTimeout()){                // 等待中断超时
      return getState();
    }
  }
  return get_int_state();
}

// 设置到根目录
void CH376_UDisk::setRoot(){
  writeVar32(VAR_START_CLUSTER, 0);
}

// 设置到当前目录
void CH376_UDisk::setToCurDir(){
//  uint8_t sec_per_clus = readVar8(VAR_SEC_PER_CLUS);
//  uint32_t dsk_start_lba = readVar32(VAR_DSK_START_LBA);
//  uint32_t lba_current = readVar32(VAR_LBA_CURRENT);
//
//  uint32_t fat_dir_lba = readVar32(VAR_FAT_DIR_LBA);
}

// 设置要操作的文件名
void CH376_UDisk::setFileName(const char* name){
  uint8_t *c = (uint8_t*)name;
  sendCmd(CMD10_SET_FILE_NAME, 1, *c);
  while(*c++){
    if(*c == DEF_SEPAR_CHAR1 || *c == DEF_SEPAR_CHAR2)
      sendByte(0);
    else
      sendByte(*c);
  }
  endCmd();
}

void CH376_UDisk::writeVar8(uint8_t var, uint8_t data){
  sendCmd(CMD20_WRITE_VAR8, 2, var, data);
  endCmd();
}

uint8_t CH376_UDisk::readVar8(uint8_t var){
  sendCmd(CMD11_READ_VAR8, 1, var);
  uint8_t data = recvByte();
  endCmd();
  return data;
}

void CH376_UDisk::writeVar32(uint8_t var, uint32_t data){
  sendCmd(CMD50_WRITE_VAR32, 5, var, (uint8_t)data & 0xFF, (uint8_t)(data >> 8) & 0xFF, (uint8_t)(data >> 16) & 0xFF, (uint8_t)(data >> 24) & 0xFF);
  endCmd();
}

uint32_t CH376_UDisk::readVar32(uint8_t var){
  sendCmd(CMD14_READ_VAR32, 1, var);
  uint32_t data = recvWord();
  endCmd();
  return data;
}

#elif defined CH376_UART
//TODO CH376_UART
#elif defined CH376_PART
//TODO CH376_PART
#endif

#endif //UDISKSUPPORT
