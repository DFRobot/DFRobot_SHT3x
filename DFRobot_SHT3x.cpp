/*!
 * @file DFRobot_SHT3x.cpp
 * @brief Define the infrastructure and the implementation of the underlying method of the DFRobot_SHT3x class, 
 * 
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @License     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-08-20
 * @url https://github.com/DFRobot/DFRobot_SHT3x
 */

#include <DFRobot_SHT3x.h>
#include"math.h"
DFRobot_SHT3x::DFRobot_SHT3x(TwoWire *pWire, uint8_t address,uint8_t RST)
{
  _pWire = pWire;
  _address = address;
  _RST = RST;
  measurementMode = eOneShot;
  pinMode(_RST,OUTPUT);
  digitalWrite(_RST,HIGH);
}

int DFRobot_SHT3x::begin() 
{
  _pWire->begin();
  if(readSerialNumber() == 0){
    DBG("bus data access error");
    return ERR_DATA_BUS;
   }
  return ERR_OK;
}
uint32_t DFRobot_SHT3x::readSerialNumber()
{
  uint32_t result = 0 ;
  uint8_t serialNumber1[3];
  uint8_t serialNumber2[3];
  uint8_t rawData[6];
  writeCommand(SHT3X_CMD_READ_SERIAL_NUMBER,2);
  delay(1);
  readData(rawData,6);
  memcpy(serialNumber1,rawData,3);
  memcpy(serialNumber2,rawData+3,3);
  if((checkCrc(serialNumber1) == serialNumber1[2]) && (checkCrc(serialNumber2) == serialNumber2[2])){
    result = serialNumber1[0];
    result = (result << 8) | serialNumber1[1];
    result = (result << 8) | serialNumber2[0];
    result = (result << 8) | serialNumber2[1];
  }
  return result;
}

bool DFRobot_SHT3x::softReset()
{
  sStatusRegister_t registerRaw;
  writeCommand(SHT3X_CMD_SOFT_RESET,2);
  delay(1);
  registerRaw = readStatusRegister();
  if(registerRaw.commandStatus == 0)
    return true;
  else 
    return false;
}

bool DFRobot_SHT3x::pinReset()
{
  sStatusRegister_t registerRaw;
  clearStatusRegister();
  digitalWrite(_RST,LOW);
  delay(1);
  digitalWrite(_RST,HIGH);
  //After hardware reset, it takes some time to enter the idle state
  delay(1);
  registerRaw = readStatusRegister();
  if(registerRaw.systemResetDeteced == 1)
    return true;
  else 
    return false;
}

bool DFRobot_SHT3x::stopPeriodicMode()
{  
  sStatusRegister_t registerRaw;
  measurementMode = eOneShot;
  writeCommand(SHT3X_CMD_STOP_PERIODIC_ACQUISITION_MODE,2);
  delay(1);
  registerRaw = readStatusRegister();
  if(registerRaw.commandStatus == 0)
    return true;
  else 
    return false;
}

bool DFRobot_SHT3x::heaterEnable()
{
  sStatusRegister_t registerRaw;
  writeCommand(SHT3X_CMD_HEATER_ENABLE,2);
  delay(1);
  registerRaw = readStatusRegister();
  if(registerRaw.heaterStaus == 1)
    return true;
  else 
    return false;
}

bool DFRobot_SHT3x::heaterDisable()
{
  sStatusRegister_t registerRaw;
  writeCommand( SHT3X_CMD_HEATER_DISABLE,2);
  delay(1);
  registerRaw = readStatusRegister();
  if(registerRaw.heaterStaus == 0)
    return true;
  else 
    return false;
}

void DFRobot_SHT3x::clearStatusRegister(){
  writeCommand(SHT3X_CMD_CLEAR_STATUS_REG,2);
  delay(10);
}

