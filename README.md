# Zaluzio Geret
Automated smart blinds retrofit device with WiFi , MQTT and light measurement capability.

![](https://media.giphy.com/media/z4RIFCi5MSZSXSOJpL/giphy.gif)

## Bill of material
* [Wemos D1 mini](http://s.click.aliexpress.com/e/b7xoMFF6) 
* [180 degree servo](http://s.click.aliexpress.com/e/cCIS5ETW)
* [LDR 5516 photoresistor](http://s.click.aliexpress.com/e/sKHeBEs)
* [NPN Transistor](http://s.click.aliexpress.com/e/bSLbOB7K)
* [Tactile push buttons](http://s.click.aliexpress.com/e/0rbMNuk)
* [10k Resistors](http://s.click.aliexpress.com/e/boawi0Ak)
* Some spring, hard to specify dimensions and power
* Some basic soldering equipment and skills

## Settings
You can redefine some default settings through Settings.h file. It is pretty self-explanatory I think.

## Usage
There is WiFi Manager on board so after burning the firmware, device is in AP mode, default password for connecting to AP is: configure.
Default account for configuring / controlling the device is admin with password: password

After connecting to WiFi, there is UI configuration interface, you can find device IP address through serial terminal or through in your router, or through scanning the network. Device hostname is ZaluzioGeret-MAC_END.

There are settings for specifiing MIN and MAX angle of rotation, and reverse direction of inner logic.

There are 2 buttons connected, UP and DOWN, each short press control the servo with 5° in respective direction. Short pressing of both buttons resets the servo in middle position. 
Long press of each button moves the servo to the MIN or MAX position defined. Long pressing of both buttons starts the LightFinder procedure. 
LightFinder procedure moves the servo to MIN position, and then continually measure light after each move. Then it move to the position in which highest light was measured. 

Long pressing of both buttons for 8 seconds resets the WiFi settings, putting the device into AP configuration mode. 

## Slave device controll
There is option in settings to setup Slave device ID which is automatically generated hostname which you can see in top of home screen. I assume using identical settings of PUB and SUB topic prefixes and also angles. 

When slave device ID is set, after pressing button, MQTT command will be sent to slave device and thus linking these together.

If you want to controll multiple devices remotely, not through buttons, you can easily done it in Home automation system, for example automation would have 2 actions, send command to 2 different topics. 

## MQTT Commands
```
  MQTTPUBTOPIC/hostname/LWT - Where status of device is sent 
  MQTTSUBTOPIC/hostname/Rotate  - Command for controlling angle of servo number expected
  MQTTSUBTOPIC/hostname/FindLight - Command for start LightFinder procedure. On expected
  MQTTPUBTOPIC/hostname/Light - Here is light measured sent
  MQTTPUBTOPIC/hostname/Angle - Here is current angle of servo sent
```

## API call / rest command
This call will start LightFinder procedure. 
You have to be authenticated or send basic auth with request.
``` 
ip.of.dev.ice/lightcontrol
```
This call will move servo to desired angle. 
You have to be authenticated or send basic auth with request.
``` 
ip.of.dev.ice/servocontrol?angle=XX
```
## Wiring scheme
![Wiring scheme](https://github.com/Luc3as/ZaluzioGeret/blob/master/PCB/PCB%20sketch_bb.png?raw=true)

## Physical installation
Device is mounted to window frame by self adhesive tape by 3M.

For cable routing I used sticky cable trunking with name MONTERA. 

All 3D source files will be uploaded to thingiverse.com

![](https://i.ibb.co/72Cv8DY/servo-verzia-lava1.png)
![](https://i.ibb.co/3ztyH5z/servo-verzia-lava.png)
![](https://i.ibb.co/2h6PdQr/IMG-20190520-203901.jpg)
![](https://i.ibb.co/PDZDb6D/IMG-20190520-203852.jpg)
![](https://i.ibb.co/RzhmBQ9/IMG-20190520-194750.jpg)
