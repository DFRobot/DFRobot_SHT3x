/*!
 * @file DFRobot_SHT3x.h
 * @brief Define the infrastructure of the DFRobot_SHT3x class
 * @details This is a library of digital temperature and humidity sensors used to drive the SHT3x series SHT30, 
 * @n SHT31 and SHT35 to read ambient temperature and relative humidity.
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @License     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-08-19
 * @url https://github.com/DFRobot/DFRobot_SHT3x
 */
 
#ifndef DFROBOT_SHT3X_H
#define DFROBOT_SHT3X_H
#include "Arduino.h"
#include <Wire.h>

//#define ENABLE_DBG
#ifdef ENABLE_DBG
#define DBG(...) {Serial.print("[");Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif

#define SHT3X_CMD_GETDATA_POLLING_H (0x2400) // measurement: polling, high repeatability
#define SHT3X_CMD_GETDATA_POLLING_M (0x240B) // measurement: polling, medium repeatability
#define SHT3X_CMD_GETDATA_POLLING_L (0x2416) // measurement: polling, low repeatability

#define SHT3X_CMD_READ_SERIAL_NUMBER             (0x3780)///<  Read the chip serial number
#define SHT3X_CMD_GETDATA_H_CLOCKENBLED          (0x2C06)///<  Measurement:high repeatability
#define SHT3X_CMD_GETDATA_M_CLOCKENBLED          (0x2C0D)///<  Measurement: medium repeatability
#define SHT3X_CMD_GETDATA_L_CLOCKENBLED          (0x2C10)///<  Measurement: low repeatability
#define SHT3X_CMD_SETMODE_H_FREQUENCY_HALF_HZ    (0x2032)///<  Measurement: periodic 0.5 mps, high repeatability
#define SHT3X_CMD_SETMODE_M_FREQUENCY_HALF_HZ    (0x2024)///<  Measurement: periodic 0.5 mps, medium
#define SHT3X_CMD_SETMODE_L_FREQUENCY_HALF_HZ    (0x202F)///<  Measurement: periodic 0.5 mps, low repeatability
#define SHT3X_CMD_SETMODE_H_FREQUENCY_1_HZ       (0x2130)///<  Measurement: periodic 1 mps, high repeatability
#define SHT3X_CMD_SETMODE_M_FREQUENCY_1_HZ       (0x2126)///<  Measurement: periodic 1 mps, medium repeatability
#define SHT3X_CMD_SETMODE_L_FREQUENCY_1_HZ       (0x212D)///<  Measurement: periodic 1 mps, low repeatability
#define SHT3X_CMD_SETMODE_H_FREQUENCY_2_HZ       (0x2236)///<  Measurement: periodic 2 mps, high repeatability
#define SHT3X_CMD_SETMODE_M_FREQUENCY_2_HZ       (0x2220)///<  Measurement: periodic 2 mps, medium repeatability
#define SHT3X_CMD_SETMODE_L_FREQUENCY_2_HZ       (0x222B)///<  Measurement: periodic 2 mps, low repeatability
#define SHT3X_CMD_SETMODE_H_FREQUENCY_4_HZ       (0x2334)///<  Measurement: periodic 4 mps, high repeatability
#define SHT3X_CMD_SETMODE_M_FREQUENCY_4_HZ       (0x2322)///<  Measurement: periodic 4 mps, medium repeatability
#define SHT3X_CMD_SETMODE_L_FREQUENCY_4_HZ       (0x2329)///<  Measurement: periodic 4 mps, low repeatability
#define SHT3X_CMD_SETMODE_H_FREQUENCY_10_HZ      (0x2737)///<  Measurement: periodic 10 mps, high repeatability
#define SHT3X_CMD_SETMODE_M_FREQUENCY_10_HZ      (0x2721)///<  Measurement: periodic 10 mps, medium
#define SHT3X_CMD_SETMODE_L_FREQUENCY_10_HZ      (0x272A)///<  Measurement: periodic 10 mps, low repeatability
#define SHT3X_CMD_GETDATA                        (0xE000)///<  Readout measurements for periodic mode