bool DFRobot_SHT3x::readAlertState()
{
  sStatusRegister_t registerRaw;
  delay(1);
  registerRaw = readStatusRegister();
  if(registerRaw.humidityAlert == 1 || registerRaw.temperatureAlert == 1){
    return true;
  } else {
    return false;
  }
}
DFRobot_SHT3x::sRHAndTemp_t DFRobot_SHT3x::readTemperatureAndHumidity(eRepeatability_t repeatability)
{
  uint8_t rawData[6];
  uint8_t rawTemperature[3];
  uint8_t rawHumidity[3];
  tempRH.ERR = 0;
      switch(repeatability){
        case eRepeatability_High:writeCommand(SHT3X_CMD_GETDATA_POLLING_H,2);break;
        case eRepeatability_Medium:writeCommand(SHT3X_CMD_GETDATA_POLLING_M,2);break;
        case eRepeatability_Low:writeCommand(SHT3X_CMD_GETDATA_POLLING_L,2);break;
    }
  delay(15);
  readData(rawData,6);
  memcpy(rawTemperature,rawData,3);
  memcpy(rawHumidity,rawData+3,3);
  if((checkCrc(rawTemperature) != rawTemperature[2]) || (checkCrc(rawHumidity) != rawHumidity[2])){
    tempRH.ERR = -1;
    return tempRH;
  }
  tempRH.TemperatureC = convertTemperature(rawTemperature);
  tempRH.TemperatureF = 1.8*tempRH.TemperatureC+32;
  tempRH.Humidity = convertHumidity(rawHumidity);
  return tempRH;
}

float DFRobot_SHT3x::getTemperatureC(){
  if(measurementMode == eOneShot){
    readTemperatureAndHumidity(eRepeatability_High);
  } else {
    readTemperatureAndHumidity();
  }
  return tempRH.TemperatureC;
}

float DFRobot_SHT3x::getTemperatureF()
{
  if(measurementMode == eOneShot){
    readTemperatureAndHumidity(eRepeatability_High);
  } else {
    readTemperatureAndHumidity();
  }
  return tempRH.TemperatureF;
}

float DFRobot_SHT3x::getHumidityRH()
{
  if(measurementMode == eOneShot){
    readTemperatureAndHumidity(eRepeatability_High);
  } else {
    readTemperatureAndHumidity();
  }
  return tempRH.Humidity;
}
bool DFRobot_SHT3x::startPeriodicMode(eMeasureFrequency_t measureFreq,eRepeatability_t repeatability)
{
  const uint16_t cmd[5][3] ={{SHT3X_CMD_SETMODE_H_FREQUENCY_HALF_HZ,SHT3X_CMD_SETMODE_M_FREQUENCY_HALF_HZ,SHT3X_CMD_SETMODE_L_FREQUENCY_HALF_HZ}\
  ,{SHT3X_CMD_SETMODE_H_FREQUENCY_1_HZ,SHT3X_CMD_SETMODE_M_FREQUENCY_1_HZ,SHT3X_CMD_SETMODE_L_FREQUENCY_1_HZ}\
  ,{SHT3X_CMD_SETMODE_H_FREQUENCY_2_HZ,SHT3X_CMD_SETMODE_M_FREQUENCY_2_HZ,SHT3X_CMD_SETMODE_L_FREQUENCY_2_HZ}\
  ,{SHT3X_CMD_SETMODE_H_FREQUENCY_4_HZ,SHT3X_CMD_SETMODE_M_FREQUENCY_4_HZ,SHT3X_CMD_SETMODE_L_FREQUENCY_4_HZ}\
  ,{SHT3X_CMD_SETMODE_H_FREQUENCY_10_HZ,SHT3X_CMD_SETMODE_M_FREQUENCY_10_HZ,SHT3X_CMD_SETMODE_L_FREQUENCY_10_HZ}} ;
  sStatusRegister_t registerRaw;
  measurementMode = ePeriodic;
  writeCommand(cmd[measureFreq][repeatability],2);
  delay(1);
  registerRaw = readStatusRegister();
  if(registerRaw.commandStatus == 0)
    return true;
  else 
    return false;
}

