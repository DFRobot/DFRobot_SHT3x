/*!
 * @file alert.ino
 * @brief Temperature and humidity over-threshold alarm.
 * @details Experimental phenomenon: The user can customize the temperature and humidity thresholds, 
 * @n and the ALERT pin generates an alarm signal once the values exceed the user-defined threshold.
 * @n NOTE: The ALERT pin on the sensor should be connected to the interrupt pin on the main panel when using this function.
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @License  The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-08-26
 * @url https://github.com/DFRobot/DFRobot_SHT3x
 */

#include <DFRobot_SHT3x.h>
/*!
 * @brief Construct the function
 * @param pWire IIC bus pointer object and construction device, can both pass or not pass parameters, Wire in default.
 * @param address Chip IIC address, two optional addresses 0x44 and 0x45(0x45 in default).
 * @param RST Chip reset pin, 4 in default.
 * @n IIC address is determined by the pin addr on the chip.
 * @n When the ADR is connected to VDD, the chip IIC address is 0x45.
 * @n When the ADR is connected to GND, the chip IIC address is 0x44.
 */
//DFRobot_SHT3x sht3x(&Wire,/*address=*/0x45,/*RST=*/4);

DFRobot_SHT3x sht3x;
//The non-alarm status of the alert pin is low;
volatile  int alertState = 0;
void alert(){
  alertState = 1 - alertState;
}
void setup() {
  Serial.begin(9600);
  #ifdef ARDUINO_ARCH_MPYTHON 
  /*                    The Correspondence Table of ESP32 Interrupt Pins And Terminal Numbers
   * -----------------------------------------------------------------------------------------------------
   * |            |  DigitalPin  | P0-P20 can be used as an external interrupt                           |
   * |    esp32   |--------------------------------------------------------------------------------------|
   * |            | Interrupt No |  DigitalPinToInterrupt (Pn) can be used to query the interrupt number |
   * |---------------------------------------------------------------------------------------------------|
   */
  attachInterrupt(digitalPinToInterrupt(P16)/*Query the interrupt number of the P16 pin*/,alert,CHANGE);
  //Open esp32's P16 pin for external interrupt, bilateral edge trigger, ALERT connected to P16
  #else
  /*    The Correspondence Table of AVR Series Arduino Interrupt Pins And Terminal Numbers
   * ---------------------------------------------------------------------------------------
   * |                                        |  DigitalPin  | 2  | 3  |                   |
   * |    Uno, Nano, Mini, other 328-based    |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  |                   |
   * |-------------------------------------------------------------------------------------|
   * |                                        |    Pin       | 2  | 3  | 21 | 20 | 19 | 18 |
   * |               Mega2560                 |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  | 2  | 3  | 4  | 5  |
   * |-------------------------------------------------------------------------------------|
   * |                                        |    Pin       | 3  | 2  | 0  | 1  | 7  |    |
   * |    Leonardo, other 32u4-based          |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  | 2  | 3  | 4  |    |
   * |--------------------------------------------------------------------------------------
   */
  /*                      The Correspondence Table of micro:bit Interrupt Pins And Terminal Numbers
   * ---------------------------------------------------------------------------------------------------------------------------------------------
   * |             micro:bit                       | DigitalPin |P0-P20 can be used as an external interrupt                                     |
   * |  (When using as an external interrupt,      |---------------------------------------------------------------------------------------------|
   * |no need to set it to input mode with pinMode)|Interrupt No|Interrupt number is a pin digital value, such as P0 interrupt number 0, P1 is 1 |
   * |-------------------------------------------------------------------------------------------------------------------------------------------|
   */
  attachInterrupt(/*Interrupt No*/0,alert,CHANGE);//Open the external interrupt 0, connect ALERT to the digital pin of the main control: 
     //UNO(2), Mega2560(2), Leonardo(3), microbit(P0).
  #endif
    //Initialize the chip to detect if it can communicate properly
  while (sht3x.begin() != 0) {
    Serial.println("The initialization of the chip is failed, please confirm whether the chip connection is correct");
    delay(1000);
  }
  /**
   * readSerialNumber Read the serial number of the chip
   * @return Return 32-digit serial number
   */
  Serial.print("The chip serial number");
  Serial.println(sht3x.readSerialNumber());
  /**
   * softReset Send command resets via iiC, enter the chip's default mode single-measure mode, turn off the heater, 
   * and clear the alert of the ALERT pin.
   * @return Read the status register to determine whether the command was executed successfully, and returning true indicates success.
   */
  if(!sht3x.softReset()){
     Serial.println("Failed to reset the chip");
   }
  /**
   * @brief All flags (Bit 15, 11, 10, 4) in the status register can be cleared (set to zero).
   * @n ALERT can work properly only when the bit:15 is set to 0, otherwise it will remain high.
   */
  sht3x.clearStatusRegister();
  /**
   * startPeriodicMode Enter cycle measurement mode and set repeatability, read frequency, and only in this mode ALERT can work.
   * @param measureFreq Read the data frequency, data type eMeasureFrequency_t
   * @note  Selectable parameters:
               eMeasureFreq_Hz5,   /**the chip collects data in every 2s
               eMeasureFreq_1Hz,   /**the chip collects data in every 1s 
               eMeasureFreq_2Hz,   /**the chip collects data in every 0.5s 
               eMeasureFreq_4Hz,   /**the chip collects data in every 0.25s 
               eMeasureFreq_10Hz   /**the chip collects data in every 0.1s 
   * @param repeatability Read the repeatability of temperature and humidity data, the default parameter is eRepeatability_High.
   * @note  Optional parameters:
               eRepeatability_High /**In high repeatability mode, the humidity repeatability is 0.10%RH, the temperature repeatability is 0.06°C
               eRepeatability_Medium,/**In medium repeatability mode, the humidity repeatability is 0.15%RH, the temperature repeatability is 0.12°C.
               eRepeatability_Low, /**In low repeatability mode, the humidity repeatability is0.25%RH, the temperature repeatability is 0.24°C
   * @return Read the status of the register to determine whether the command was executed successfully, and return true indicates success.
   */
  if(!sht3x.startPeriodicMode(sht3x.eMeasureFreq_10Hz)){
    Serial.println("Failed to enter the periodic mode");
  }
  /**
   * setTemperatureLimitC Set the threshold temperature and alarm clear temperature(°C)
   * setTemperatureLimitF Set the threshold temperature and alarm clear temperature(°F)
   * @param highset High temperature alarm point, when the temperature is greater than this value, the ALERT pin generates an alarm signal.
   * @param highClear High temperature alarm clear point, alarming when the temp higher than the highset, otherwise the alarm signal will be cleared.
   * @param lowset Low temperature alarm point, when the temperature is lower than this value, the ALERT pin generates an alarm signal.
   * @param lowclear Low temperature alarm clear point, alarming when the temp lower than the highset, otherwise the alarm signal will be cleared.
   * @note The filled value should be an integer (range: -40 to 125 degrees Celsius), -40 to 257 (Fahrenheit)highset>highClear>lowclear>lowset)
   */
  //sht3x.setTemperatureLimitF(/*highset=*/35,/*highClear=*/34,/*lowSet=*/18,/*lowClear=*/20)
  if(sht3x.setTemperatureLimitC(/*highset=*/35,/*highClear=*/34,/*lowSet=*/18,/*lowClear=*/20) != 0){
    Serial.println("Failed to set the temperature limit");
  }
  /**
   * setHumidityLimitRH Set the relative humidity threshold temperature and the alarm clear humidity(%RH)
   * @param highset High humidity alarm point, when the humidity is greater than this value, the ALERT pin generates an alarm signal.
   * @param highClear High humidity alarm clear point, alarming when the humidity higher than the highset, otherwise the alarm signal will be cleared.
   * @param lowset Low humidity alarm point, when the humidity is lower than this value, the ALERT pin generates an alarm signal.
   * @param lowclear Low humidity alarm clear point, alarming when the humidity lower than the highset, otherwise the alarm signal will be cleared.
   * @note The filled value should be an integer (range: 0 - 100 %RH,highset>highClear>lowclear>lowset) 
   */
  if(sht3x.setHumidityLimitRH(/*highset=*/70,/*highClear=*/68,/*lowSet=*/19,/*lowClear=*/20) != 0){
    Serial.println("Failed to set the humidity limit");
  }
  //Serial.println(F("string") Save stings to flash to save the dynamic ram when compiling.
  Serial.println(F("----------------------Alarm Detection-------------------------------"));
  Serial.println(F("Alarms raised when temp and humidity are out of the threshold range. Please connect ALERT to the main control board interrupt pin"));
  Serial.println(F("-Different main contorl UNO(2), Mega2560(2), Leonardo(3), microbit(P0), mPython(P16)----"));
  Serial.println(F("----------------------the humidity limit(%RH)-----------------------------------"));
  /**
   * @brief Measure relative humidity threshold temperature and alarm clear humidity
   * @return Return true indicates successful data acquisition
   */
  if(sht3x.measureHumidityLimitRH()){
    Serial.print("high set:");
    //getHumidityHighSetRH() Get the high humidity alarm point
    Serial.print(sht3x.getHumidityHighSetRH());
    Serial.print("               low clear:");
    //getHumidityHighClearRH() Get the high humidity alarm clear point
    Serial.println(sht3x.getHumidityLowClearRH());
    Serial.print("high clear:");
    //getHumidityLowClearRH() Get the low humidity alarm clear point
    Serial.print(sht3x.getHumidityHighClearRH());
    Serial.print("               low set:");
    //getHumidityLowSetRH() Get the low humidity alarm point
    Serial.println(sht3x.getHumidityLowSetRH());
  } else {
    Serial.println("Failed to get the humidity limit");
  }
  /**
   * measureTemperatureLimitC Measure the threshold temperature and alarm clear temperature(°C)
   * measureTemperatureLimitF Measure the threshold temperature and alarm clear temperature(°F)
   * @return Return true indicates successful data acquisition
   */
  Serial.println("----------------------temperature limit(°C)---------------------------------");
  //Serial.println(F("----------------------temperature limit(°F)---------------------------------"));
  if(sht3x.measureTemperatureLimitC()){
    Serial.print("high set:");
    //getTemperatureHighSetC() Get high temperature alarm points(°C)
    //getTemperatureHighSetF() Get high temperature alarm points(°F)
    Serial.print(sht3x.getTemperatureHighSetC());
    Serial.print("               low clear:");
    //getTemperatureHighClearC() Get high temperature alarm clear points(°C)
    //getTemperatureHighClearF() Get high temperature alarm clear points(°F))
    Serial.println(sht3x.getTemperatureLowClearC());
    Serial.print("high clear:");
    //getTemperatureLowClearC() Get low temperature alarm clear points(°C)
    //getTemperatureLowClearF() Get low temperature alarm clear points(°F)
    Serial.print(sht3x.getTemperatureHighClearC());
    Serial.print("               low set:");
    //getTemperatureLowSetC() Get low temperature alarm points(°C)
    //getTemperatureLowSetF() Get low temperature alarm points(°F)
    Serial.println(sht3x.getTemperatureLowSetC());
    Serial.println("------------------------------------------------------------------");
  } else {
    Serial.println("Failed to get temperature limit");
  }
  /**
   * readAlertState Read the status of the ALERT pin.
   * @return High returns 1, low returns 0.
   */
  //To initialize the state of ALERT
  if(sht3x.readAlertState() == 1){
    alertState = 1;
  } else {
    alertState = 0;
  }
}   
void loop() {
  Serial.print("environment temperature(°C/F):");
  /**
   * getTemperatureC Get the measured temperature (in degrees Celsius)
   * @return Return temperature data of the type float
   */
  Serial.print(sht3x.getTemperatureC());
  Serial.print(" C/");
  /**
   * getTemperatureF Get the measured temperature (in degrees Celsius)
   * @return Return temperature data of the type float
   */
  Serial.print(sht3x.getTemperatureF());
  Serial.print(" F      ");
  Serial.print("relative humidity(%RH):");
  /**
   * getHumidityRH Get measured humidity (in %RH)
   * @return Return humidity data of the type float
   */
  Serial.print(sht3x.getHumidityRH());
  Serial.println(" %RH");
  //The read data frequency should greater than the frequency to collect data, otherwise the return data will make errors.
  if(alertState == 1){
    /**
     * @brief Determine if the temperature and humidity are out of the threshold range
     * @return Return the status code, representing as follows
     * @n 01 Indicates that the humidity exceeds the lower threshold range
     * @n 10 Indicates that the temperature exceeds the lower threshold range
     * @n 11 Indicates that both the humidity and the temperature exceed the lower threshold range
     * @n 02 Indicates that the humidity exceeds the upper threshold range
     * @n 20 Indicates that the temperature exceeds the upper threshold range
     * @n 22 Indicates that both the humidity and the temperature exceed the upper threshold range
     * @n 12 Indicates that the temperature exceeds the lower threshold range,
     //and the humidity exceeds the upper threshold range
     * @n 21 Indicates that the temperature exceeds the upper threshold range,
     //and the humidity exceeds the lower threshold range
     * @n 0  Back to normal, but the alarm is not cleared.
     */
    uint8_t state = sht3x.environmentState();
    //Serial.println(F("string") Save stings to flash to save the dynamic ram when compiling.
    if(state == 1)  Serial.println(F("The humidity exceeds the lower threshold range!"));
    else if(state == 10)  Serial.println(F("The temperature exceeds the lower threshold range!"));
    else if(state == 11)  Serial.println(F("The humidity and the temperature exceed the lower threshold range!"));
    else if(state == 2)   Serial.println(F("The humidity exceeds the upper threshold range!"));
    else if(state == 20)  Serial.println(F("The temperature exceeds the upper threshold range!"));
    else if(state == 22)  Serial.println(F("The humidity and the temperature exceed the upper threshold range!"));
    else if(state == 12)  Serial.println(F("The temperature exceeds the lower threshold range,the humidity exceeds the upper threshold range!"));
    else if(state == 21)  Serial.println(F("The temperature exceeds the upper threshold range, and the humidity exceeds the lower threshold range!"));
    else Serial.println(F("T&H back to normal, but the alarm is not cleared!"));
  } else {
    Serial.println(F("T&H in normal range, alarm cleared"));
  }
  delay(1000);
}