#define SHT3X_CMD_STOP_PERIODIC_ACQUISITION_MODE (0x3093)///< 
#define SHT3X_CMD_SOFT_RESET                     (0x30A2)///<  Soft reset
#define SHT3X_CMD_HEATER_ENABLE                  (0x306D)///<  Enabled heater
#define SHT3X_CMD_HEATER_DISABLE                 (0x3066)///<  Disable heater
#define SHT3X_CMD_READ_STATUS_REG                (0xF32D)///<  Read status register
#define SHT3X_CMD_CLEAR_STATUS_REG               (0x3041)///<  Clear status register

#define SHT3X_CMD_READ_HIGH_ALERT_LIMIT_SET      (0xE11F)///<  Read alert limits, high set
#define SHT3X_CMD_READ_HIGH_ALERT_LIMIT_CLEAR    (0xE114)///<  Read alert limits, high clear
#define SHT3X_CMD_READ_LOW_ALERT_LIMIT_CLEAR     (0xE109)///<  Read alert limits, low clear
#define SHT3X_CMD_READ_LOW_ALERT_LIMIT_SET       (0xE102)///<  Read alert limits, low set
#define SHT3X_CMD_WRITE_HIGH_ALERT_LIMIT_SET     (0x611D)///<  Write alert limits, high set
#define SHT3X_CMD_WRITE_HIGH_ALERT_LIMIT_CLEAR   (0x6116)///<  Write alert limits, high clear
#define SHT3X_CMD_WRITE_LOW_ALERT_LIMIT_CLEAR    (0x610B)///<  Write alert limits, low clear
#define SHT3X_CMD_WRITE_LOW_ALERT_LIMIT_SET      (0x6100)///<  Write alert limits, low set
class DFRobot_SHT3x
{
public:
  #define ERR_OK             0      //No error
  #define ERR_DATA_BUS      -1      //Data bus error
  #define ERR_IC_VERSION    -2      //Chip version does not match
  
  /**
   * @struct sStatusRegister_t
   * @brief The status register contains information on the operational status of the heater, the alert mode and on the execution status of 
   * @n the last command and the last write sequence. 
    
   * @n ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            b15       |       b14 |      b13   |      b12 |        b11   |        b10       |  b5~b9    |    b4              |    b2~b3 |    b1       |       b0              |
   * @n ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   * @n  alertPendingStatus| reserved3 | heaterStaus|reserved2 |humidityAlert | temperatureAlert | reserved1 | systemResetDeteced |reserved0 |commendStatus|writeDataChecksumStatus|
   * @n ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   * @n writeDataChecksumStatus:   '0' : checksum of last write transfer was correct
   * @n                            '1' : checksum of last write transfer failed
   * @n commendStatus              '0' : last command executed successfully
   * @n                            '1' : last command not processed. 
   * @n systemResetDeteced         '0' : no reset detected since last ‘clear status register’ command
   * @n                            '1' : reset detected (hard reset, soft reset command or supply fail)
   * @n temperatureAlert           '0' : no alert
   * @n                            '1' : alert
   * @n humidityAlert              '0' : no alert
   * @n                            '1' : alert
   * @n heaterStaus                '0' : heater OFF
   * @n                            '1' : heater ON
   * @n alertPendingStatus         '0' : no pending alerts
   * @n                            '1' : at least one pending alert
  */
  typedef struct {
    uint16_t writeDataChecksumStatus : 1;
    uint16_t commandStatus : 1;
    uint16_t reserved0 : 2;
    uint16_t systemResetDeteced : 1;
    uint16_t reserved1 : 5;
    uint16_t temperatureAlert : 1;
    uint16_t humidityAlert : 1;
    uint16_t reserved2 : 1;
    uint16_t heaterStaus :1;
    uint16_t reserved3 :1;
    uint16_t alertPendingStatus :1;
  }__attribute__ ((packed)) sStatusRegister_t;
  
  /**
   * @struct eMode_t
   * @brief Two measurement modes for the chip
   */
  typedef enum {
    ePeriodic,/**<Cycle measurement mode*/
    eOneShot,/**<Single measurement mode*/
  } eMode_t;
  