DFRobot_SHT3x::sStatusRegister_t DFRobot_SHT3x::readStatusRegister(){
  uint8_t register1[3];
  uint16_t data;
  sStatusRegister_t registerRaw;
  uint8_t retry = 10;
  while(retry--){
    writeCommand(SHT3X_CMD_READ_STATUS_REG,2);
    delay(1);
    readData(register1,3);
    if(checkCrc(register1) == register1[2]){
      break;
     }
    }
  data = (register1[0]<<8) | register1[1];
  memcpy(&registerRaw,&data,2);
  return registerRaw;
}
uint8_t DFRobot_SHT3x::environmentState()
{ 
  sStatusRegister_t registerRaw;
  float rhHighSet ;
  float rhLowSet ;
  delay(1);
  registerRaw = readStatusRegister();
  sRHAndTemp_t data = readTemperatureAndHumidity();
  if(measureTemperatureLimitC()){
    tempHighSet = getTemperatureHighSetC();
    tempLowSet  = getTemperatureLowSetC();
  }
  if(measureHumidityLimitRH()){
     rhHighSet = getHumidityHighSetRH();
     rhLowSet  = getHumidityLowSetRH();
  }
  if(registerRaw.humidityAlert == 1 && registerRaw.temperatureAlert ==0){
    if(data.Humidity > rhHighSet)
      return 2;
    else if(data.Humidity < rhLowSet)
      return 1;
  }
  else if(registerRaw.humidityAlert == 0 && registerRaw.temperatureAlert ==1){
    if(data.TemperatureC > tempHighSet)
      return 20;
    else if(data.TemperatureC < tempLowSet)
      return 10;
  }
  else if(registerRaw.humidityAlert == 1 && registerRaw.temperatureAlert ==1){
    if(data.TemperatureC < tempLowSet && data.Humidity < rhLowSet)
      return 11;
    else if(data.TemperatureC > tempHighSet && data.Humidity > rhHighSet)
      return 22;
    else if(data.TemperatureC > tempHighSet && data.Humidity < rhLowSet)
      return 21;
    else if(data.TemperatureC < tempLowSet && data.Humidity > rhHighSet)
      return 12;
  }

  return 0 ;
  
}
DFRobot_SHT3x::sRHAndTemp_t DFRobot_SHT3x::readTemperatureAndHumidity()
{
  uint8_t rawData[6];
  uint8_t rawTemperature[3];
  uint8_t rawHumidity[3];
  tempRH.ERR = 0;
  writeCommand(SHT3X_CMD_GETDATA,2);
  readData(rawData,6);
  memcpy(rawTemperature,rawData,3);
  memcpy(rawHumidity,rawData+3,3);
  if((checkCrc(rawTemperature) != rawTemperature[2]) || (checkCrc(rawHumidity) != rawHumidity[2])){
    tempRH.ERR = -1; 
    return tempRH;
  }
  tempRH.TemperatureC = convertTemperature(rawTemperature);
  tempRH.Humidity = convertHumidity(rawHumidity);
  tempRH.TemperatureF = 1.8*tempRH.TemperatureC+32;
  return tempRH;
}

