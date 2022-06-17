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
// The following is a SPI method based on hardware links

//#define USE_SPI_LIB

#ifdef USE_SPI_LIB
#include <SPI.h>
static bool initialized = false;
#else
// SPI by byte transmission is completed
#define SPI_TRANSFER_DONE loop_until_bit_is_set(SPSR, SPIF)
#endif

// SPI receives a byte
uint8_t MCU_CH376::spiRecv()
{
#ifdef USE_SPI_LIB
  return SPI.transfer(0xFF);
#else
  if (_spiRate == SPI_SPEED_DISABLE)
    return 0xFF;
  SPDR = 0xFF;
  // SPDR = 0x00;
  SPI_TRANSFER_DONE;
  return SPDR;
#endif
}

// SPI send a byte
void MCU_CH376::spiSend(uint8_t b)
{
#ifdef USE_SPI_LIB
  SPI.transfer(b);
#else
  if (_spiRate == SPI_SPEED_DISABLE)
    return;
  SPDR = b;
  SPI_TRANSFER_DONE;
#endif
}

// SPI receiving data of specified length
void MCU_CH376::spiRead(uint8_t *buf, uint16_t nbyte)
{
#ifdef USE_SPI_LIB
#else
  if (nbyte-- == 0)
    return;
  SPDR = 0xFF;
  for (uint16_t i = 0; i < nbyte; i++)
  {
    SPI_TRANSFER_DONE;
    buf[i] = SPDR;
    SPDR = 0xFF;
  }
  SPI_TRANSFER_DONE;
  buf[nbyte] = SPDR;
#endif
}

// SPI sends data of specified length
void MCU_CH376::spiWrite(const uint8_t *buf, uint16_t nbyte)
{
#ifdef USE_SPI_LIB
#else
  if (nbyte-- == 0)
    return;
  for (uint16_t i = 0; i < nbyte; i++)
  {
    SPI_TRANSFER_DONE;
    SPDR = buf[i];
  }
  SPI_TRANSFER_DONE;
#endif
}

// SPI transmission rate initialization
void MCU_CH376::spiRateInit(uint8_t rateId)
{
#ifdef USE_SPI_LIB
  if (rateId == SPI_SPEED_DISABLE)
  {
    SPI.end();
    initialized = false;
    return;
  }
  SPI.setClockDivider(_spiRate);
  SPI.setDataMode(SPI_MODE3);
#else
  if (rateId == SPI_SPEED_DISABLE)
  {
    SPCR &= ~_BV(SPE); // Set SPI cannot be available
    return;
  }

  SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA); // Set SPI available, set the host mode, set the timing mode to MODE3 (that is, the high level is high level, the clock rising along the collection data)

  SPCR = (SPCR & ~0x03) | (rateId & 0x03);        // The clock rate selection is 1-0, so the mask is 0x03
  SPSR = (SPSR & ~0x01) | ((rateId >> 2) & 0x01); // The clock rate is twice the 0th place, so the mask is 0x01
#endif
}

// SPI disable film selection
void MCU_CH376::disableChipSelect()
{
#ifdef _FASTIO_ARDUINO_H
  WRITE(USB_SS, HIGH);
#else
  PORTB |= _BV(PB0);                              // Set SS to high level
#endif
}

// SPI allows film selection
void MCU_CH376::enableChipSelect()
{
  spiRateInit(_spiRate);
#ifdef _FASTIO_ARDUINO_H
  WRITE(USB_SS, LOW);
#else
  PORTB &= ~_BV(PB0);                             // Set SS to low level
#endif
}

