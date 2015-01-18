#ifndef _AIR_SENSOR_H_
#define _AIR_SENSOR_H_

/*
	BMP180 specific defines
*/
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


void AIRSENS_init( );
void AIRSENS_start( );
void AIRSENS_stop( );
char AIRSENS_read( char reg );
void AIRSENS_write( char reg, char data );
unsigned int AIRSENS_getPres();
/*
	void IIC_RegWrite(byte address, byte reg,byte val);
	byte IIC_RegRead(byte address, byte reg);
	void IIC_Start(void);
	byte IIC_StopRead(void);

*/
#endif // _AIR_SENSOR_H_
