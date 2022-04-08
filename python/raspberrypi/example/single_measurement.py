# -*- coding:utf-8 -*-
'''!
   @file single_measurement.py
   @brief Read ambient temperature (C/F) and relative humidity (%RH) in single-read mode.
   @n Experimental phenomenon: the chip defaults in this mode, we need to send instructions to enable the chip collect data,
   @n which means the repeatability of the read needs to be set (the difference between the data measured by the chip under the same measurement conditions)
   @n then read the temperature and humidity data and print the data in the serial port.
   @n Single measure mode: read data as needed, power consumption is relatively low, the chip idle state only costs 0.5mA. 
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

  print("------------------Read data in single measurement mode-----------------------")

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
