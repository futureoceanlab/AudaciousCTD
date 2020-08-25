#include "pressure.h"
#include "UART.h"
#include "I2C.h"
#include <string.h>

#define MS5837_ADDR               0x76
#define MS5837_RESET              0x1E
#define MS5837_ADC_READ           0x00
#define MS5837_PROM_READ          0xA0
#define MS5837_CONVERT_D1_8192    0x4A
#define MS5837_CONVERT_D2_8192    0x5A


void Pressure_Compute(uint32_t D1, uint32_t D2, uint16_t C[], uint32_t *P, int32_t *T);
uint8_t crc4(uint16_t n_prom[]);


void Pressure_Measure(uint32_t *P, int32_t *T) {
	// Reset the Sensor
	uint8_t outbuffer = MS5837_RESET;
	HAL_I2C_Master_Transmit(&hi2c2, MS5837_ADDR<<1, &outbuffer, 1, 1000);
	// reset completion waiting
	HAL_Delay(10);

	// Read Calibration values and CRC
	uint16_t C[8];
	unsigned char buffer[3];
	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0;
	for (uint8_t i = 0; i < 7; i++) {
	  uint8_t outbuffer2 = MS5837_PROM_READ + i*2;
	  HAL_I2C_Master_Transmit(&hi2c2, MS5837_ADDR<<1, &outbuffer2, 1, 1000);

	  HAL_I2C_Master_Receive(&hi2c2, MS5837_ADDR<<1, buffer, 2, 1000);
	  C[i] = buffer[0]<<8 | buffer[1];
	}
	uint8_t crcRead = C[0] >> 12;
	uint8_t crcCalculated = crc4(C);
	if (crcCalculated == crcRead) {
	  char m[20];
	  sprintf(m, "CRC success %u %u\r\n", crcRead, crcCalculated);
	  HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	} else {
	  char m[20];
	  sprintf(m, "CRC failed\r\n");
	  HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	}
	// Read D1
	uint8_t D1_convert = MS5837_CONVERT_D1_8192;
	HAL_I2C_Master_Transmit(&hi2c2, MS5837_ADDR<<1, &D1_convert, 1, 1000);
	HAL_Delay(20); // Max conversion time per datasheet
	// Read ADC
	uint8_t ADC_Read = MS5837_ADC_READ;
	HAL_I2C_Master_Transmit(&hi2c2, MS5837_ADDR<<1, &ADC_Read, 1, 1000);
	buffer[0] = 0; buffer[1] = 0;
	HAL_I2C_Master_Receive(&hi2c2, MS5837_ADDR<<1, buffer, 3, 1000);
	uint32_t D1 = buffer[0] << 16 | buffer[1] << 8 | buffer[2];

	// Read D2
	uint8_t D2_convert = MS5837_CONVERT_D2_8192;
	HAL_I2C_Master_Transmit(&hi2c2, MS5837_ADDR<<1, &D2_convert, 1, 1000);
	HAL_Delay(20); // Max conversion time per datasheet
	// Read ADC
	HAL_I2C_Master_Transmit(&hi2c2, MS5837_ADDR<<1, &ADC_Read, 1, 1000);
	buffer[0] = 0; buffer[1] = 0; buffer[2] = 0;
	HAL_I2C_Master_Receive(&hi2c2, MS5837_ADDR<<1, buffer, 3, 1000);
	uint32_t D2 = buffer[0] << 16 | buffer[1] << 8 | buffer[2];

	// calculate the pressure
	Pressure_Compute(D1, D2, C, P, T);
//	char m[10];
//	int n = sprintf(m, "Pressure: %d\r\n", P);
//	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
//	return ((float)P)/10.0f;
}


void Pressure_Compute(uint32_t D1, uint32_t D2, uint16_t C[], uint32_t *P, int32_t *T) {
	// Given C1-C6 and D1, D2, calculated TEMP and P
	// Do conversion first and then second order temp compensation

	int32_t dT = 0;
	int64_t SENS = 0;
	int64_t OFF = 0;
	int32_t SENSi = 0;
	int32_t OFFi = 0;
	int32_t Ti = 0;
	int64_t OFF2 = 0;
	int64_t SENS2 = 0;

	// Terms called
	dT = D2-((uint32_t)C[5])*256l;
	SENS = ((int64_t)C[1])*32768l+(((int64_t)C[3])*dT)/256l;
	OFF = ((int64_t)C[2])*65536l+(((int64_t)C[4])*dT)/128l;

	// Temp conversion
	int32_t TEMP = 2000l+((int64_t)dT)*C[6]/8388608LL;

	//Second order compensation

	if((TEMP/100)<20){         //Low temp
		Ti = (3*((int64_t)dT)*((int64_t)dT))/(8589934592LL);
		OFFi = (3*(TEMP-2000)*(TEMP-2000))/2;
		SENSi = (5*(TEMP-2000)*(TEMP-2000))/8;
		if((TEMP/100)<-15){    //Very low temp
			OFFi = OFFi+7*(TEMP+1500l)*(TEMP+1500l);
			SENSi = SENSi+4*(TEMP+1500l)*(TEMP+1500l);
		}
	}
	else if((TEMP/100)>=20){    //High temp
		Ti = 2*(dT*dT)/(137438953472LL);
		OFFi = (1*(TEMP-2000)*(TEMP-2000))/16;
		SENSi = 0;
	}

	OFF2 = OFF-OFFi;           //Calculate pressure and temp second order
	SENS2 = SENS-SENSi;

	*T = (TEMP-Ti)*10;
//	char m[20];
//	sprintf(m, "Temp: %d\r\n", (int)TEMP);
//	HAL_UART_Transmit(&huart2, (uint8_t*)m, strlen(m), 1000);
	*P = (((D1*SENS2)/2097152l-OFF2)/8192l);
}



uint8_t crc4(uint16_t n_prom[]) {
	uint16_t n_rem = 0;

	n_prom[0] = ((n_prom[0]) & 0x0FFF);
	n_prom[7] = 0;

	for ( uint8_t i = 0 ; i < 16; i++ ) {
		if ( i%2 == 1 ) {
			n_rem ^= (uint16_t)((n_prom[i>>1]) & 0x00FF);
		} else {
			n_rem ^= (uint16_t)(n_prom[i>>1] >> 8);
		}
		for ( uint8_t n_bit = 8 ; n_bit > 0 ; n_bit-- ) {
			if ( n_rem & 0x8000 ) {
				n_rem = (n_rem << 1) ^ 0x3000;
			} else {
				n_rem = (n_rem << 1);
			}
		}
	}

	n_rem = ((n_rem >> 12) & 0x000F);

	return n_rem ^ 0x00;
}
