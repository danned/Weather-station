/*air_sensor.h - Staffan Piledahl, Daniel Dahlberg
Driver for air pressure/altitude sensor BMP180 (sparkfun breakout) I2C device.
Application specific drivers for reading air pressure, see example.
Full support read/write to device possible. See device manual for other functionality.

Note: Library includes I2C drivers tailored for this device.  

Blocks used: PIOA, TWI0

------- Usage example: -----
AIRSENS_init();
int reading = AIRSENS_getPres();
---------------------------
TODO:
- 
*/

#ifndef _AIR_SENSOR_H_
#define _AIR_SENSOR_H_

/************************************************************************/
/* BMP180 defines										                */
/************************************************************************/
#define BMP180_CTRL_REG1 0x26
#define BMP180_CTRL_REG1_RAW 1<<6
#define BMP180_CTRL_REG_OS 3
#define BMP180_CTRL_REG_SBYB 1
#define BMP180_PT_DATA_CFG 0x13
#define BMP180_PT_DATA_CFG_TDEFE 1<<0
#define BMP180_PT_DATA_CFG_PDEFE 1<<1
#define BMP180_PT_DATA_CFG_DREM 1<<2
#define BMP180_P_OUT_MSB 0x1
#define BMP180_P_OUT_CSB 0x2
#define BMP180_P_OUT_LSB 0x3

/************************************************************************/
/* Basic drivers (Air pressure readings only)			                */
/************************************************************************/
void AIRSENS_init( );
unsigned int AIRSENS_getPres();

/************************************************************************/
/* More Generic I2C drivers (Full device use)				            */
/************************************************************************/
void AIRSENS_start( );
void AIRSENS_stop( );
char AIRSENS_read( char reg );
void AIRSENS_write( char reg, char data );

#endif // _AIR_SENSOR_H_