  /**
   * @struct eRepeatability_t
   * @brief We can choose the repeatability of the chip to measure temperature and humidity data
   * @details (which means the difference between the data measured by the chip under two identical measurementconditions).
   * @note There are 3 repeatabilities to choose: low, medium and high. The higher repeatability, the more accurate data.
  */
  typedef enum{
    eRepeatability_High = 0,/**<In high repeatability mode, the humidity repeatability is 0.10%RH, the temperature repeatability is 0.06°C*/
    eRepeatability_Medium = 1,/**<In medium repeatability mode, the humidity repeatability is 0.15%RH, the temperature repeatability is 0.12°C*/
    eRepeatability_Low = 2,/**<In low repeatability mode, the humidity repeatability is0.25%RH, the temperature repeatability is 0.24°C*/
  } eRepeatability_t;
  
  /**
   * @struct eMeasureFrequency_t
   * @brief Under the periodic data acquisition mode, we can select the frequency at which the chip measures temperature and humidity data.
   * @n Optional frequencies are 0.5Hz, 1Hz, 2Hz, 4Hz, 10Hz.
   */
  typedef enum{
    eMeasureFreq_Hz5 = 0,
    eMeasureFreq_1Hz = 1,
    eMeasureFreq_2Hz = 2,
    eMeasureFreq_4Hz = 3,
    eMeasureFreq_10Hz = 4,
  } eMeasureFrequency_t;

  /**
   * @struct sRHAndTemp_t
   * @brief Structures used to store temperature and relative humidity
   */
  typedef struct{
    float TemperatureC;
    float Humidity;
    float TemperatureF;
    int ERR;
  }sRHAndTemp_t;
  
  /**
   * @struct sMode_t
   * @brief Structures used to store the limits of temperature and relative humidity read
   */
  typedef struct{
    float highSet;/**<Define the temperature (C)/humidity (%RH) range upper threshold, ALERT generates a high-level alarm once the data greater than the value defined*/
    float highClear;/**<Clear the alarm once the temperature (C)/humidity (%RH) less than the value defined>*/
    float lowSet;/**<Define the temperature (C)/humidity (%RH) range low threshold,ALERT generates a high-level alarm once the data lower than the value defined>*/
    float lowClear;/**<Clear the alarm once the temperature (C)/humidity (%RH) more than the value defined>*/
  } sLimitData_t;
  
public:

  /**
   * @fn DFRobot_Sensor
   * @brief Construct the function
   * @param pWire IC bus pointer object and construction device, can both pass or not pass parameters, Wire in default.
   * @param address Chip IIC address, two optional addresses 0x44 and 0x45(0x45 in default).
   * @param RST Chip reset pin, 4 in default.
   * @n The IIC address is determined by the pin addr on the chip.
   * @n When the ADR is connected to VDD, the chip IIC address is 0x45.
   * @n When the ADR is connected to GND, the chip IIC address is 0x44.
   */
  DFRobot_SHT3x(TwoWire *pWire = &Wire, uint8_t address = 0x45,uint8_t RST = 4);
  
  /**
   * @fn readSerialNumber
   * @brief Read the serial number of the chip
   * @return 32-digit serial number
   */
  uint32_t  readSerialNumber();
  
  /**
   * @fn begin
   * @brief Initialize the function
   * @return Return 0 indicates a successful initialization, while other values indicates failure and return to error code.
   */
  int begin();
  
  /**
   * @fn softReset
   * @brief Send command resets via iiC, enter the chip's default mode single-measure mode, 
   * turn off the heater, and clear the alert of the ALERT pin.
   * @return Read the status register to determine whether the command was executed successfully, and returning true indicates success
   */
  bool softReset();
  
  /**
   * @fn pinReset
   * @brief Reset through the chip's reset pin, enter the chip's default mode single-measure mode, and clear the alert of the ALERT pin.
   * @return The status register has a data bit that detects whether the chip has been reset, and returning true indicates success
   */
  bool pinReset();
  
  /**
   * @fn readTemperatureAndHumidity
   * @brief Get temperature and humidity data in single measurement mode.
   * @param repeatability Set repeatability to read temperature and humidity data with the type eRepeatability_t.
   * @return Return a structure containing celsius temperature (°C), Fahrenheit temperature (°F), relative humidity (%RH), status code
   * @n A status of 0 indicates the right return data.
   */
  sRHAndTemp_t readTemperatureAndHumidity(eRepeatability_t repeatability );
  
  /**
   * @fn getTemperatureC
   * @brief Get the measured temperature (in degrees Celsius)
   * @return Return the float temperature data 
   */
  float getTemperatureC();
  
