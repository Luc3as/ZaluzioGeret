# Zaluzio Geret
Automated smart blinds retrofit device with WiFi , MQTT and light measurement capability.

## Bill of material
* [Wemos D1 mini](https://www.aliexpress.com/item/ESP8266-ESP12-ESP-12-WeMos-D1-Mini-WIFI-Dev-Kit-Development-Board-NodeMCU-Lua/32653918483.html?ws_ab_test=searchweb0_0,searchweb201602_4_10152_10151_10065_10068_10344_10342_10343_10340_10341_10696_10084_10083_10618_10307_10820_10821_10303_10846_10059_100031_524_10103_10624_10623_10622_10621_10620,searchweb201603_25,ppcSwitch_5&algo_expid=7c525067-46ab-4a80-baab-f36e6cb5dd58-0&algo_pvid=7c525067-46ab-4a80-baab-f36e6cb5dd58&transAbTest=ae803_1&priceBeautifyAB=0) 
* [180 degree servo](https://www.aliexpress.com/item/HAILANGNIAO-1pcs-lot-MG995-55g-servos-Digital-Metal-Gear-rc-car-robot-Servo-The-rotation-is/32869652979.html?spm=a2g0s.9042311.0.0.27424c4dIOCw13)
* [LDR photoresistor](https://www.aliexpress.com/item/20pcs-Photo-Light-Sensitive-Resistor-Photoresistor-Optoresistor-5mm-GL5528-5528/32760631393.html?spm=a2g0s.9042311.0.0.27424c4d1pNKxh)
* [NPN Transistor](https://www.aliexpress.com/item/10pcs-lot-Original-TIP120-120-TIP-TO-220-NPN-TIP-120-Original-Best-Quality/32868965838.html?spm=a2g0s.9042311.0.0.27424c4dcDvkOk)
* [Tactile push buttons](https://www.aliexpress.com/item/100pcs-lot-Mini-Micro-Momentary-Tactile-Push-Button-Switch-4-pin-ON-OFF-keys-button-DIP/32987958978.html?spm=2114.search0604.3.7.25b56455ZFHSgc&s=p&ws_ab_test=searchweb0_0,searchweb201602_9_10065_10068_319_10059_10884_317_10887_10696_321_322_10084_453_10083_454_10103_10618_10307_10820_10821_10303_537_536,searchweb201603_16,ppcSwitch_0&algo_expid=60950a20-5326-4de1-b878-4711bb04b38d-1&algo_pvid=60950a20-5326-4de1-b878-4711bb04b38d)
* [10k Resistors](https://www.aliexpress.com/item/100pcs-1-4W-1R-22M-1-Metal-film-resistor-100R-220R-1K-1-5K-2-2K/32847096736.html?spm=2114.search0604.3.1.35c636baRat6rk&s=p&ws_ab_test=searchweb0_0,searchweb201602_9_10065_10068_319_10059_10884_317_10887_10696_321_322_10084_453_10083_454_10103_10618_10307_10820_10821_10303_537_536,searchweb201603_16,ppcSwitch_0&algo_expid=ef55fbd0-71f9-4acd-bb67-d94bdb1d6613-0&algo_pvid=ef55fbd0-71f9-4acd-bb67-d94bdb1d6613)
* Some spring, hard to specify dimensions and power
* Some basic soldering equipment and skills

## Settings
You can redefine some default settings through Settings.h file. It is pretty self-explanatory I think.

## Usage
There is WiFi Manager on board so after burning the firmware, device is in AP mode, default password for connecting to AP is: configure.

After connecting to WiFi, there is UI configuration interface, you can find device IP address through serial terminal or through in your router, or through scanning the network. Device hostname is ZaluzioGeret-MAC_END.

There are settings for specifiing MIN and MAX angle of rotation, and reverse direction of inner logic.

There are 2 buttons connected, UP and DOWN, each short press control the servo with 5° in respective direction. Short pressing of both buttons resets the servo in middle position. 
Long press of each button moves the servo to the MIN or MAX position defined. Long pressing of both buttons starts the LightFinder procedure. LightFinder procedure moves the servo to MIN position, and then continually measure light after each move. Then it move to the position in which highest light was measured. 

## Wiring scheme
TO BE DONE

## Physical installation
Device is mounted to window frame by self adhesive tape by 3M.