
#include <Wire.h>
#include <Time.h>

#include "gmx_bg96.h"
#include "SeeedOLED.h"
#include "display_utils.h"
#include "MAX17048.h"

#include "at_client.h"
#include "gmx_bg96.h"

#define DEBUG 1

void _debug_init()
{
  Serial.begin(9600);
}
void _debug(char *string )
{
  Serial.println(string);
}


time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss)
{
  struct tm datetime;
  
  datetime.tm_year = YYYY - 1900 + 30;
  datetime.tm_mon = MM - 1;
  datetime.tm_mday = DD;
  datetime.tm_hour = hh;
  datetime.tm_min = mm;
  datetime.tm_sec = ss;
  Serial.println( datetime.tm_year );
  Serial.println( datetime.tm_mon );
  Serial.println( datetime.tm_mday );
  
  Serial.println( datetime.tm_hour );
  Serial.println( datetime.tm_min );
  Serial.println( datetime.tm_sec );
  
  return mktime(&datetime);         //convert to time_t
}


unsigned char IMEI[32];
unsigned char IMSI[32];
unsigned char NB[128];
bool network_attached;

long int timer_period_to_tx = 20000;
long int timer_millis_tx = 0;

unsigned char devAddress[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char SendBuffer[256];

//char confAPN[]="shared.m2m.ch";
char confAPN[]="";

char op_code[]="22801";

char confIP[]="5.79.89.1";
char confPort[]="9200";


int join_wait;

/* 
 *  Utility
 */

void convertHexToByte(char *hex_string, uint8_t *binary_data )
{
    int len,i;

    len = strlen(hex_string);
    for(i=0;i<len/2;i++)
    {
      uint8_t high,low;
      
      high = (hex_string[i*2]-0x30) * 16;
      low =  (hex_string[(i*2)+1]-0x30);
        
      binary_data[i] =  high + low;
    }

}
/*
 *  Temp sensor settings and used variables
 */

#define TEMPERATURE_SAMPLING    (20)

int tempSensorPin = A0;
float current_temperature = 0.0f;
float temp_temperature = 0;
int temperature_counts = 0;

// Temperature Sensor constants, use these to calibrate
const int B = 4275;             // B value of the thermistor

//temperature reading and converting function
float readTemp() {
  int a = analogRead(tempSensorPin );
  float R = 1023.0 / ((float)a) - 1.0;
  R = 100000.0 * R;
  
  //convert to temperature via datasheet
  float temperature = 1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15; 
  return temperature;
}


void setup() {

    char tmp_string[32];
    String string1,string2;
    unsigned char imei_buffer[32];
   
    // put your setup code here, to run once:

    Wire.begin();

 
    // Init Oled
    SeeedOled.init();  //initialze SEEED OLED display

    SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
    SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
    SeeedOled.setHorizontalMode(); 
    
    _debug_init();
    _debug("Starting");

    splashScreen();
    delay(2000);

    string1 = confIP;
    string2 = confPort;

    // We init the BG96
    if ( gmxBG96_init(string1,string2) ==  0 )
     _debug("BG96 OK!");


    // READ IMEI and IMSI
    // We need IMEI as the device address
    if ( gmxBG96_getIMEI( IMEI, 32 ) == 0 )
    {
      
      _debug("IMEI_OK");
      _debug(IMEI);

       sprintf(imei_buffer,"0%s",IMEI);
 
      for (int i=0;i<8;i++)
      {
        uint8_t high,low;

        high = (imei_buffer[i*2]-0x30) * 16;
        low =  (imei_buffer[(i*2)+1]-0x30);

       devAddress[i] = high + low;
      }

    } 

    if ( gmxBG96_getIMSI( IMSI, 32 ) == 0 )
    {
      _debug("IMSI_OK");
      _debug(IMSI);
    } 
    else
    {
      _debug("IMSI_KO");
    }


   /* HERE YOU CAN SELECT DIFFERENT RADIO TECHNOLOGIES */
   // if (  gmxBG96_setGSM(confAPN) == 0 ) 
    // 80000 is band 20 
    if ( gmxBG96_setNBIoT("swisscom-test.m2m.ch","80000") == 0) 
   //if ( gmxBG96_setCatM1(confAPN) == 0 )
    {
      _debug("Access_Tech_OK");
    } 
    else
    {
      _debug("Access_Tech_KO");
    }
    

    if (  gmxBG96_GpsOn() == 0 ) 
      _debug("GPSON_OK");
    else
      _debug("GPSON_KO");

    /*
     *  Display Configuration
     */
    Serial.println("*** Configuration ***");

    SeeedOled.clearDisplay(); 
      
    SeeedOled.setTextXY(0,0);        
    SeeedOled.putString(imei_buffer); 
    
    SeeedOled.setTextXY(2,0);  
    sprintf(tmp_string,"%s",confIP);
    Serial.println(tmp_string );
    centerString( tmp_string );
    SeeedOled.putString(tmp_string);

    SeeedOled.setTextXY(4,0);  
    sprintf(tmp_string,"%s",confPort);
    Serial.println(tmp_string );
    centerString( tmp_string );
    SeeedOled.putString(tmp_string);

    SeeedOled.setTextXY(6,0);  
    sprintf(tmp_string,"%s",confAPN);
    Serial.println(tmp_string );
    centerString( tmp_string );
    SeeedOled.putString(tmp_string);
    
    delay(3000);
    
    network_attached = false;

    SeeedOled.clearDisplay(); 
    SeeedOled.setTextXY(0,0);        
    SeeedOled.putString("Attaching.."); 

  
    if (strlen(op_code)> 0 )
    {
      Serial.print("OP Code ");
      Serial.println(op_code);
      if ( gmxBG96_setOperator(op_code) == 0)
      {
        Serial.print("Operator Set:");
        Serial.println(op_code);
      }
    }
    else
      Serial.println("No OpCode");
      
    if ( gmxBG96_attach() == 0)
      Serial.println("Attaching to Network");

    // Init Temperature
    current_temperature = readTemp();

    bool attach_flag = true;
    while(attach_flag)
    {
      int attach_status;
      
      gmxBG96_isNetworkAttached(&attach_status);
      if (( attach_status == 1 ) || ( attach_status == 5 ))
      {
        Serial.println("Attached");
        attach_flag =false;
      } 
      else
      {
       Serial.println("Waiting for Attach");
       delay(3000);
      }
    }
}


void loop() {
  // put your main code here, to run repeatedly:
  int network_status;
  int rssi,ber;
  long int delta_tx;
  char lat[16];
  char lon[16];
  char cog[16];
  char alt[16];
  char speed[16];
  int hour,minute,seconds;
  int day,month,year;
  char datetime[64];
  char gps_data[256];
  char tmp_string[32];
  int temperature_int;

  uint8_t rxdata[128];
  char rxbuffer[256];
  int rx_received_len;
 
  time_t timestamp;
  
  delta_tx = millis() - timer_millis_tx;
  

  if ((( delta_tx > timer_period_to_tx) || (timer_millis_tx == 0 )) && network_attached )
  {
    if ( gmxBG96_isNetworkAttachedStatus() )
    {
      network_attached = true;
    
      //multiply the temperature with 100 to avoid sending as float value
      temperature_int = current_temperature * 100;
  
      char buffering[128];
    
      // Device Address
      for(int i=0;i<8;i++)
       buffering[i]=devAddress[i];

      // This is the payload lenght
      buffering[8]= 3;

      // Here we start the payload
      buffering[9] = 0x01;
      buffering[10] = (temperature_int & 0xff00 ) >> 8;
      buffering[11] = temperature_int & 0x00ff;
  
      Serial.println("TX DATA");
              
      if ( gmxBG96_SocketOpen()==0)
        Serial.println("Socket Opened");
      else
        Serial.println("Socket Error");
            
                
      if ( gmxBG96_TXData(buffering,13) == 0 )
        Serial.println("Send Ok!");
      else
        Serial.println("Send KO!");   
      
      // Wait for RX 
      
      if (  gmxBG96_RXData(  rxbuffer, 255, &rx_received_len ) == GMX_BG96_OK )
      {
       Serial.println("Receieved");
       Serial.println(rxbuffer);

       // Convert to Byte
       convertHexToByte( rxbuffer, rxdata );

       if ( rxdata[0] == 0x00 )
          digitalWrite(GMX_BG96_LED1_PIN,LOW);
          
       if ( rxdata[0] == 0x01 )
          digitalWrite(GMX_BG96_LED1_PIN,HIGH);
       
       if ( rxdata[1] == 0x01 )
          digitalWrite(GMX_BG96_LED2_PIN,HIGH);

       if ( rxdata[1] == 0x00 )
         digitalWrite(GMX_BG96_LED2_PIN,LOW);
       
      }

      if ( gmxBG96_SocketClose()==0)
        Serial.println("Socket Opened");
      else
        Serial.println("Socket Error");
    }
    else
    {
      Serial.print("Trying to reattach");
      network_attached = false;

      // force a reattach
      if ( gmxBG96_attach() == 0)
        network_attached = true;
    }
  }

  if ( network_attached == false )
  {
    if (   gmxBG96_isNetworkAttachedStatus() )
    {
      network_attached = true;
        
      Serial.println("Attached!");
        
      SeeedOled.setTextXY(2, 0);
      SeeedOled.putString("Attached!");
        
      delay(2000);
      SeeedOled.clearDisplay();
    }
    else
    {
      Serial.println("Not Attached!");
        
      SeeedOled.setTextXY(1, 0);
      sprintf(tmp_string, "Attempt: %d", join_wait);
      SeeedOled.putString(tmp_string);

      join_wait++;

      if ( gmxBG96_attach() == 0)
        Serial.println("Attaching to Network");
    
    }

    delay(5000);
  }
  
  // update temperature
  // we sample and make an average of the temperature - since in this demo we use a NTC sensor that fluctuates
  temp_temperature += readTemp();
  temperature_counts ++;

  if ( temperature_counts >= TEMPERATURE_SAMPLING )
  {
    current_temperature = temp_temperature / TEMPERATURE_SAMPLING;

    temperature_counts = 0;
    temp_temperature = 0;
  }

}

