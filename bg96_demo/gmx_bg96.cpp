
#include "at_client.h"
#include "gmx_bg96.h"


String udp_ip;
String udp_port;

void _log(String data )
{
  #ifdef DEBUG
    Serial.println(data);
  #endif
}

void _resetGMX()
{

  pinMode(GMX_BG96_RESET_PIN,OUTPUT);
  
  // Reset 
  digitalWrite(GMX_BG96_RESET_PIN,LOW);
  delay(200);
  digitalWrite(GMX_BG96_RESET_PIN,HIGH);
  delay(500);
  digitalWrite(GMX_BG96_RESET_PIN,LOW);
}

void _BG96PowerON()
{

  if ( digitalRead(GMX_BG96_STATUS) == 0 )
  {
  // 
  digitalWrite(GMX_BG96_PWRKEY_PIN,LOW);
  delay(500);
  digitalWrite(GMX_BG96_PWRKEY_PIN,HIGH);
  delay(500);
  digitalWrite(GMX_BG96_PWRKEY_PIN,LOW);
  }
  else
  {
    _resetGMX();
  }
}


void _GPIOInit()
{

    pinMode(GMX_BG96_PWRKEY_PIN,OUTPUT);
    pinMode(GMX_BG96_LED1_PIN,OUTPUT);
    pinMode(GMX_BG96_LED2_PIN,OUTPUT);
    pinMode(GMX_BG96_STATUS,INPUT);

    // RESET PIN SETUP
    pinMode(GMX_RESET,OUTPUT);
    digitalWrite(GMX_RESET,LOW);
    
    pinMode(GMX_BG96_PWRKEY_PIN,LOW);
    
}

void _LedBootFlash()
{
    digitalWrite(GMX_BG96_LED1_PIN,LOW);
    digitalWrite(GMX_BG96_LED2_PIN,LOW);

  
    digitalWrite(GMX_BG96_LED1_PIN,HIGH);
    delay(500);
    digitalWrite(GMX_BG96_LED1_PIN,LOW);
        
    digitalWrite(GMX_BG96_LED2_PIN,HIGH);
    delay(500);
    digitalWrite(GMX_BG96_LED2_PIN,LOW);
  
    
    digitalWrite(GMX_BG96_LED1_PIN,HIGH);
    delay(500);
    digitalWrite(GMX_BG96_LED1_PIN,LOW);
    
    digitalWrite(GMX_BG96_LED2_PIN,HIGH);
    delay(500);
    digitalWrite(GMX_BG96_LED2_PIN,LOW);
  
  
}

uint8_t gmxBG96_init(String _ip, String _port) 
{

   
    _log("BG96 Init");

    udp_ip = _ip;
    udp_port = _port;

    // Init AT Interface
    at_serial_init(115200);
   
    _GPIOInit();
    _LedBootFlash();
    _BG96PowerON();
    
     delay(500);
   
    if ( at_read_for_response_single("RDY",BG96_BOOT_TIMEOUT) != 0 )
    {
      return GMX_BG96_KO;
    }

    _log("Boot OK !!!");

    at_send_command("ATE0");

    if ( at_read_for_response_single("OK",BG96_DEFAULT_TIMEOUT) != 0 )
    {
      _log("ERROR");
      return GMX_BG96_KO;
    }

    _log("AT OK !!!");

     // Delay for SIM reading
     delay(1000);
     
     return GMX_BG96_OK;
}

void _strip_all_newlines(char *data )
{
  
  String _temp= String(data);

  _temp.replace("\n","");
  _temp.replace("\r","");
  
  _temp.toCharArray(data,_temp.length()+1);

}

void _strip_all_spaces(char *data )
{
  
  String _temp= String(data);

  _temp.trim(); 
  _temp.toCharArray(data,_temp.length()+1);

}



