#ifndef _GMX_BG96_H_
#define _GMX_BG96_H_

#include <Arduino.h>

#define GMX_BG96_OK                      0 
#define GMX_BG96_KO                      1


#define BG96_DEFAULT_TIMEOUT          1000
#define BG96_BOOT_TIMEOUT             8000
#define BG96_ATTACH_TIMEOUT           200000


#define BG96_BAND_B1                0x1
#define BG96_BAND_B2                0x2
#define BG96_BAND_B3                0x4
#define BG96_BAND_B4                0x8
#define BG96_BAND_B5                0x10
#define BG96_BAND_B8                0x80
#define BG96_BAND_B12               0x800
#define BG96_BAND_B13               0x1000
#define BG96_BAND_B18               0x20000
#define BG96_BAND_B19               0x40000
#define BG96_BAND_B20               0x80000
#define BG96_BAND_B26               0x2000000
#define BG96_BAND_B28               0x8000000
#define BG96_BAND_B39               0x4000000000



#define GMX_BG96_PWRKEY_PIN			GMX_GPIO6
#define GMX_BG96_RESET_PIN			GMX_RESET
#define GMX_BG96_LED1_PIN			GMX_GPIO1
#define GMX_BG96_LED2_PIN			GMX_GPIO2
#define GMX_BG96_STATUS       GMX_GPIO3



uint8_t gmxBG96_init(String _ip, String _port);
uint8_t gmxBG96_getIMEI(char *data, uint8_t max_size);
uint8_t gmxBG96_getIMSI(char *data, uint8_t max_size);

uint8_t gmxBG96_setGSM(char *apn);
uint8_t gmxBG96_setNBIoT(char *apn, char *band);
uint8_t gmxBG96_setCatM1(char *apn);

uint8_t gmxBG96_attach();

uint8_t gmxBG96_isNetworkAttached(int *attach_status);
uint8_t gmxBG96_isNBNetworkAttached(char *op, char *techno);
bool gmxBG96_isNetworkAttachedStatus(void);
uint8_t gmxBG96_setOperator( char *op_code);

uint8_t gmxBG96_TXData(char *data, int data_len);
uint8_t gmxBG96_RXData(  char *rx_buffer, int rx_buffer_len, int *rx_received_len );
uint8_t gmxBG96_SocketClose();
uint8_t gmxBG96_SocketOpen();

uint8_t gmxBG96_GpsOn(void);
uint8_t gmxBG96_GpsOff(void);
uint8_t gmxBG96_GpsLoc(char *lat, char *lon, char *alt, char *cog, char *speed, int* hour, int* minute, int* second, int* day, int* month, int* year ); 


uint8_t gmxBG96_signalQuality(int *rssi, int *ber );


#endif 
