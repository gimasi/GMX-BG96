# GMX_BG96
The GMX-BG96 is a Quectel BG96 based GMX module for the Tuino plaform.<br/>
<br/>
<img src="/docs/bg96_1.jpg"/>
<br/>
<br/>

The board has 2 uFL connectors for both the Radio antenna and the GNSS antenna ( passive ), since the BG96 comes with an internal GNSS. Another neat feature is the presence of the USB port so that you can access directly the module ( which is very usefull when debugging AT command sequences).<br/>
There 3 led's, the first 2 ( red and green) are drivable via arduino code, while the third led ( blue ) is directly connected to the BG96 and signals the activity of the radio network.<br/>
Finally we have also added 2 microswitches to turn on and to reset the module. This can be useful if you want to use the board standalone ( but you need to supply 3.3V to it) and use it connected directly via the on board USB.<br/>

# USB Drivers and Firmware update.
To install USB drivers to access the BG96 as well as the firmware update tool please refere to the Tuino 096 [repository](https://github.com/gimasi/TUINO_ZERO_96) in the BG96_firmware_update directory. 

# Schematics
These are the schematics of the module: [gmx_bg96_rev3.pdf](gmx_bg96_rev3.pdf)

# Simple Tuino Script
Here a first basic example that let you control the BG96 via the Tuino Serial port. 
```c
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  Serial.println("Starting...");
  
  if(Serial1) {
  Serial1.begin(115200);
  } 
        
  // RESET
  pinMode(GMX_RESET,OUTPUT);
  digitalWrite(GMX_RESET,LOW);

  // Boot
  // LED1
  pinMode(GMX_GPIO1,OUTPUT);
  // LED2
  pinMode(GMX_GPIO2,OUTPUT);
  // BG96 STATUS PIN
  pinMode(GMX_GPIO3,INPUT);
   // BG96 AP_READY
  pinMode(GMX_GPIO4,OUTPUT);
  // BG96 DTR
  pinMode(GMX_GPIO5,OUTPUT);
  //BG96 POWERKEY
  pinMode(GMX_GPIO6,OUTPUT); 

  digitalWrite(GMX_GPIO1,LOW);
  digitalWrite(GMX_GPIO2,LOW);
  digitalWrite(GMX_GPIO4,HIGH);
  digitalWrite(GMX_GPIO5,HIGH);
  digitalWrite(GMX_GPIO6,LOW);


  // Reset
  
  digitalWrite(GMX_RESET,LOW);
  delay(100);
  digitalWrite(GMX_RESET,HIGH);
  delay(200);
  digitalWrite(GMX_RESET,LOW);
  delay(200);
  
 
  // Power On
 
  digitalWrite(GMX_GPIO6,HIGH);
  delay(800);
  digitalWrite(GMX_GPIO6,LOW);
  delay(100);
 
}

void loop() {
  // put your main code here, to run repeatedly:
  byte ch;

  if (Serial1.available()>0) 
  {
    Serial.write(Serial1.read());
  }

  if (Serial.available()>0) 
  {
    ch = Serial.read();
    Serial1.write(ch);
  }
  
}

```
You can find this script in the gmx-bg96-proxy directory.<br/>
<br>

# Simple Demo
In the [bg96_demo](bg96_demo) directory you can find a simple script that uses a basic gmx_bg96 library for interacting with the BG96.<br/>
The demo consists in reading an analog temperature sensor, connected to the A0 port ( GROOVE Connector) and transmitting the data over to the cloud via a simple UDP socket connection.<br/>
The demo is configured to send data to our test cloud system ( please send us an email if you want an access ). In addition in the demo we are also using an OLED Display, connected to the I2C port ( always on a GROOVE Connector ) that displays some additional data.<br/>
The code also handles simple downlinks that can trigger the LED on the GMX-BG96 module ON and OFF.<br/>The LEDS are mapped onto 2 bytes, setting the byte to 0 turns the led off, setting it to 1 turns the led ON.


# GMX-BG96 Library 
In the Simple Demo you will find also the 'core' libraries that will help you use the BG96 module.<br/>
The files needed are:
* gmx_bg96.cpp
* gmx_bg96.h
* at_client.cpp
* at_client.h
<br/>
<br/>
The libraries gives a basic set of functions to setup the BG96 for the different Radio Technologies: 2G, LTE-NB1, LTE-CATM1 it supports. Attach to the network and send and receive data via UDP Sockets.<br/>
<br>
All function returns success or failure via the values:
* GMX_BG96_OK  => success
* GMX_BG96_KO  => failure
<br>
<br>
Here the Functions present in the library:<br/>

```c
int8_t gmxBG96_init(String ip, String port);
```
Powers up and initializes the BG96. The function expects  <b>ip</b> and <b>port</b> as input parameters for the UDP endpoint to which data will be sent.
<br/>
```c
uint8_t gmxBG96_getIMEI(char *data, uint8_t max_size);
```
Gets the IMEI of the BG96 returning it in <b>data</b>, <b>max_size</b> is the data buffer len. 
<br/>

```c
uint8_t gmxBG96_getIMSI(char *data, uint8_t max_size);
```
Gets the IMSI of the SIM returning it in <b>data</b>, <b>max_size</b> is the data buffer len. 

```c
uint8_t gmxBG96_setGSM(char *apn);
```
Setup the BG96 for GSM connection, <b>apn</b> is the APN of the operator that will be selected.

```c
uint8_t gmxBG96_setNBIoT(char *apn, char *band);
```
Setup the BG96 for NBIOT connection, <b>apn</b> is the APN of the operator that will be selected.<br>
Check in the BG96 for the <b>band</b> parametes to select which NB1 band to use. For example B20 is 80000.<br/>

```c
uint8_t gmxBG96_setCatM1(char *apn);
```
Setup the BG96 for CATM1 connection, <b>apn</b> is the APN of the operator that will be selected.<br>

```c
uint8_t gmxBG96_setOperator( char *op_code);
```
Sets the operator code <b>op_code</b>.This command can take some time to finish.<br/>

```c
uint8_t gmxBG96_attach();
```
Attaches to the network.<br/>This command can take some time to finish.<br/>

```c
uint8_t gmxBG96_isNetworkAttached(int *attach_status);
```
Returns in <b>attach_status</b> the state of the network attach.<br>
Possible values are:
* 0 -> Not trying to register
* 1 -> Attached Home Network
* 2 -> Trying to register to network
* 3 -> Attach denied
* 5 -> Attached Roaming Network
<br/>

```c
bool gmxBG96_isNetworkAttachedStatus(void);
```
This function simply returns <b>true</b> or <b>false</b> if the network is attached.


```c
uint8_t gmxBG96_SocketOpen();
```
Open the Socket.<br/>

```c
uint8_t gmxBG96_SocketClose();
```
Close the Socket.<br/>

```c
uint8_t gmxBG96_TXData(char *data, int data_len);
```
Transmits the binary buffer <b>data</b> of size <b>data_len</b> via UDP socket.

```c
uint8_t gmxBG96_RXData(char *rx_buffer, int rx_buffer_len, int *rx_received_len );
```
Checks if data is available in the RX buffer. Parameters are <b>rx_buffer<b/> which will store the received Hex string, <b>rx_buffer_len</b> which is the maximum length for rx_buffer and <b>rx_received_len<b/> which will return the actual bytes received.


```c
uint8_t gmxBG96_GpsOn(void);
```
Turns On GPS.
```c
uint8_t gmxBG96_GpsOff(void);
```
Turns Off GPS.

```c
uint8_t gmxBG96_GpsLoc(char *lat, char *lon, char *alt, char *cog, char *speed, int* hour, int* minute, int* second, int* day, int* month, int* year ); 
```
Get GPS Fix.
