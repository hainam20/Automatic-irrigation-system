#include "dht11.h"
uint16_t x;
void delay_us (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim1,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim1) < us);  // wait for the counter to reach the us input in the parameter
}
//void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
//{
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	GPIO_InitStruct.Pin = GPIO_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
//}

//void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
//{
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	GPIO_InitStruct.Pin = GPIO_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
//}

//void DHT11_start(void){
//	Set_Pin_Output(dht11_GPIO_Port, dht11_Pin);
//	HAL_GPIO_WritePin(dht11_GPIO_Port, dht11_Pin, GPIO_PIN_RESET);
//	delay_us(18000);
//	HAL_GPIO_WritePin(dht11_GPIO_Port, dht11_Pin, GPIO_PIN_SET);
//	delay_us(20);
//	Set_Pin_Input(dht11_GPIO_Port, dht11_Pin);
//}
//uint8_t DHT11_Check_Response (void){
//	uint8_t respone = 0;
//	delay_us(40);
//	if(!HAL_GPIO_ReadPin(dht11_GPIO_Port, dht11_Pin)){
//		delay_us(80);
//		if(HAL_GPIO_ReadPin(dht11_GPIO_Port, dht11_Pin)){
//				respone = 1;
//		}
//		else{
//				respone = 0;
//		}
//	}
//	while ((HAL_GPIO_ReadPin (dht11_GPIO_Port, dht11_Pin)));
//	return respone;
//}
//uint8_t DHT11_Read (void){
//	uint8_t value;
//	for (uint8_t i = 0; i < 8; i++){
//		while (!(HAL_GPIO_ReadPin(dht11_GPIO_Port, dht11_Pin)));
//		delay_us(40);
//		if(!(HAL_GPIO_ReadPin(dht11_GPIO_Port, dht11_Pin))) 		
//		{
//				value&= ~(1<<(7-i));
//		}
//		else{
//				value|= (1<<(7-i));
//		}
//		while ((HAL_GPIO_ReadPin(dht11_GPIO_Port, dht11_Pin)));
//	}	
//	return value;
//}	
//void DHT11_Read_Data (uint8_t *temp, uint8_t *hum)
//{
//	uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2, CheckSum;
//	DHT11_start();
//	if (DHT11_Check_Response() == 1)
//	{
//		Rh_byte1 = DHT11_Read();
//		Rh_byte2 = DHT11_Read();
//		Temp_byte1 = DHT11_Read();
//		Temp_byte2 = DHT11_Read();
//		CheckSum = DHT11_Read();
//		if ((Rh_byte1 + Rh_byte2 + Temp_byte1 + Temp_byte2) == CheckSum)
//		{
//			*hum = Rh_byte1;
//			*temp = Temp_byte1;
//			return;
//		}
//	}
//}

