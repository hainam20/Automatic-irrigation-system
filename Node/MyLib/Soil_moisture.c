#include "Soil_moisture.h"
void Read_soil_moisture_value(uint8_t *Buffer_ADC, uint16_t *ADC_VAL){
		HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 1000);
    *ADC_VAL = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
//		10101010 10101010 10101010 10101010
		Buffer_ADC[0] = (*ADC_VAL / 1000) + '0';
		Buffer_ADC[1] = ((*ADC_VAL / 100) % 10) + '0';
		Buffer_ADC[2] = ((*ADC_VAL / 10) % 10) + '0';
		Buffer_ADC[3] = (*ADC_VAL % 10) + '0';
    HAL_Delay(100);
}