uint8_t  DFRobot_SHT3x::setTemperatureLimitC(float highset,float highclear, float lowset,float lowclear)
{
  uint16_t _highset ,_highclear,_lowclear,_lowset,limit[1];
  
  if(highset > highclear && highclear > lowclear && lowclear> lowset){
    _highset = convertRawTemperature(highset);
    if(readLimitData(SHT3X_CMD_READ_HIGH_ALERT_LIMIT_SET,limit) != 0) {
      return 1;
    }
    _highset = (_highset >> 7) |(limit[0] & 0xfe00);
    writeLimitData(SHT3X_CMD_WRITE_HIGH_ALERT_LIMIT_SET,_highset);
    _highclear = convertRawTemperature(highclear);
    if(readLimitData(SHT3X_CMD_READ_HIGH_ALERT_LIMIT_CLEAR,limit) != 0){
      return 1;
    }
    _highclear = (_highclear >> 7) |(limit[0] & 0xfe00);
    writeLimitData(SHT3X_CMD_WRITE_HIGH_ALERT_LIMIT_CLEAR,_highclear);
    _lowclear = convertRawTemperature(lowclear);
    if(readLimitData(SHT3X_CMD_READ_LOW_ALERT_LIMIT_CLEAR,limit) != 0){
      return 1;
    }
    _lowclear = (_lowclear >> 7) |(limit[0] & 0xfe00);
    writeLimitData(SHT3X_CMD_WRITE_LOW_ALERT_LIMIT_CLEAR,_lowclear);
    _lowset = convertRawTemperature(lowset);
    if(readLimitData(SHT3X_CMD_READ_LOW_ALERT_LIMIT_SET,limit) != 0){
      return 1;
    }
    _lowset = (_lowset >> 7) |(limit[0] & 0xfe00);
    writeLimitData(SHT3X_CMD_WRITE_LOW_ALERT_LIMIT_SET,_lowset);
  } else {
    return 1;
  }
   
  return 0;
}
uint8_t DFRobot_SHT3x::setTemperatureLimitF(float highset,float highclear,float lowset,float lowclear)
{
  float _highset ,_highclear,_lowclear,_lowset;
  _highset = (highset - 32.0) * 5.0 / 9.0;
  _highclear = (highclear - 32.0) * 5.0 / 9.0;
  _lowclear = (lowclear - 32.0) * 5.0 / 9.0;
  _lowset = (lowset - 32.0) * 5.0 / 9.0;
  if(setTemperatureLimitC(_highset,_highclear,_lowset,_lowclear) == 0){
    return 0;
  }
  return 1;
}
uint8_t DFRobot_SHT3x::setHumidityLimitRH(float highset,float highclear, float lowset,float lowclear)
{
  uint16_t _highset ,_highclear,_lowclear,_lowset,limit[1];
  if(highset > highclear && highclear > lowclear && lowclear> lowset){
    _highset = convertRawHumidity(highset);
    if(readLimitData(SHT3X_CMD_READ_HIGH_ALERT_LIMIT_SET,limit) != 0) {
      return 1;
    }
    _highset = (_highset & 0xFE00) |(limit[0] & 0x1FF);
    writeLimitData(SHT3X_CMD_WRITE_HIGH_ALERT_LIMIT_SET,_highset);
    
    _highclear = convertRawHumidity(highclear);
    if(readLimitData(SHT3X_CMD_READ_HIGH_ALERT_LIMIT_CLEAR,limit) != 0){
      return 1;
    }
    _highclear = (_highclear & 0xFE00) |(limit[0] & 0x1FF);
    writeLimitData(SHT3X_CMD_WRITE_HIGH_ALERT_LIMIT_CLEAR,_highclear);
    _lowclear = convertRawHumidity(lowclear);
    if(readLimitData(SHT3X_CMD_READ_LOW_ALERT_LIMIT_CLEAR,limit) != 0){
      return 1;
    }
    _lowclear = (_lowclear & 0xFE00) |(limit[0] & 0x1FF);
    writeLimitData(SHT3X_CMD_WRITE_LOW_ALERT_LIMIT_CLEAR,_lowclear);
    _lowset = convertRawHumidity(lowset);
    if(readLimitData(SHT3X_CMD_READ_LOW_ALERT_LIMIT_SET,limit) != 0){
      return 1;
    }
    _lowset = (_lowset & 0xFE00) |(limit[0] & 0x1FF);
    writeLimitData(SHT3X_CMD_WRITE_LOW_ALERT_LIMIT_SET,_lowset);
  } else {
    return 1;
  }
  return 0;
}

