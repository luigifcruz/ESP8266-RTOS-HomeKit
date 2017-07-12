# ESP8266 RTOS HomeKit Accessory - Development Log

Log #1 - July 11, 2017
As we reach the limits of memory and proccessing power of the ESP8266 these types os problems will get more and more common. Right now, I'm stuck trying to figure out how to prevent the WatchDog Timer from reseting the system after calculating the srp_setA(); (M4 State). I already increase the Stack Size of xTaskCreate. 