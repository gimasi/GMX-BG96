# GMX_BG96
The GMX-BG96 is a Quectel BG96 based GMX module for the Tuino plaform.
<br/>
<img src="/docs/bg96_1.png"/>
<br/>
<br/>

The board has 2 uFL connectors for both the Radio antenna and the GNSS antenna ( passive ), since the BG96 comes with an internal GNSS. Another neat feature is the presence of the USB port so that you can access directly the module ( which is very usefull when debugging AT command sequences).<br/>
There 3 led's, the first 2 ( red and green) are drivable via arduino code, while the third led ( blue ) is directly connected to the BG96 and signals the activity of the radio network.<br/>
Finally we have also added 2 microswitches to turn on and to reset the module. This can be useful if you want to use the board standalone ( but you need to supply 3.3V to it) and use it connected directly via the on board USB.<br/>

# USB Drivers and Firmware update.
To install USB drivers to access the BG96 as well as the firmware update tool please refere to the Tuino 096 [repository](https://github.com/gimasi/TUINO_ZERO_96) in the BG96_firmware_update directory. 

# Arudino Library
... coming soon ...