bool DFRobot_SHT3x::measureTemperatureLimitC(){

  uint16_t limit[1] ;
  float data;
  if(readLimitData(SHT3X_CMD_READ_HIGH_ALERT_LIMIT_SET,limit) != 0) {
    return false;
  }
  data = convertTempLimitData(limit);
  limitData.highSet = round(data);
  if(readLimitData(SHT3X_CMD_READ_HIGH_ALERT_LIMIT_CLEAR,limit) != 0){
    return false;
  }
  data = convertTempLimitData(limit);
  limitData.highClear = round(data);
  if(readLimitData(SHT3X_CMD_READ_LOW_ALERT_LIMIT_CLEAR,limit) != 0){
    return false;
  }
  data = convertTempLimitData(limit);
  limitData.lowClear = round(data);
  if(readLimitData(SHT3X_CMD_READ_LOW_ALERT_LIMIT_SET,limit) != 0){
    return false;
  }
  data = convertTempLimitData(limit);
  limitData.lowSet = round(data);
  return true;
}

bool DFRobot_SHT3x::measureTemperatureLimitF()
{
  uint16_t limit[1] ;
  float data;
  if(readLimitData(SHT3X_CMD_READ_HIGH_ALERT_LIMIT_SET,limit) != 0) {
    return false;
  }
  data = convertTempLimitData(limit);
  limitData.highSet = round(data * 9.0 / 5.0 + 32.0);
  if(readLimitData(SHT3X_CMD_READ_HIGH_ALERT_LIMIT_CLEAR,limit) != 0){
    return false;
  }
  data = convertTempLimitData(limit);
  limitData.highClear = round(data * 9.0 / 5.0 + 32.0);
  if(readLimitData(SHT3X_CMD_READ_LOW_ALERT_LIMIT_CLEAR,limit) != 0){
    return false;
  }
  data = convertTempLimitData(limit);
  limitData.lowClear = round(data * 9.0 / 5.0 + 32.0);
  if(readLimitData(SHT3X_CMD_READ_LOW_ALERT_LIMIT_SET,limit) != 0){
    return false;
  }
  data = convertTempLimitData(limit);
  limitData.lowSet = round(data * 9.0 / 5.0 + 32.0);
  return true;

}
float DFRobot_SHT3x::getTemperatureHighSetF(){
  return limitData.highSet;
}

float DFRobot_SHT3x::getTemperatureHighClearF(){
  return limitData.highClear;
}

float DFRobot_SHT3x::getTemperatureLowClearF(){
  return limitData.lowClear;
}
float DFRobot_SHT3x::getTemperatureLowSetF(){
  return limitData.lowSet;
}
uint8_t DFRobot_SHT3x::readLimitData(uint16_t cmd,uint16_t *pBuf)
{ 
  uint8_t rawData[3];
  uint8_t crc;

  writeCommand(cmd,2);
  readData(rawData,3);
  crc = rawData[2];
  if(checkCrc(rawData) != crc){
    return 1 ;
  }
  pBuf[1] = rawData[0];
  pBuf[1] = (pBuf[1] << 8) | rawData[1];
  return 0;
}
float DFRobot_SHT3x::getTemperatureHighSetC(){
  return limitData.highSet;
}
float DFRobot_SHT3x::getTemperatureHighClearC(){
  return limitData.highClear;
}
float DFRobot_SHT3x::getTemperatureLowClearC(){
  return limitData.lowClear;
}
float DFRobot_SHT3x::getTemperatureLowSetC(){
  return limitData.lowSet;
}
float DFRobot_SHT3x::getHumidityHighSetRH(){
  return limitData.highSet;
}
float DFRobot_SHT3x::getHumidityHighClearRH(){
  return limitData.highClear;
}
float DFRobot_SHT3x::getHumidityLowClearRH(){
  return limitData.lowClear;
}
float DFRobot_SHT3x::getHumidityLowSetRH(){
  return limitData.lowSet;
}