// SPI initialization
void MCU_CH376::spi_init(uint8_t rate)
{
#ifdef USE_SPI_LIB
  if (!initialized)
  {
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
  DDRB = _BV(DDB0) | _BV(DDB1) | _BV(DDB2); // Set SS (DDB0)), SCK (DDB1), MOSI (DDB2) to the output mode, MISO (DDB3) as the input mode
#endif
#endif

  disableChipSelect();
  setSpiRate(rate);
}

// SPI termination
void MCU_CH376::spi_end()
{
  setState(DEF_DISK_UNKNOWN);
  disableChipSelect();
  setSpiRate(SPI_SPEED_DISABLE);
}

// Send a single command
void MCU_CH376::sendCmd(const uint8_t cmd)
{
  enableChipSelect();
  spiSend(cmd);
}

// Send the command of the band parameter, where the ARGN is the number of parameters
void MCU_CH376::sendCmd(const uint8_t cmd, uint8_t argn, ...)
{
  enableChipSelect();
  spiSend(cmd);

  va_list ap;
  va_start(ap, argn);
  while (argn--)
  {
    spiSend(va_arg(ap, int));
  }
  va_end(ap);
}

// Send data with specified length
void MCU_CH376::sendData(const uint8_t *data, const uint8_t length)
{
  int n = length, i = 0;
  while (n--)
  {
    spiSend(data[i++]);
  }
}

// Send a byte
void MCU_CH376::sendByte(const uint8_t b)
{
  spiSend(b);
}

// Receive a byte
uint8_t MCU_CH376::recvByte()
{
  return spiRecv();
}

// Receive four bytes
uint32_t MCU_CH376::recvWord()
{
  return spiRecv() | (uint16_t)spiRecv() << 8 | (uint32_t)spiRecv() << 16 | (uint32_t)spiRecv() << 24;
}

// Receive data of specified length
void MCU_CH376::recvData(uint8_t *data, const uint8_t length)
{
  int n = length;
  while (n--)
  {
    *data = recvByte();
    if (n)
      data++;
  }
}

// Command transmission ends
void MCU_CH376::endCmd()
{
  disableChipSelect();
}

// Test communication interface
bool MCU_CH376::check_exist()
{
  uint8_t testData = random(0xFF);
  sendCmd(CMD11_CHECK_EXIST, 1, testData);
  uint8_t recvData = recvByte();
  endCmd();

  if (testData + recvData == 0xFF)
    return true;
  return false;
}

// Whether to set the SPI SDO pin to interrupt method
void MCU_CH376::set_sdo_int(bool allow)
{
  sendCmd(CMD20_SET_SDO_INT, 2, 0x16, allow ? 0x90 : 0x10);
  endCmd();
}

// Set up working mode
bool MCU_CH376::set_usb_mode(uint8_t mode)
{
  sendCmd(CMD11_SET_USB_MODE, 1, mode);
  delayMicroseconds(20); // dealy 10us.
  uint8_t result = recvByte();
  endCmd();

  setState(result);

  if (result == CMD_RET_SUCCESS)
    return true;
  else if (result == CMD_RET_ABORT)
    return false;
  else
    return false;
}

// Query whether there is interruption of interruption
bool MCU_CH376::queryInterrupt()
{
#ifdef SPI_INT_PIN
  return READ(SPI_INT_PIN) ? false : true;
#else
  return (PORTB & _BV(PB3)) ? false : true;
#endif
}

// Get the interrupt state and cancel the interrupt request
uint8_t MCU_CH376::get_int_state()
{
  uint8_t intState;
  sendCmd(CMD01_GET_STATUS);
  intState = recvByte();
  endCmd();

  setState(intState);
  return intState;
}

#ifdef USB_READ
// Read the data block from the current buffer, return the length
uint8_t MCU_CH376::readBlock(uint8_t *buf)
{
  sendCmd(CMD01_RD_USB_DATA0);
  uint8_t n = recvByte();
  recvData(buf, n); // orSpiRead(buf,n);
  endCmd();
  return n;
}
#endif // USB_READ

#ifdef USB_WRITE
// Write data blocks to send buffer at the end point of the USB host
void MCU_CH376::writeBlock(uint8_t *buf, uint8_t len)
{
  sendCmd(CMD10_WR_HOST_DATA, 1, len);
  sendData(buf, len); // orSpiWrite(buf,Len);
  endCmd();
}

// Write the data block to the internal buffer specify the offset address
void MCU_CH376::writeOffsetBlock(uint8_t *buf, uint8_t ofs, uint8_t len)
{
  sendCmd(CMD20_WR_OFS_DATA, 2, ofs, len);
  sendData(buf, len); // orSpiWrite(buf,Len);
  endCmd();
}

// Write the data block of the request to the internal designated buffer, and return the length
uint8_t MCU_CH376::requestBlock(uint8_t *buf)
{
  sendCmd(CMD01_WR_REQ_DATA);
  uint8_t n = recvByte();
  sendData(buf, n); // orSpiWrite(buf,N);
  endCmd();
  return n;
}
#endif // USB_WRITE

#ifdef UDISK_DEBUG
uint8_t MCU_CH376::getVer()
{

  uint8_t ver;
  sendCmd(CMD01_GET_IC_VER);
  ver = recvByte() & ~(0x40);
  endCmd();
  return ver;
}

void MCU_CH376::sleep()
{
  sendCmd(CMD00_ENTER_SLEEP);
  endCmd();
}

void MCU_CH376::hreaset()
{
  sendCmd(CMD00_RESET_ALL);
  endCmd();
  delayMicroseconds(35000); // delay 35ms.
}
#endif // UDISK_DEBUG

void CH376_UDisk::init(const uint8_t mode, const uint8_t rstPin, const uint8_t busyPin, const uint8_t intPin)
{

#ifdef _FASTIO_ARDUINO_H
  // OUT_WRITE(USB_RST, LOW);
  // SET_INPUT(USB_BZ, LOW);
  SET_INPUT_PULLUP(USB_INT);
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

#ifdef CH376_SPI
  spi_init();
#elif defined CH376_UART
  // TODO CH376_UART
#elif defined CH376_PART
  // TODO CH376_PART
#endif

  setDeviceState(CH376_NO_INIT); // The setting module is not ready

  if (!check_exist())
  { // Can't detect chip
    setState(ERR_USB_UNKNOWN);
    return;
  }

  set_sdo_int(false); // Set the interrupt method to interrupt pins

  if (set_usb_mode(mode))
  {
    setState(USB_INT_SUCCESS);
  }
  else
  {
    setState(ERR_USB_MODE); // Model setting errors
    return;
  }

#ifdef SPI_INT_PIN
  if (!READ(SPI_INT_PIN))
  {                  // Detect the interrupt signal
    get_int_state(); // Cancel the interrupt signal
  }

  if (!READ(SPI_INT_PIN))
  { // The interrupt signal still exists (continuous low level)
    // SERIAL_ECHOLN("A");
    return;
  }
  else
  {
    sendCmd(CMD0H_DISK_CONNECT); // Send interruption requirements instructions (detection link instructions), detect the effectiveness of the interrupt needle foot
    endCmd();
    if (!READ(SPI_INT_PIN))
    { // The interrupt signal exists, indicating that the response is valid
      get_int_state();
      setDeviceState(CH376_INIT); // Module
    }
    else
    { // No interrupt signal (continuous high level) is not detected (continuous high level)
      // SERIAL_ECHOLN("B");
    }
  }
#endif
}

// Is CH376 initialization?
bool CH376_UDisk::IsReady()
{
  return !isDeviceState(CH376_NO_INIT);
}

// Detect whether Udisk is connected
bool CH376_UDisk::IsDiskConnect()
{
  if (queryInterrupt())
    get_int_state();

  sendCmd(CMD0H_DISK_CONNECT);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// Initialization Udisk
bool CH376_UDisk::diskMount()
{
  sendCmd(CMD0H_DISK_MOUNT);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// Udisk interrupt processing handle
bool CH376_UDisk::InterruptHandle()
{
  if (queryInterrupt())
  {                                     // Test interrupt occur
    setTimeout(USB_INT_CANCEL_TIMEOUT); // Set up interrupt processing timeout time

    while (queryInterrupt())
    {
      if (checkTimeout())
        return false; // 中断处理超时

      uint8_t intState = get_int_state(); // Get interrupt and cancel the interrupt state

      if (intState == USB_INT_CONNECT)
      {                                   // Detect udisk insertion
        setTimeout(USB_MODE_SET_TIMEOUT); // Set the mode processing timeout
        while (!set_usb_mode(USB_HOST_READY_RESET))
        { // Set to disk reset mode
          if (checkTimeout())
          {
            return false;
          }
        }

        setTimeout(USB_MODE_SET_TIMEOUT); // Set the mode processing timeout
        while (!set_usb_mode(USB_HOST_READY_AUTO_SOF))
        { // Set to disk automatic SOF mode
          if (checkTimeout())
          {
            return false;
          }
        }

        if (queryInterrupt())
        {
          setTimeout(USB_INT_CANCEL_TIMEOUT); // Set up interrupt processing timeout time

          // After the U disk is inserted, other interruptions ignore it
          //(Switching from No_SOF mode to auto_sof will re -generate USB_INT_CONNECT interrupt)
          while (queryInterrupt())
          {
            if (checkTimeout())
            {
              return false;
            }
            get_int_state();
          }
          setDeviceState(UDISK_INSERT);
        }
      }
      else if (intState == USB_INT_DISCONNECT)
      {                                   // Detect Udisk pulled out
        setTimeout(USB_MODE_SET_TIMEOUT); // Set the mode processing timeout
        while (!set_usb_mode(USB_HOST_READY_NO_SOF))
        { // Set to disk off SOF mode
          if (checkTimeout())
          {
            return false;
          }
        }
        setDeviceState(UDISK_REMOVE);

        if (queryInterrupt())
        {
          setTimeout(USB_INT_CANCEL_TIMEOUT); // Set up interrupt processing timeout time
        }
      }
      else
      {
        // TODO Other interruptions ??
        // SERIAL_ECHOLN((int)intState);
      }
    }
  }
  return true;
}

#if defined(USB_READ) || defined(USB_WRITE)
#ifdef USB_READ
// Open the file or directory
bool CH376_UDisk::fileOpen(const char *name)
{
  for (char *c = (char *)name; *c; c++)
  {
    *c = toupper(*c);
  }
  setFileName(name);
  // if (name[0] == DEF_SEPAR_CHAR1 || name[0] == DEF_SEPAR_CHAR2) setRoot();
  sendCmd(CMD0H_FILE_OPEN);
  endCmd();

  uint8_t state = waitForInterrupt();

  return (state == ERR_OPEN_DIR || state == USB_INT_SUCCESS) ? true : false;
}

// Open multi -level files or directory
bool CH376_UDisk::fileOpenPath(const char *name, uint8_t stopPos)
{
  uint8_t start, end;
  start = 0;
  end = 1;
  bool isSuccess = false;
  while (true)
  {
    if (!name[start])
      return isSuccess;

    for (; name[end] != DEF_SEPAR_CHAR1 && name[end] != DEF_SEPAR_CHAR2 && name[end] != 0; end++)
      ;
    if (name[end])
      end++;
    else
      end = 0;

    isSuccess = fileOpen(&name[start]);
    if (end && end != stopPos)
    {
      if (getState() != ERR_OPEN_DIR)
      {
        if (getState() == USB_INT_SUCCESS)
          setState(ERR_FOUND_NAME);
        else if (getState() == ERR_MISS_FILE)
          setState(ERR_MISS_DIR);
        break;
      }
      start = end;
    }
    else
      return isSuccess;
  }
  return false;
}

// Get the offset of the last level of the path
uint8_t CH376_UDisk::getPathLastDir(const char *name)
{
  uint8_t *p = (uint8_t *)name;
  while (*p)
  {
    p++;
  }
  while (*p != DEF_SEPAR_CHAR1 && *p != DEF_SEPAR_CHAR2 && p != (uint8_t *)name)
  {
    p--;
  }
  if (p != (uint8_t *)name)
    p++;
  return (p - (uint8_t *)name);
}

// Enumeration file or directory
void CH376_UDisk::listFile(uint8_t *buf, uint16_t index)
{
  fileClose();
  setFileName("*");
  sendCmd(CMD0H_FILE_OPEN);
  endCmd();

  uint8_t state;
  uint16_t count = 0;
  while (true)
  {
    state = waitForInterrupt();
    if (state == USB_INT_DISK_READ)
    {
      readBlock(buf);
      if (count++ > index)
        break;
      sendCmd(CMD0H_FILE_ENUM_GO);
      endCmd();
    }
    else
      break;
  }
}

// Enumeration file or directory
void CH376_UDisk::listFile(uint8_t *buf, void (*callback)())
{
  fileClose();
  setFileName("*");
  sendCmd(CMD0H_FILE_OPEN);
  endCmd();

  uint8_t state;
  while (true)
  {
    state = waitForInterrupt();

    if (state == USB_INT_DISK_READ)
    {
      readBlock(buf);
      sendCmd(CMD0H_FILE_ENUM_GO);
      endCmd();
      callback();
    }
    else
      break;
  }
}

// Enumeration file or directory
void CH376_UDisk::listFile(uint8_t *buf, uint16_t (*callback)(), uint16_t index)
{
  fileClose();
  setFileName("*");
  sendCmd(CMD0H_FILE_OPEN);
  endCmd();

  uint8_t state;
  while (true)
  {
    state = waitForInterrupt();

    if (state == USB_INT_DISK_READ)
    {
      readBlock(buf);
      if (callback() == (index + 1))
        break;
      sendCmd(CMD0H_FILE_ENUM_GO);
      endCmd();
    }
    else
      break;
  }
}

// Get the current file size
uint32_t CH376_UDisk::get_file_size()
{
  uint32_t size;
  sendCmd(CMD14_GET_FILE_SIZE, 1, 0x68);
  size = recvWord();
  endCmd();
  return size;
}

// Read a byte
uint8_t CH376_UDisk::readByte()
{
  // while (true) {
  sendCmd(CMD2H_BYTE_READ, 2, 0x01, 0x00);
  endCmd();
  waitForInterrupt();
  if (getState() == USB_INT_DISK_READ)
  {
    uint8_t buf;
    if (readBlock(&buf) == 1)
    {
      sendCmd(CMD0H_BYTE_RD_GO);
      endCmd();
      waitForInterrupt();
      if (getState() == USB_INT_SUCCESS)
      {
        // SERIAL_ECHO(buf);
        return buf;
      }
    }
  }
  return 0xFF;
  //}
}

#endif // USB_READ

#ifdef USB_WRITE
// Create a file
bool CH376_UDisk::fileCreate(const char *name)
{
  setFileName(name);
  sendCmd(CMD0H_FILE_CREATE);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// Create multi -level files
bool CH376_UDisk::fileCreatePath(const char *name)
{
  uint8_t pos = getPathLastDir(name);

  if (pos && !fileOpenPath(name, pos))
    return false;
  return fileCreate(&name[pos]);
}

// Create a directory
bool CH376_UDisk::dirCreate(const char *name)
{
  setFileName(name);
  sendCmd(CMD0H_DIR_CREATE);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// Create a multi -level directory
bool CH376_UDisk::dirCreatePath(const char *name)
{
  return false;
}

// Delete files or directory
bool CH376_UDisk::fileErase(const char *name)
{
  fileClose();
  setFileName(name);
  sendCmd(CMD0H_FILE_ERASE);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// Set the current file size
void CH376_UDisk::set_file_size(uint32_t size)
{
  sendCmd(CMD50_SET_FILE_SIZE, 5, 0x68, (uint8_t)size & 0xFF, (uint8_t)(size >> 8) & 0xFF, (uint8_t)(size >> 16) & 0xFF, (uint8_t)(size >> 24) & 0xFF);
  endCmd();
}
#endif // USB_WRITE

// Turn off the currently opened file or directory
bool CH376_UDisk::fileClose(bool updateSize)
{
  sendCmd(CMD1H_FILE_CLOSE, 1, (uint8_t)updateSize);
  endCmd();
  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// Read the current directory information
bool CH376_UDisk::dirInfoRead()
{
  sendCmd(CMD1H_DIR_INFO_READ, 1, 0xFF);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// Read the directory information of the specified index number in the sector
bool CH376_UDisk::dirInfoRead(uint8_t index)
{
  sendCmd(CMD1H_DIR_INFO_READ, 1, index);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// Set the current file pointer offset
bool CH376_UDisk::setOffset(uint32_t index)
{
  sendCmd(CMD4H_BYTE_LOCATE, 4, (uint8_t)index & 0xFF, (uint8_t)(index >> 8) & 0xFF, (uint8_t)(index >> 16) & 0xFF, (uint8_t)(index >> 24) & 0xFF);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// Get the current file pointer offset
uint32_t CH376_UDisk::getOffset()
{
  return readVar32(VAR_CURRENT_OFFSET);
}

// Set the sector offset
bool CH376_UDisk::setSecOffset(uint32_t index)
{
  sendCmd(CMD4H_SEC_LOCATE, 4, (uint8_t)index & 0xFF, (uint8_t)(index >> 8) & 0xFF, (uint8_t)(index >> 16) & 0xFF, (uint8_t)(index >> 24) & 0xFF);
  endCmd();

  return waitForInterrupt() == USB_INT_SUCCESS ? true : false;
}

// Set directory information LBA
void CH376_UDisk::setDirInfoLBA(uint32_t lba)
{
  writeVar32(VAR_FAT_DIR_LBA, lba);
}

// Get the directory information LBA
uint32_t CH376_UDisk::getDirInfoLBA()
{
  return readVar32(VAR_FAT_DIR_LBA);
}

// Get the current pointer offset
uint32_t CH376_UDisk::getCurOffSet()
{
  return readVar32(VAR_CURRENT_OFFSET);
}

// Get the root cluster number
uint32_t CH376_UDisk::getRootClus()
{
  return readVar32(VAR_DISK_ROOT);
}

// Set the start cluster number
void CH376_UDisk::setStartClus(uint32_t cluster)
{
  writeVar32(VAR_START_CLUSTER, cluster);
}

// Get the start cluster number
uint32_t CH376_UDisk::getStartClus()
{
  return readVar32(VAR_START_CLUSTER);
}

// Get the number of sectors per sector
uint8_t CH376_UDisk::getSecPerClus()
{
  return readVar8(VAR_SEC_PER_CLUS);
}

// Get the directory information index
uint8_t CH376_UDisk::getfileDirIndex()
{
  return readVar8(VAR_FILE_DIR_INDEX);
}

#endif // USB_READ || USB_WRITE

// Set the action timeout time
void CH376_UDisk::setTimeout(uint32_t waitTime)
{
#if ENABLED(USE_WATCHDOG)
  watchdog_reset(); // The communication time with the U disk may be longer, resulting in the slow cycle of LOOP, which causes the door dog to start; therefore, the door of the door should be reset at this time.
#endif
  TimeoutTime = millis() + waitTime;
}

// Detect whether the action timeout
bool CH376_UDisk::checkTimeout()
{
  if (PENDING(TimeoutTime, millis()))
  {
    setDeviceState(CH376_NO_INIT); // Module is not ready
    spi_end();                     // Close the underlying communication and save MCU resources
    ;                              // TODO may be crashed by CH376, and it requires hardware reset
    return true;
  }
  else
  {
    return false;
  }
}

// Waiting to return to interrupt
uint8_t CH376_UDisk::waitForInterrupt()
{
  setTimeout(USB_INT_WAIT_TIMEOUT); // Settings waiting for interrupt timeout
  while (!queryInterrupt())
  {
    if (checkTimeout())
    { // Waiting for interruption
      return getState();
    }
  }
  return get_int_state();
}

// Set to the roots
void CH376_UDisk::setRoot()
{
  writeVar32(VAR_START_CLUSTER, 0);
}

// Set to the current directory
void CH376_UDisk::setToCurDir()
{
  //	uint8_t sec_per_clus = readVar8(VAR_SEC_PER_CLUS);
  //	uint32_t dsk_start_lba = readVar32(VAR_DSK_START_LBA);
  //	uint32_t lba_current = readVar32(VAR_LBA_CURRENT);
  //
  //	uint32_t fat_dir_lba = readVar32(VAR_FAT_DIR_LBA);
}

// Set the file name to be operated
void CH376_UDisk::setFileName(const char *name)
{
  uint8_t *c = (uint8_t *)name;
  sendCmd(CMD10_SET_FILE_NAME);
  sendByte(*c);
  while (*c++)
  {
    if (*c == DEF_SEPAR_CHAR1 || *c == DEF_SEPAR_CHAR2)
      sendByte(0);
    else
      sendByte(*c);
  }
  endCmd();
}

void CH376_UDisk::writeVar8(uint8_t var, uint8_t data)
{
  sendCmd(CMD20_WRITE_VAR8, 2, var, data);
  endCmd();
}

uint8_t CH376_UDisk::readVar8(uint8_t var)
{
  sendCmd(CMD11_READ_VAR8, 1, var);
  uint8_t data = recvByte();
  endCmd();
  return data;
}

void CH376_UDisk::writeVar32(uint8_t var, uint32_t data)
{
  sendCmd(CMD50_WRITE_VAR32, 5, var, (uint8_t)data & 0xFF, (uint8_t)(data >> 8) & 0xFF, (uint8_t)(data >> 16) & 0xFF, (uint8_t)(data >> 24) & 0xFF);
  endCmd();
}

uint32_t CH376_UDisk::readVar32(uint8_t var)
{
  sendCmd(CMD14_READ_VAR32, 1, var);
  uint32_t data = recvWord();
  endCmd();
  return data;
}

#elif defined CH376_UART
// TODO CH376_UART
#elif defined CH376_PART
// TODO CH376_PART
#endif

#endif // UDISKSUPPORT
