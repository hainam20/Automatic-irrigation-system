/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2024 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "LoRa.h"
#include "dht11.h"
#include "Soil_moisture.h"
#include <string.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#ifndef __LoRa_H
    #error Include LoRa lib please
#endif

#ifndef __dht11_H
    #error Include dht11 lib please
#endif

#ifndef _Soil_moisture_H
    #error Include soil lib please
#endif


#define DEVICE_0			0
#define DEVICE_1			1
#define DEVICE_2			2

#define receive

#define DEVICE_ID			DEVICE_2


#define DHT11_PORT GPIOA
#define DHT11_PIN GPIO_PIN_0

uint8_t RH1, RH2, TC1, TC2, SUM, CHECK;
uint8_t RHI, RHD, TCI, TCD, SUM;
uint32_t pMillis, cMillis;
int tCelsius = 0;
float tFahrenheit = 0;
int RH = 0;

LoRa myLoRa;
uint8_t LoRa_stat;
uint8_t Buffer_ADC[8];
uint8_t temp = 25, hum = 20;
uint16_t ADC_VAL = 0;
uint16_t Percent_Val;
uint8_t received_data[10];
uint8_t flag_transmit = 0;
uint8_t flag_transmit_relay = 0;
uint8_t ID = DEVICE_ID;
uint8_t relay_state = 0;
uint8_t flag_200 = 0;
uint8_t	flag_200_rv = 0;
uint16_t count_time = 0;
char json[128];
int len;

//int			RSSI;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == DIO0_Pin){
		//received_data = "";
		LoRa_receive(&myLoRa, received_data, 10);
		#if (DEVICE_ID == DEVICE_0)
			if(!strcmp((char*) received_data,"GET_ID0")){
				flag_transmit=1;
			}
			else if (!strcmp((char*) received_data,"200") && flag_transmit){
				flag_200 = 1;
			}
			else if(!strcmp((char*) received_data,"ON_ID0")){
				flag_transmit_relay=1;
				relay_state = 1;
				HAL_GPIO_WritePin(relay_GPIO_Port,relay_Pin,GPIO_PIN_RESET);
			}
			else if(!strcmp((char*) received_data,"OFF_ID0")){
				flag_transmit_relay=1;
				relay_state = 0;
				HAL_GPIO_WritePin(relay_GPIO_Port,relay_Pin,GPIO_PIN_SET);
			}
			memset(received_data, 0, sizeof(received_data));
			
			#elif (DEVICE_ID == DEVICE_1)
			if(!strcmp((char*) received_data,"GET_ID1")){
				flag_transmit=1;
			}
			else if (!strcmp((char*) received_data,"200") && flag_transmit){
				flag_200 = 1;
			}
			else if(!strcmp((char*) received_data,"ON_ID1")){
				flag_transmit_relay=1;
				relay_state = 1;
				HAL_GPIO_WritePin(relay_GPIO_Port,relay_Pin,GPIO_PIN_RESET);
				
			}
			else if(!strcmp((char*) received_data,"OFF_ID1")){
				flag_transmit_relay=1;
				relay_state = 0;
				HAL_GPIO_WritePin(relay_GPIO_Port,relay_Pin,GPIO_PIN_SET);
			}
			memset(received_data, 0, sizeof(received_data));
			
		#elif (DEVICE_ID == DEVICE_2)
			if(!strcmp((char*) received_data,"GET_ID2")){
				flag_transmit = 1;
			}
			else if (!strcmp((char*) received_data,"200") && flag_transmit){
				flag_200 = 1;
			}
			else if(!strcmp((char*) received_data,"ON_ID2")){
				flag_transmit_relay=1;
				relay_state = 1;
				HAL_GPIO_WritePin(relay_GPIO_Port,relay_Pin,GPIO_PIN_RESET);
			}
			else if(!strcmp((char*) received_data,"OFF_ID2")){
				flag_transmit_relay=1;
				relay_state = 0;
				HAL_GPIO_WritePin(relay_GPIO_Port,relay_Pin,GPIO_PIN_SET);
			}
			memset(received_data, 0, sizeof(received_data));
		#endif
	
	}
}

uint16_t map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t DHT11_Start (void)
{
  uint8_t Response = 0;
  GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
  GPIO_InitStructPrivate.Pin = DHT11_PIN;
  GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructPrivate); // set the pin as output
  HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);   // pull the pin low
  HAL_Delay(20);   // wait for 20ms
  HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);   // pull the pin high
  delay_us (30);   // wait for 30us
  GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructPrivate); // set the pin as input
 delay_us (40);
  if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))
  {
    delay_us (80);
    if ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) Response = 1;
  }
  pMillis = HAL_GetTick();
  cMillis = HAL_GetTick();
  while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)) && pMillis + 2 > cMillis)
  {
    cMillis = HAL_GetTick();
  }
  return Response;
}