uint8_t gmxBG96_getIMEI(char *data, uint8_t max_size)
{
  
  at_send_command("AT+GSN");

  if ( at_copy_serial_to_buffer(data, 'O', max_size,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;
  
   _strip_all_newlines( data );
   
  return GMX_BG96_OK;
}

uint8_t gmxBG96_getIMSI(char *data, uint8_t max_size)
{
  
  at_send_command("AT+CIMI");


  if ( at_copy_serial_to_buffer(data, 'O', max_size,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;

  _strip_all_newlines( data );
  
  
  return GMX_BG96_OK;
}

uint8_t gmxBG96_setGSM(char *apn)
{
  char cgdcont[128];

  at_send_command("AT+QCFG=\"nwscanseq\",01,1");
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO;    


  at_send_command("AT+QCFG=\"nwscanmode\",1,1");
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO;    
  
  sprintf(cgdcont,"AT+CGDCONT=1,\"IP\",\"%s\"",apn);

  at_send_command(cgdcont);
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO;    


  sprintf(cgdcont,"AT+QICSGP=1,1,\"%s\",\"\",\"\",1",apn);
  at_send_command(cgdcont);
  
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO;   
    
  return GMX_BG96_OK;
}

uint8_t gmxBG96_setNBIoT(char *apn, char *band)
{
  char cgdcont[128];
  char qicsgp[128];
  char bandcmd[128];


  at_send_command("AT+QCFG=\"nwscanseq\",03,1");
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO; 

  at_send_command("AT+QCFG=\"nwscanmode\",3,1");
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO; 

  at_send_command("AT+QCFG=\"iotopmode\",1,1");
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO; 

  sprintf(bandcmd,"AT+QCFG=\"band\",0,0,%s,1",band);
  at_send_command(bandcmd);
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO; 

  sprintf(cgdcont,"AT+CGDCONT=1,\"IP\",\"%s\"",apn);
  at_send_command(cgdcont);
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO; 

  sprintf(qicsgp,"AT+QICSGP=1,1,\"%s\",\"\",\"\",1",apn);
  at_send_command(qicsgp);
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO; 


 
  return GMX_BG96_OK;
}


uint8_t gmxBG96_setOperator( char *op_code )
{

  char temp_string[128];

  sprintf(temp_string,"AT+COPS=1,2,%s",op_code);
 
  at_send_command(temp_string);
  if ( read_for_responses_dual("OK","ERROR",BG96_ATTACH_TIMEOUT) != 0 )
    return  GMX_BG96_KO;    

  return GMX_BG96_OK;

}

uint8_t gmxBG96_setCatM1(char *apn)
{
  char cgdcont[128];
  char qicsgp[128];

  at_send_command("AT+QCFG=\"nwscanseq\",02,1");
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO;    

 at_send_command("AT+QCFG=\"nwscanmode\",3,1");
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO; 

  at_send_command("AT+QCFG=\"iotopmode\",0,1");
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO;    

  sprintf(cgdcont,"AT+CGDCONT=1,\"IP\",\"%s\"",apn);
  at_send_command(cgdcont);
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO; 


  sprintf(qicsgp,"AT+QICSGP=1,1,\"%s\",\"\",\"\",1",apn);
  at_send_command(qicsgp);  
  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO; 
    
  return GMX_BG96_OK;
}



uint8_t gmxBG96_attach(void)
{
  char data[16];
  int network_status;
  
  at_send_command("AT+CGATT=1");
   
  if ( read_for_responses_dual("OK","ERROR",BG96_ATTACH_TIMEOUT) != 0 )
    return  GMX_BG96_KO;  

  return GMX_BG96_OK;
}

uint8_t gmxBG96_isNetworkAttached(int *attach_status)
{
 
  at_send_command("AT+CGATT?");

  *attach_status = 0;

  if ( read_for_responses_dual("1","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO;

  *attach_status = 1;

  return GMX_BG96_OK;
}

bool gmxBG96_isNetworkAttachedStatus()
{  
  at_send_command("AT+CGATT?");
  if ( read_for_responses_dual("1","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  false;

  return true;
 }


uint8_t gmxBG96_RXData(  char *rx_buffer, int rx_buffer_len, int *rx_received_len )
{
    uint32_t conn_id;
    char tmp[64];
    char tmp_data[32];
  
    conn_id = 0;

    /* Data Received? */
    if ( at_read_for_response_single("+QIURC: \"recv\",",15000) != 0 )
      return  GMX_BG96_KO; 
      
    delay(500);

    /* Request Data */
    sprintf(tmp,"AT+QIRD=%d,512",conn_id);
    at_send_command(tmp);
    
    if ( read_for_responses_dual("+QIRD: ","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
      return  GMX_BG96_KO; 
    
    
    if ( at_copy_serial_to_buffer_match(tmp_data, "\r\n", 16,  3*BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
      return GMX_BG96_KO;
    
    _strip_all_newlines( tmp_data );
    *rx_received_len = atoi( tmp_data );
    
  
    /* Read Received Data */
    if ( at_copy_serial_to_buffer_match(rx_buffer, "OK", rx_buffer_len,  3*BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    {
      *rx_received_len = -1;
      return GMX_BG96_KO;
    }

    _strip_all_newlines( rx_buffer );

    
      return GMX_BG96_OK;
}

    


uint8_t gmxBG96_signalQuality(int *rssi, int *ber )
{
  char data[16];
  char *pch;
  
  at_send_command("AT+CSQ");

  memset(data,0,sizeof(data));
  if ( at_copy_serial_to_buffer(data, ':', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;

  memset(data,0,sizeof(data));
  if ( at_copy_serial_to_buffer(data, '\r', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;
    
  _strip_all_newlines( data );

  // we have 2 items
  pch = strtok (data,",");
  
  if (pch!=NULL)
  {
    *rssi = atoi( pch );
    pch = strtok (NULL,",");

    if ( pch != NULL )
    {
      *ber = atoi( pch );

      return GMX_BG96_OK;
    }
  }

   return  GMX_BG96_KO; 
}



uint8_t gmxBG96_SocketOpen(void)
{
  char at_string[128];
  char port[64];
  char ip[64];

  udp_ip.toCharArray( ip, udp_ip.length()+1);
  udp_port.toCharArray( port, udp_port.length()+1 );
  
  
  sprintf(at_string,"AT+QIOPEN=1,0,\"UDP\",\"%s\",%s,0,0\r",ip,port);

  at_send_command(at_string);

  if ( at_read_for_response_single("+QIOPEN: 0,0",10000) != 0 )
        return  GMX_BG96_KO;   
 
  return GMX_BG96_OK;
}


uint8_t gmxBG96_SocketClose(void)
{

  at_send_command("AT+QICLOSE=0");

  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO;    
 
 
  return GMX_BG96_OK;
}


uint8_t gmxBG96_GpsOn(void){

  at_send_command("AT+QGPS=1");

  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO;    
 
 
  return GMX_BG96_OK;

}

uint8_t gmxBG96_GpsOff(void){

  at_send_command("AT+QGPSEND");

  if ( read_for_responses_dual("OK","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO;    
 
 
  return GMX_BG96_OK;

}

uint8_t gmxBG96_GpsLoc(char *lat, char *lon, char *alt, char *cog, char *speed, int *hour, int *minute, int *seconds, int *day, int *month, int *year ) 
{

  at_send_command("AT+QGPSLOC=2");
  char time[16];
  char hdop[16];
  char date[16];
  char dummy[16];
  char nsat[16];
  
 
if ( read_for_responses_dual("+QGPSLOC:","ERROR",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO;    


if ( at_copy_serial_to_buffer(time, ',', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;


if ( at_copy_serial_to_buffer(lat, ',', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;

    
if ( at_copy_serial_to_buffer(lon, ',', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;


if ( at_copy_serial_to_buffer(hdop, ',', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;


if ( at_copy_serial_to_buffer(alt, ',', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS)
    return GMX_BG96_KO;


if ( at_copy_serial_to_buffer(dummy, ',', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS)
    return GMX_BG96_KO;


if ( at_copy_serial_to_buffer(cog, ',', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;


if ( at_copy_serial_to_buffer(speed, ',', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;
    

if ( at_copy_serial_to_buffer(dummy, ',', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS)
    return GMX_BG96_KO;


if ( at_copy_serial_to_buffer(date, ',', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;


if ( at_copy_serial_to_buffer(nsat, '\n', 16,  BG96_DEFAULT_TIMEOUT) != AT_COMMAND_SUCCESS )
    return GMX_BG96_KO;


_strip_all_newlines(time);
_strip_all_spaces(time);

_strip_all_newlines(lat);
_strip_all_newlines(lon);
_strip_all_newlines(alt);
_strip_all_newlines(speed);
_strip_all_newlines(date);

sprintf(dummy,"%c%c", time[0],time[1]);
*hour = atoi(dummy);
sprintf(dummy,"%c%c", time[2],time[3]);
*minute = atoi(dummy);
sprintf(dummy,"%c%c", time[4],time[5]);
*seconds = atoi(dummy);

sprintf(dummy,"%c%c", date[0],date[1]);
*day = atoi(dummy);
sprintf(dummy,"%c%c", date[2],date[3]);
*month = atoi(dummy);
sprintf(dummy,"%c%c", date[4],date[5]);
*year = atoi(dummy);


 Serial.print("Time=>");
 Serial.println(time);
 
 Serial.print("Date=>");
 Serial.println(date);

 Serial.print("Latitude=>");
 Serial.println(lat);

 Serial.print("Longitude=>");
 Serial.println(lon);

 Serial.print("Altitude=>");
 Serial.println(alt);

 Serial.print("COG=>");
 Serial.println(cog);

 Serial.print("Speed=>");
 Serial.println(speed);

 Serial.print("Num Sats=>");
 Serial.println(nsat);

    

  return GMX_BG96_OK;
}




uint8_t gmxBG96_TXData(char *data, int data_len)
{
  char tx_string[128];
  
  sprintf(tx_string,"AT+QISEND=0,%d",data_len);
  
   at_send_command(tx_string);   
   

   if ( at_read_for_response_single(">",BG96_DEFAULT_TIMEOUT) != 0 )
        return  GMX_BG96_KO;   
        
  
  Serial.println("Sending Data to Socket");


  for (int i=0; i<data_len; i++) 
  {
    at_send_char(data[i]);
    delay(1);
  }

  if ( read_for_responses_dual("SEND OK","SEND FAIL",BG96_DEFAULT_TIMEOUT) != 0 )
    return  GMX_BG96_KO; 

  return GMX_BG96_OK;
}
