
/*!
 * @file singleMeasurement.ino
 * @brief Read ambient temperature (C/F) and relative humidity (%RH) in single-read mode.
 * @details n Experimental phenomenon: the chip defaults in this mode, we need to send instructions to enable the chip collect data,
 * which means the repeatability of the read needs to be set (the difference between the data measured by the chip under the same measurement conditions)
 * then read the temperature and humidity data and print the data in the serial port.
 * @n Single measure mode: read data as needed, power consumption is relatively low, the chip idle state only costs 0.5mA. 
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @License     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-08-21
 * @url https://github.com/DFRobot/DFRobot_SHT3x
*/


#include <DFRobot_SHT3x.h>


/*!

 * @brief Construct the function

 * @param pWire IIC bus pointer object and construction device, both can pass or not pass parameters, 
 * Wire in default.

 * @param address Chip IIC address, two optional addresses 0x44 and 0x45(0x45 in default).

 * @param RST RST Chip reset pin, 4 in default.

 * @n IIC address is determined by the pin addr on the chip.

 * @n When the ADR is connected to VDD, the chip IIC address is 0x45.

 * @n When the ADR is connected to GND, the chip IIC address is 0x44.

 */

//DFRobot_SHT3x sht3x(&Wire,/*address=*/0x45,/*RST=*/4);



DFRobot_SHT3x   sht3x;



void setup() {

  Serial.begin(9600);

  //Initialize the chip

  while (sht3x.begin() != 0) {

    Serial.println("Failed to Initialize the chip, please confirm the wire connection");

    delay(1000);

  }

  /**

   * readSerialNumber Read the serial number of the chip.

   * @return Return 32-digit serial number.

   */

  Serial.print("Chip serial number");

  Serial.println(sht3x.readSerialNumber());

  

  /**

   * softReset Send command resets via IIC, enter the chip's default mode single-measure mode, 
   * turn off the heater, and clear the alert of the ALERT pin.

   * @return Read the register status to determine whether the command was executed successfully, 
   * and return true indicates success.

   */

   if(!sht3x.softReset()){

     Serial.println("Failed to Initialize the chip....");

   }

   

  /**

   * heaterEnable(): Turn on the heater inside the chip to enable the sensor get correct humidity value in wet environments.

   * @return Read the status of the register to determine whether the command was executed successfully,
   * and return true indicates success.

   * @note Heaters should be used in wet environments, and other cases of use will result in incorrect readings
   */

  //if(!sht3x.heaterEnable()){

    // Serial.println("Failed to turn on the heater....");

  //}

  Serial.println("------------------Read adta in single measurement mode-----------------------");

}



void loop() {



  Serial.print("Ambient Temperature(°C/F):");

  /**

   * getTemperatureC Get the meansured temperature(℃).

   * @return Return float temperature data.

   */

  Serial.print(sht3x.getTemperatureC());

  Serial.print(" C/");

  /**

   * getTemperatureF:Get the meansured temperature(℉).

   * @return Return float temperature data.
   */

  Serial.print(sht3x.getTemperatureF());

  Serial.print(" F ");

  Serial.print("Relative Humidity(%RH):");

  /**

   * getHumidityRH: Get the meansured humidity (%RH)

   * @return Return float humidity data

   */

  Serial.print(sht3x.getHumidityRH());

  Serial.println(" %RH");

  

  /**
   * @brief Get temperature and humidity data in single measurement mode.
   * @param repeatability Set repeatability to read temperature and humidity data with the type eRepeatability_t.
   * @note  Optional parameters:
               eRepeatability_High /In high repeatability mode, the humidity repeatability is 0.10%RH, the temperature repeatability is 0.06°C
               eRepeatability_Medium,/In medium repeatability mode, the humidity repeatability is 0.15%RH, the temperature repeatability is 0.12°C.
               eRepeatability_Low, /In low repeatability mode, the humidity repeatability is0.25%RH, the temperature repeatability is 0.24°C
   * @return Return a structure containing celsius temperature (°C), Fahrenheit temperature (°F), relative humidity(%RH), status code.
   * @n Return O indicates right data return.

  DFRobot_SHT3x::sRHAndTemp_t data = sht3x.readTemperatureAndHumidity(sht3x.eRepeatability_High);

  if(data.ERR == 0){

    Serial.print("Ambient Temperature(°C/F):");

    Serial.print(data.TemperatureC);

    Serial.print(" C/");

    Serial.print(data.TemperatureF);

    Serial.print(" F ");

    Serial.print("Relative Humidity(%RH):");

    Serial.print(data.Humidity);

    Serial.println(" %RH");

  }

  */

  delay(1000);

}