uint8_t DHT11_Read (void)
{
  uint8_t a,b;
  for (a=0;a<8;a++)
  {
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)) && pMillis + 2 > cMillis)
    {  // wait for the pin to go high
      cMillis = HAL_GetTick();
    }
    delay_us (40);   // wait for 40 us
    if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))   // if the pin is low
      b&= ~(1<<(7-a));
    else
      b|= (1<<(7-a));
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)) && pMillis + 2 > cMillis)
    {  // wait for the pin to go low
      cMillis = HAL_GetTick();
    }
  }
  return b;
}
void value_handle(){
		if(DHT11_Start())
    {
      RHI = DHT11_Read(); // Relative humidity integral
      RHD = DHT11_Read(); // Relative humidity decimal
      TCI = DHT11_Read(); // Celsius integral
      TCD = DHT11_Read(); // Celsius decimal
      SUM = DHT11_Read(); // Check sum
      if (RHI + RHD + TCI + TCD == SUM)
      {
        // Can use RHI and TCI for any purposes if whole number only needed
        tCelsius = (float)TCI + (float)(TCD/10.0);
        tFahrenheit = tCelsius * 9/5 + 32;
        RH = (float)RHI + (float)(RHD/10.0);
        // Can use tCelsius, tFahrenheit and RH for any purposes
      }
    }
		HAL_Delay(200);
		
		Read_soil_moisture_value(Buffer_ADC, &ADC_VAL);
		HAL_Delay(200);

		Percent_Val = map(ADC_VAL,0,4096,0,100);
		Percent_Val = 100 - Percent_Val;
}

//===========================================================

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();

  /* USER CODE BEGIN 2 */
	//	myLoRa = newLoRa();
	
	myLoRa.hSPIx                 = &hspi1;
	myLoRa.CS_port               = NSS_GPIO_Port;
	myLoRa.CS_pin                = NSS_Pin;
	myLoRa.reset_port            = RST_GPIO_Port;
	myLoRa.reset_pin             = RST_Pin;
	myLoRa.DIO0_port						 = DIO0_GPIO_Port;
	myLoRa.DIO0_pin							 = DIO0_Pin;

	myLoRa.frequency             = 433;							  // default = 433 MHz
	myLoRa.spredingFactor        = SF_7;							// default = SF_7
	myLoRa.bandWidth			       = BW_125KHz;				  // default = BW_125KHz
	myLoRa.crcRate				       = CR_4_5;						// default = CR_4_5
	myLoRa.power					       = POWER_20db;				// default = 20db
	myLoRa.overCurrentProtection = 120; 							// default = 100 mA
	myLoRa.preamble				       = 10;		  					// default = 8;
	
	LoRa_reset(&myLoRa);
	if(LoRa_init(&myLoRa) == LORA_OK){
			LoRa_stat = 1;
	}
	LoRa_startReceiving(&myLoRa);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_TIM_Base_Start(&htim1);

  /* USER CODE END 2 */

	HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, GPIO_PIN_SET);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		#ifdef receive
		HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_RESET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_SuspendTick();
//		
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
//		
		HAL_ResumeTick();
		
	if(flag_transmit==1){
			value_handle();
			len = sprintf(json, "{\"ID\":%d,\"temp\":%d,\"hum\":%d,\"adc_val\":%d}", ID, tCelsius, RH, Percent_Val);
			json[len] = '\0';
			for(uint8_t i = 0; i < 5; i++){
				LoRa_transmit(&myLoRa,(uint8_t *) json,len+1,100);
				while(count_time <= 2000){
						HAL_Delay(1);
						count_time++;
						if(flag_200){
							flag_200_rv=1;
							break;
						}
					} 
				count_time = 0;
				if (flag_200_rv){
					flag_200_rv = 0;
					HAL_Delay(100);
					break;
				}
			}
			flag_transmit = 0;
		}
	
	if(flag_transmit_relay){
				HAL_Delay(200);
				if(relay_state){
					len = sprintf(json, "{\"ID\":%d,\"relay\":%d}", ID, 1);
					json[len] = '\0';
					LoRa_transmit(&myLoRa,(uint8_t *) json,len+1,100);
				}
				while(relay_state){
						value_handle();
						len = sprintf(json, "{\"ID\":%d,\"temp\":%d,\"hum\":%d,\"adc_val\":%d}", ID, tCelsius, RH, Percent_Val);
						json[len] = '\0';
						LoRa_transmit(&myLoRa,(uint8_t *) json,len+1,100);
						HAL_Delay(5000);
						if(!relay_state){break;}
				}
				len = sprintf(json, "{\"ID\":%d,\"relay\":%d}", ID, 0);
				json[len] = '\0';
				LoRa_transmit(&myLoRa,(uint8_t *) json,len+1,100);
				HAL_Delay(200);
				flag_transmit_relay = 0;
		}

		#else
		value_handle();
		
		HAL_Delay(2000);

		#endif
 }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  HAL_ADC_Init(&hadc1);

    /**Configure Regular Channel
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

}

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  HAL_SPI_Init(&hspi1);

}

/* TIM1 init function */
void MX_TIM1_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htim1);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);

}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : led_Pin */
  GPIO_InitStruct.Pin = led_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(led_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 RST_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : NSS_Pin relay_Pin */
  GPIO_InitStruct.Pin = NSS_Pin|relay_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DIO0_Pin */
  GPIO_InitStruct.Pin = DIO0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DIO0_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
