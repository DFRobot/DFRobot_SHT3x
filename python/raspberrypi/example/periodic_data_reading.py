# -*- coding:utf-8 -*-
'''!
   @file periodic_data_reading.py
   @brief Read ambient temperature (C/F) and relative humidity (%RH) in cycle read mode.
   @n Experimental phenomenon: Before we start, please set the read frequency and repeatability of the read
   @n(the difference between the data measured by the chip under the same measurement conditions),
   @n and enter the periodic read mode, and then read the temperature and humidity data.
   @n The temperature and humidity data will be printed at the serial port, after 10 seconds of operation.
   @n It will exit the cycle mode and enter 2 measurement mode: Single measurement mode and Cycle measurement mode.
   @n Single measurement mode: reflect the difference between the two modes of reading data.
   @n Cycle measurement mode: the chip periodically monitors temperature and humidity, only in this mode the ALERT pin will work.
   @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
   @License     The MIT License (MIT)
   @author      [yangfeng]<feng.yang@dfrobot.com>
   @version  V1.0
   @date  2021-06-11
   @url https://github.com/DFRobot/DFRobot_SHT3x
'''
import sys
sys.path.append("../") # set system path to top
import time
from DFRobot_SHT3X import *
SHT3X =  DFRobot_SHT3x(iic_addr = 0x45,bus = 1)

def setup():

  while(SHT3X.begin(RST = 4) != 0):
    print("The initialization of the chip is failed, please confirm whether the chip connection is correct")
    time.sleep(1)

  '''
     readSerialNumber Read the serial number of the chip
     @return Return 32-digit serial number
  '''
  print("The chip serial number = %d "%SHT3X.read_serial_number())

  '''
     softReset Send command resets via iiC, enter the chip's default mode single-measure mode, turn off the heater, 
     and clear the alert of the ALERT pin.
     @return Read the status register to determine whether the command was executed successfully, and returning true indicates success.
  '''
  if(SHT3X.soft_reset() == False):
    print("Failed to reset the chip")


  '''
     @brief Enter cycle measurement mode and set repeatability(the difference between the data measured 
     the difference between the data measured by the chip under the same measurement conditions)
     @param measureFreq: Read the  data frequency
     @n                  measureFreq_0_5Hz = 0
     @n                  measureFreq_1Hz = 1
     @n                  measureFreq_2Hz = 2
     @n                  measureFreq_4Hz = 3
     @n                  measureFreq_10Hz = 4
     @param repeatability: The mode of reading data,repeatability_high in default.
     @n                    repeatability_high = 0    #/**<In high repeatability mode, the humidity repeatability is 0.10%RH, the temperature repeatability is 0.06°C*/
     @n                    repeatability_medium = 1  #/**<In medium repeatability mode, the humidity repeatability is 0.15%RH, the temperature repeatability is 0.12°C*/
     @n                    repeatability_low = 2     #/**<In low repeatability mode, the humidity repeatability is0.25%RH, the temperature repeatability is 0.24°C*/
     @return Return true indicates a successful entrance to cycle measurement mode.
  '''
  if(SHT3X.start_periodic_mode(measure_freq = SHT3X.measureFreq_10Hz,repeatability = SHT3X.repeatability_high) == False):
    print("Failed to enter the periodic mode")

  print("------------------Read data in cycle measurement mode-----------------------")

def loop():
  '''
     @brief Get the measured temperature (in degrees Celsius)
     @return Return the float temperature data 
  '''
  print("environment temperature(°C): %f C"%SHT3X.get_temperature_C())

  '''
     @brief Get the measured temperature (in degrees Fahrenheit)
     @return Return the float temperature data 
  '''
  print("environment temperature(F): %f F"%SHT3X.get_temperature_F())

  '''
     @brief Get measured humidity(%RH)
     @return Return the float humidity data
  '''
  print("relative humidity(%%RH): %f %%RH"%SHT3X.get_humidity_RH())
  time.sleep(1)
if __name__ == "__main__":
  setup()
  while True:
    loop()