  /**
   * @fn getTemperatureF
   * @brief Get the measured temperature (in degrees Fahrenheit)
   * @return Return the float temperature data 
   */
  float getTemperatureF();
  
  /**
   * @fn getHumidityRH
   * @brief Get measured humidity(%RH)
   * @return Return the float humidity data
   */
  float getHumidityRH();
  
  /**
   * @fn startPeriodicMode
   * @brief Enter cycle measurement mode and set repeatability(the difference between the data measured 
   * the difference between the data measured by the chip under the same measurement conditions)
   * @param measureFreq  Read the eMeasureFrequency_t data frequency
   * @param repeatability  Set repeatability to read temperature and humidity data with the type eRepeatability_t. 
   * eRepeatability_High(high repeatability mode) in default.
   * @return Return true indicates a successful entrance to cycle measurement mode.
   */
  bool startPeriodicMode(eMeasureFrequency_t measureFreq,eRepeatability_t repeatability = eRepeatability_High);
  
  /**
   * @fn readTemperatureAndHumidity
   * @brief Get temperature and humidity data in cycle measurement mode.
   * @return Return a structure containing celsius temperature (°C), Fahrenheit temperature (°F), relative humidity (%RH), status code
   * @n A status of 0 indicates the right return data.
   */
  sRHAndTemp_t readTemperatureAndHumidity();
  
  /**
   * @fn stopPeriodicMode
   * @brief Exit from cycle measurement mode
   * @return Read the status of the register to determine whether the command was executed successfully, and returning true indicates success
   */
  bool stopPeriodicMode();
  
  /**
   * @fn heaterEnable
   * @brief Turn on the heater inside the chip
   * @return Read the status of the register to determine whether the command was executed successfully, and returning true indicates success
   * @note Heaters should be used in wet environments, and other cases of use will result in incorrect readings
   */
  bool heaterEnable();
  
  /**
   * @fn heaterDisable
   * @brief Turn off the heater inside the chip
   * @return Read the status of the register to determine whether the command was executed successfully, and returning true indicates success
   * @note Heaters should be used in wet environments, and other cases of use will result in incorrect readings
   */
  bool heaterDisable();
  
  /**
   * @fn clearStatusRegister
   * @brief All flags (Bit 15, 11, 10, 4) in the status register can be cleared (set to zero)
   * @n  Set bit:15 to 0 so that ALERT pin can work, otherwise it will keep high.
   */
  void clearStatusRegister();
  
  /**
   * @fn readAlertState
   * @brief Read the state of the pin ALERT.
   * @return High returns 1, low returns 0.
   */
  bool readAlertState();
  
  /**
   * @fn environmentState
   * @brief Determine if the temperature and humidity are out of the threshold range
   * @return Return the status code, representing as follows
   * @retval 01 ：Indicates that the humidity exceeds the lower threshold range
   * @retval 10 ：Indicates that the temperature exceeds the lower threshold range
   * @retval 11 ：Indicates that both the humidity and the temperature exceed the lower threshold range
   * @retval 02 ：Indicates that the humidity exceeds the upper threshold range
   * @retval 20 ：Indicates that the temperature exceeds the upper threshold range
   * @retval 22 ：Indicates that both the humidity and the temperature exceed the upper threshold range
   * @retval 12 ：Indicates that the temperature exceeds the lower threshold range,
   * @n and the humidity exceeds the upper threshold range
   * @retval 21 ：Indicates that the temperature exceeds the upper threshold range,
   * @n and the humidity exceeds the lower threshold range
   */
  uint8_t environmentState();
  
  /**
   * @fn setTemperatureLimitC
   * @brief Set the threshold temperature and alarm clear temperature(°C)
   * @param highset  High temperature alarm point, when the temperature is greater than this value, the ALERT pin generates an alarm signal.
   * @param highClear  High temperature alarm clear point, alarming when the temp higher than the highset, otherwise the alarm signal will be cleared.
   * @param lowset  Low temperature alarm point, when the temperature is lower than this value, the ALERT pin generates an alarm signal.
   * @param lowclear  Low temperature alarm clear point, alarming when the temp lower than the highset, otherwise the alarm signal will be cleared.
   * @note range: -40 to 125 degrees Celsius, highset > highClear > lowclear > lowset. 
   * @return  A return to 0 indicates a successful setting.
   */
  uint8_t  setTemperatureLimitC(float highset,float highclear,float lowset,float lowclear);
  
