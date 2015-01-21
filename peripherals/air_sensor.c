#include "air_sensor.h"
#include "../includes/at91sam3x8.h"
void AIRSENS_init(){
	/*
		-Set CLDIV, CHDIV, CKDIV in TWI_CWGR

		we will only be reading the sensor. will only do following commands once
		-Set control register: - master enable
			TWI_CR= MSEN + SVDIS



	*/
	*AT91C_PMC_PCER = (1<<22)|(1<<11);
    *AT91C_PIOA_PDR = (3<<17);
	*AT91C_TWI0_CWGR = 0x60606;//ckdiv = 7, chdiv = cldiv = 3
	*AT91C_TWI0_CR = AT91C_TWI_SVDIS | AT91C_TWI_MSEN;
	*AT91C_TWI0_MMR = (0x60<<16) | AT91C_TWI_IADRSZ_1_BYTE ;

        //for(int i = 0; i < 1000 ; i++)
          //asm("nop");
	AIRSENS_write(BMP180_CTRL_REG1, BMP180_CTRL_REG_SBYB |1<<BMP180_CTRL_REG_OS );// set raw data output, oversample ratio = 2. Alt = 0. Sbyb = 1
	AIRSENS_write(BMP180_PT_DATA_CFG, BMP180_PT_DATA_CFG_PDEFE|BMP180_PT_DATA_CFG_DREM );


}

void AIRSENS_start(){
	/*
		Set TWI_CR start |stop

		while ((!Rx ready)))
	*/
	//AT91C_TWI0_CR
}

void AIRSENS_ready(){
	int timeout = 0;
	while(!(*AT91C_TWI0_SR&AT91C_TWI_TXCOMP_MASTER) && timeout < 1000){
		timeout ++;
	}
}

char AIRSENS_read( char reg ){
	/*
		-Set master mode register: dev slave adress, transfer direction bit
		-Set the internal address
			TWI_IADR = address
		Read receive holding register

		read status register
		while ((!TX Complete)))

	*/
    AIRSENS_ready();
	*AT91C_TWI0_MMR |= AT91C_TWI_MREAD;
	*AT91C_TWI0_IADR = reg;
	*AT91C_TWI0_CR =  AT91C_TWI_START|AT91C_TWI_STOP;
        //*AT91C_TWI0_CR = AT91C_TWI_STOP ;

	int timeout = 0;
	while(!(*AT91C_TWI0_SR&AT91C_TWI_RXRDY) && timeout < 10000){
		timeout ++;
	}
        if(timeout >=10000){
            //printf("Timeout read\n");
        }
	char c_Return_Data = *AT91C_TWI0_RHR;

	//Air_Sensor_Ready();
        return c_Return_Data;
}

void AIRSENS_write( char reg, char data ){
	//MREAD = 0
	//Air_Sensor_Start();
    AIRSENS_ready();
	*AT91C_TWI0_MMR &= ~AT91C_TWI_MREAD;
	*AT91C_TWI0_IADR = reg;
	*AT91C_TWI0_THR = data;
	*AT91C_TWI0_CR = AT91C_TWI_STOP;
	int timeout = 0;
	while(!(*AT91C_TWI0_SR&AT91C_TWI_TXRDY_MASTER) && timeout < 10000){
		timeout ++;
	}
        if(timeout >=10000){
            //printf("Timeout write\n");
        }
	//Air_Sensor_Ready();
}


unsigned int AIRSENS_getPres(){
    unsigned char msb = AIRSENS_read(BMP180_P_OUT_MSB);
    unsigned char csb = AIRSENS_read(BMP180_P_OUT_CSB);
    unsigned char lsb = AIRSENS_read(BMP180_P_OUT_LSB);
    return lsb>>6 | csb<<2|msb<<10;
}