bool DFRobot_SHT3x::measureHumidityLimitRH()
{

  uint16_t limit[1];
  if(readLimitData(SHT3X_CMD_READ_HIGH_ALERT_LIMIT_SET,limit) != 0) {
    return false;
  }
  limitData.highSet = convertHumidityLimitData(limit);
  if(readLimitData(SHT3X_CMD_READ_HIGH_ALERT_LIMIT_CLEAR,limit) != 0){
    return false;
  }
  limitData.highClear = convertHumidityLimitData(limit);
  if(readLimitData(SHT3X_CMD_READ_LOW_ALERT_LIMIT_CLEAR,limit) != 0){
    return false;
  }
  limitData.lowClear = convertHumidityLimitData(limit);
  
  if(readLimitData(SHT3X_CMD_READ_LOW_ALERT_LIMIT_SET,limit) != 0){
    return false;
  }
  limitData.lowSet = convertHumidityLimitData(limit);
  return true;
}
float DFRobot_SHT3x::convertTemperature(uint8_t rawTemperature[])
{
  uint16_t rawValue ;
  rawValue = rawTemperature[0];
  rawValue = (rawValue << 8) | rawTemperature[1];
  return 175.0f * (float)rawValue / 65535.0f - 45.0f;
}
float DFRobot_SHT3x::convertHumidity(uint8_t rawHumidity[])
{
  uint16_t rawValue ;
  rawValue = rawHumidity[0];
  rawValue = (rawValue << 8) | rawHumidity[1];
  return 100.0f * (float)rawValue / 65535.0f;
}

uint16_t DFRobot_SHT3x::convertRawTemperature(float value)
{
  return (value + 45.0f) / 175.0f * 65535.0f;
}

uint16_t DFRobot_SHT3x::convertRawHumidity(float value)
{
  return value / 100.0f * 65535.0f;
}

float DFRobot_SHT3x::convertTempLimitData(uint16_t limit[])
{
  limit[1] = limit[1] << 7;
  limit[1] = limit[1] & 0xFF80;
  limit[1] = limit[1] | 0x1A;
  return 175.0f * (float)limit[1] / 65535.0f - 45.0f;
}

float DFRobot_SHT3x::convertHumidityLimitData(uint16_t limit[])
{
  limit[1] = limit[1] & 0xFE00;
  limit[1] = limit[1] | 0xCD;
  return round(100.0f * (float)limit[1] / 65535.0f) ;
}

uint8_t DFRobot_SHT3x::checkCrc(uint8_t data[])
{
    uint8_t bit;
    uint8_t crc = 0xFF;

    for (uint8_t dataCounter = 0; dataCounter < 2; dataCounter++)
    {
        crc ^= (data[dataCounter]);
        for (bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x131;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

void DFRobot_SHT3x::writeLimitData(uint16_t cmd,uint16_t limitData){
  uint8_t _pBuf[5];
  _pBuf[0] = cmd >>8;
  _pBuf[1] = cmd & 0xff;
  _pBuf[2] = limitData >> 8;
  _pBuf[3] = limitData & 0xff;
  uint8_t crc = checkCrc(_pBuf+2);
  _pBuf[4] = crc;
  write(_pBuf,5);
}

void DFRobot_SHT3x::writeCommand(uint16_t cmd,size_t size)
{
  uint8_t _pBuf[2];
  _pBuf[0] = cmd >> 8;
  _pBuf[1] = cmd & 0xFF;
  delay(1);
  write(_pBuf,2);
}

void DFRobot_SHT3x::write(const void* pBuf,size_t size)
{
  if (pBuf == NULL) {
    DBG("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;
  _pWire->beginTransmission(_address);
  for (uint8_t i = 0; i < size; i++) {
    _pWire->write(_pBuf[i]);
  }
  _pWire->endTransmission();
}

uint8_t DFRobot_SHT3x::readData(void *pBuf, size_t size) {
  if (pBuf == NULL) {
    DBG("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;

  _pWire->requestFrom(_address,size);
  uint8_t len = 0;
  for (uint8_t i = 0 ; i < size; i++) {
    _pBuf[i] = _pWire->read();
    len++;
  }

  return len;
}