  /**
   * @fn setTemperatureLimitF
   * @brief Set the threshold temperature and alarm clear temperature(°F)
   * @param highset  High temperature alarm point, when the temperature is greater than this value, the ALERT pin generates an alarm signal.
   * @param highClear High temperature alarm clear point, alarming when the temp higher than the highset, otherwise the alarm signal will be cleared.
   * @param lowset Low temperature alarm point, when the temperature is lower than this value, the ALERT pin generates an alarm signal.
   * @param lowclear Low temperature alarm clear point, alarming when the temp lower than the highset, otherwise the alarm signal will be cleared.
   * @note Range: -40 to 257 (Fahrenheit), highset > highClear > lowclear > lowset.
   * @return  A return to 0 indicates a successful setting.
   */
  uint8_t  setTemperatureLimitF(float highset,float highclear, float lowset,float lowclear);

  /**
   * @fn setHumidityLimitRH
   * @brief Set the relative humidity threshold temperature and the alarm clear humidity(%RH)
   * @param highset  High humidity alarm point, when the humidity is greater than this value, the ALERT pin generates an alarm signal.
   * @param highClear  High humidity alarm clear point, alarming when the humidity higher than the highset, otherwise the alarm signal will be cleared.
   * @param lowset  Low humidity alarm point, when the humidity is lower than this value, the ALERT pin generates an alarm signal.
   * @param lowclear  Low humidity alarm clear point, alarming when the humidity lower than the highset, otherwise the alarm signal will be cleared.
   * @note range: 0 - 100 %RH, highset > highClear > lowclear > lowset 
   * @return: A return to 0 indicates a successful setting.
   */
  uint8_t setHumidityLimitRH(float highset,float highclear, float lowset,float lowclear);
  
  /**
   * @fn measureTemperatureLimitC
   * @brief Measure temperature threshold temperature and alarm clear temperature
   * @return Return true indicates successful data acquisition
   */
  bool measureTemperatureLimitC();
  
  /**
   * @fn getTemperatureHighSetC
   * @brief Get high temperature alarm points(°C)
   * @return Return high temperature alarm points(°C)
   */
  float getTemperatureHighSetC();
  
  /**
   * @fn getTemperatureHighClearC
   * @brief Get high temperature alarm clear points(°C)
   * @return Return high temperature alarm clear points(°C)
   */
  float getTemperatureHighClearC();
  
  /**
   * @fn getTemperatureLowClearC
   * @brief Get low temperature alarm clear points(°C)
   * @return Return low temperature alarm clear points(°C)
   */
  float getTemperatureLowClearC();
  
  /**
   * @fn getTemperatureLowSetC
   * @brief Get low temperature alarm points(°C)
   * @return Return low temperature alarm points
   */
  float getTemperatureLowSetC();
  
  /**
   * @fn measureTemperatureLimitF
   * @brief Measure the threshold temperature and alarm clear temperature
   * @return Return true indicates successful data acquisition
   */
  bool measureTemperatureLimitF();
  
  /**
   * @fn getTemperatureHighSetF
   * @brief Get high temperature alarm points(°F)
   * @return Return high temperature alarm points(°F)
   */
  float getTemperatureHighSetF();
  
  /**
   * @fn getTemperatureHighClearF
   * @brief Get high temperature alarm clear points(°F)
   * @return Return high temperature alarm clear points(°F))
   */
  float getTemperatureHighClearF();
  
  /**
   * @fn getTemperatureLowClearF
   * @brief Get low temperature alarm clear points(°F)
   * @return Return low temperature alarm clear points(°F)
   */
  float getTemperatureLowClearF();
  
  /**
   * @fn getTemperatureLowSetF
   * @brief Get low temperature alarm points(°F)
   * @return Return low temperature alarm points
   */
  float getTemperatureLowSetF();
  
  /**
   * @fn measureHumidityLimitRH
   * @brief Measure the threshold humidity of relative humidity and alarm clear humidity
   * @return Return true indicates successful data acquisition
   */
  bool measureHumidityLimitRH();
  
  /**
   * @fn getHumidityHighSetRH
   * @brief Get the high humidity alarm point(%RH)
   * @return Return the high humidity alarm point
   */
  float getHumidityHighSetRH();

