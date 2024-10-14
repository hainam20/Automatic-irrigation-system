#ifndef _Soil_moisture_H
#define _Soil_moisture_H
#include "stm32f1xx_hal.h"
extern ADC_HandleTypeDef hadc1;
void Read_soil_moisture_value(uint8_t *Buffer_ADC, uint16_t *ADC_VAL);
#endif

