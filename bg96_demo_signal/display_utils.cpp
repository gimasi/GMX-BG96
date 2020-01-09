/*
 * 
 */

#include "SeeedOLED.h"

 void centerString(char *string )
{
  String tmpString;
  int len,spaces,i;
  
  len = strlen(string);

  spaces = 0;
  if ( len < 16 ) 
    spaces = ( 16 - len ) / 2;

  for (i=0;i<spaces;i++)
    tmpString = tmpString + " ";
    
  tmpString = tmpString + string;
  tmpString.toCharArray(string, 16);
 
}

void splashScreen() {

  char string[16];
  
  SeeedOled.clearDisplay();   

  SeeedOled.setTextXY(0,0);        
  SeeedOled.putString("** TUINO+BG96 **"); 
  SeeedOled.setTextXY(2,0);  
  
  sprintf(string,"Swisscom LTE");
  centerString( string );
  SeeedOled.putString(string);
  
  SeeedOled.setTextXY(4,0);  
  sprintf(string,"Demo");
  centerString( string );
  SeeedOled.putString(string);
   
  
 }


 void displayGPS(char *lat, char *lon, char *altitude, char *speed, char *cog, char *datetime, float soc ) 
 {

  SeeedOled.clearDisplay();  
  
  SeeedOled.setTextXY(0,0);        
  SeeedOled.putString("Lat: ");
  SeeedOled.putString(lat);
  SeeedOled.setTextXY(1,0);  
  SeeedOled.putString("Lon: "); 
  SeeedOled.putString(lon);  
  SeeedOled.setTextXY(2,0);  
  SeeedOled.putString("Alt: ");
  SeeedOled.putString(altitude);  
  SeeedOled.setTextXY(3,0);  
  SeeedOled.putString("Spd: ");
  SeeedOled.putString(speed);  
  SeeedOled.setTextXY(4,0);  
  SeeedOled.putString("COG: ");
  SeeedOled.putString(cog);  
  SeeedOled.setTextXY(5,0);  
  SeeedOled.putString(datetime);

  SeeedOled.setTextXY(7,0);  
  SeeedOled.putString("Battery: ");
  SeeedOled.putFloat(soc,2);
  SeeedOled.putString("%");

 }


 void displaySignal(int value, int pos, char type[6]) {

  char buf[32];

  sprintf(buf,"%s: %3d",type, value);
  
  SeeedOled.setTextXY(pos,0);    
  SeeedOled.putString(buf); 
  
 }

 void displayTime2TX(long int time_ms) {

  char buf[32];
  int time_sec;

  time_sec = time_ms / 1000;

  sprintf(buf,"Time to TX: %3d",time_sec);
  
  SeeedOled.setTextXY(7,0);    
  SeeedOled.putString(buf); 
  
 }
 void displayTX(bool status) {
  SeeedOled.setTextXY(7,0);    
  if ( status )    
    SeeedOled.putString("  ***RADIO TX*** "); 
  else
    SeeedOled.putString("                "); 
 }


 void displayRX(bool status) {
  SeeedOled.setTextXY(7,0);    
  if ( status )    
    SeeedOled.putString("  ***RADIO RX*** "); 
  else
    SeeedOled.putString("                "); 
 }
 
 void displayLora( ) {

  SeeedOled.clearDisplay();   

 }