  /**
   * @fn getHumidityHighClearRH
   * @brief Get the high humidity alarm clear point(%RH)
   * @return Return the high humidity alarm clear point
   */
  float getHumidityHighClearRH();
  
  /**
   * @fn getHumidityLowClearRH
   * @brief Get the low humidity alarm clear point(%RH)
   * @return Return the low humidity alarm clear point
   */
  float getHumidityLowClearRH();
  
  /**
   * @fn getHumidityLowSetRH
   * @brief Get the low humidity alarm point
   * @return Return the low humidity alarm point
   */
  float getHumidityLowSetRH();

private:

  /**
   * @fn writeCommand
   * @brief Write commands to the sensor chip
   * @param cmd  chip command
   * @param size  The number of command data, 8 digits for one data.
   */
  void  writeCommand(uint16_t cmd,size_t size);
  /**
   * @fn readStatusRegister
   * @brief Read the data stored in the status register.
   * @return Return to status like whether heater is ON or OFF, the status of the pin alert, reset status and the former cmd is executed or not.
   */
  sStatusRegister_t readStatusRegister();
  
  /**
   * @fn writeLimitData
   * @brief Write threshold data.
   * @param cmd  Send threshold data of chip command.
   * @param limitData Raw data on temperature and humidity need to be sent (humidity is 7 bits and temperatures are 11 bits).
   * @return Return 0 indicates that the command was sent successfully, other return values suggest unsuccessful send.
   */
  void writeLimitData(uint16_t cmd,uint16_t limitData);
  
  /**
   * @fn readLimitData
   * @brief Read threshold data.
   * @param cmd  Read threshold data of chip command.
   * @param *pBuf Save read data.
   */
  uint8_t readLimitData(uint16_t cmd,uint16_t *pBuf);
  /**
   * @fn readData
   * @brief Write command to sensor chip.
   * @param pBuf  The data contained in the command.
   * @param size  Number of command data
   * @return Return 0 indicates the successful read, other return values suggest unsuccessful read.
   */
  uint8_t readData(void *pBuf,size_t size);
  
  /**
   * @fn checkCrc
   * @brief CRC calibration.
   * @param data[] Data need to be calibrated.
   * @return Obtained calibration code.
   */
  uint8_t checkCrc(uint8_t data[]);
  
  /**
   * @fn convertTemperature
   * @brief Convert the data returned from the sensor to temperature(°C).
   * @param Data obtained from the sensor
   * @return Celsius temperature.
   */
  float convertTemperature(uint8_t rawTemperature[]);
  /**
   * @fn convertHumidity
   * @brief Convert the data returned from the sensor to relative humidity.
   * @param Data obtained from the sensor.
   * @return Relative humidity.
   */
  float convertHumidity(uint8_t rawHumidity[]);
  
  /**
   * @fn convertRawTemperature
   * @brief The temperature data to be written is converted into the data needed by the chip.
   * @param The temperature need to be written.
   * @return Data write to sensor.
   */
  uint16_t convertRawTemperature(float value);
  /**
   * @fn convertRawHumidity
   * @brief The relative humidity data to be written is converted into the data needed by the chip.
   * @param The relative humidity data to be written
   * @return Data write to sensor.
   */
  uint16_t convertRawHumidity(float value);
  /**
   * @fn convertTempLimitData
   * @brief Convert the data returned from the sensor to temperature limited data
   * @param Temperature limited data from sensor
   * @return Temperature limited data
   */
  float convertTempLimitData(uint16_t limit[]);
  /**
   * @fn convertHumidityLimitData
   * @brief Convert the data returned from the sensor to humidity limited data
   * @param Humidity limited data from sensor
   * @return Humidity limited data
   */
  float convertHumidityLimitData(uint16_t limit[]);
  /**
   * @fn write
   * @brief Transport data to chip
   * @param Data address
   * @param Data length
   */
  void write(const void* pBuf,size_t size);
  
private:

  sLimitData_t limitData;
  sRHAndTemp_t tempRH;
  TwoWire *_pWire;
  eMode_t measurementMode ;
  uint8_t _address;
  uint8_t _RST;
  float tempHighSet ;
  float tempLowSet ;
};   
#endif
