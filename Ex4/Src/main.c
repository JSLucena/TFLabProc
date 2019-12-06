/* USER CODE BEGIN Header */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define pino_rs_0   GPIOA->BSRR = 1<<(16+9)  
#define pino_rs_1	GPIOA->BSRR = 1<<9
#define pino_e_0	GPIOC->BSRR = 1<<(16+7)
#define pino_e_1	GPIOC->BSRR = 1<<7

#define cursor_on 	 0x0c
#define cursor_off   0x0e
#define cursor_blink 0x0f

#define liga_led GPIOA->BSRR = (1 << 5);
#define desliga_led GPIOA->BRR = (1 << 5);


#define VERMELHO 0.5
#define VERDE 3.0
#define YELLOW 2.6
#define AZUL 3.3
#define BRANCO 0

void uDelay(void)
{
  int x=10;
	
  while(x) x--;
}

void delayUs(int tempo)
{
  while(tempo--) uDelay();
}
void lcd_send4bits(unsigned char dat)
{
	if((dat & (1 << 0)) == 0)
		GPIOB->BRR = (1 << 5);
	else
		GPIOB->BSRR = (1 << 5);
	
	if((dat & (1 << 1)) == 0)
		GPIOB->BRR = (1 << 4);
	else
		GPIOB->BSRR = (1 << 4);

	if((dat & (1 << 2)) == 0)
		GPIOB->BRR = (1 << 10);
	else
		GPIOB->BSRR = (1 << 10);
	
	if((dat & (1 << 3)) == 0)
		GPIOA->BRR = (1 << 8);
	else
		GPIOA->BSRR = (1 << 8);	
}


void lcd_wrcom4(unsigned char com)
{
	lcd_send4bits(com);
	pino_rs_0;
	pino_e_1;
	delayUs(5);
	pino_e_0;
	HAL_Delay(5);
}

void lcd_wrcom(unsigned char com)
{
	lcd_send4bits(com/0x10);
	pino_rs_0;
	pino_e_1;
	delayUs(5);
	pino_e_0;
	
	lcd_send4bits(com%0x10);
	pino_rs_0;
	pino_e_1;
	delayUs(5);
	pino_e_0;
	
	HAL_Delay(5);
}


void lcd_wrchar(unsigned char ch)
{
	lcd_send4bits(ch/0x10);
	pino_rs_1;
	pino_e_1;
	delayUs(5);
	pino_e_0;
	
	lcd_send4bits(ch % 0x10);
	pino_rs_1;
	pino_e_1;
	delayUs(5);
	pino_e_0;
	
	HAL_Delay(5);
}
void lcd_init(unsigned char cursor)
{
	lcd_wrcom4(3);
  lcd_wrcom4(3);
  lcd_wrcom4(3);
  lcd_wrcom4(2);

  lcd_wrcom(0x28);
  lcd_wrcom(cursor);
  lcd_wrcom(0x06);
  lcd_wrcom(0x01);

}

void lcd_goto(unsigned char x, unsigned char y)
{
  if(x<16)
  {
    if(y==0) lcd_wrcom(0x80+x);
    if(y==1) lcd_wrcom(0xc0+x);
    if(y==2) lcd_wrcom(0x90+x);
    if(y==3) lcd_wrcom(0xd0+x);
  }
}

void lcd_wrstr(char * str)
{
	while((*str))
	{
		lcd_wrchar(*str++);
	}
}


int le_AD(void)
{
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc,50);
	int X = HAL_ADC_GetValue(&hadc);
  HAL_ADC_Stop(&hadc);
	return X;
}
void print_AD(int v)
{
	char str[30];
	sprintf(str,"%04d",v);
	lcd_wrstr(str);
}


char aonde = 'L';
int fputc(int ch, FILE *f)
{
	if(aonde == 'L') lcd_wrchar(ch);
	else if(aonde == 'S') HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 10);
	return ch;
}
void print_color(char x)
{
	
	
	lcd_goto(0,0);
	
	printf("%c%c%c%c",x,x,x,x);
}
	

void move90neg()
{
		GPIOC->BSRR = 1 << 0;
		HAL_Delay(1);
		GPIOC->BRR = 1 << 0;
		HAL_Delay(19);
}
void move0()
{		int i = 0;
	for(i = 0;i<5;i++)
	{
		GPIOC->BSRR = 1 << 0;
		HAL_Delay(1);
		delayUs(900);
		GPIOC->BRR = 1 << 0;
		HAL_Delay(18);
		delayUs(100);
	}
}
void move90pos()
{
	
	int i = 0;
	for(i = 0;i<10;i++)
	{
		GPIOC->BSRR = 1 << 0;
		HAL_Delay(1);
		//delayUs(950);
		GPIOC->BRR = 1 << 0;
		HAL_Delay(11);
	}
}
void calibra()
{
	GPIOA -> BSRR = 1 << 4;
	lcd_goto(0,0);
	printf("teste RRR");
	HAL_Delay(3000);
	GPIOA -> BRR = 1 << 4;
	
	GPIOB -> BSRR = 1 << 0;
	lcd_goto(0,0);
	printf("teste BBB");
	HAL_Delay(3000);
	GPIOB -> BRR = 1 << 0;
	
	GPIOC -> BSRR = 1 << 1;
	lcd_goto(0,0);
	printf("teste GGG");
	HAL_Delay(3000);
	GPIOC -> BRR = 1 << 1;

	
}
char detectaCor()
{
	float vR,vB,vG;
	int x;

	GPIOA -> BSRR = 1 << 4;
	lcd_goto(12,0);
	HAL_Delay(250);
	x = le_AD();
	print_AD(x);
	
	HAL_Delay(250);
	GPIOA -> BRR = 1 << 4;	
	vR = (3.3*(float)x)/4095;
	lcd_goto(9,1);
	printf("vR %2.3f",vR);
	
	GPIOB -> BSRR = 1 << 0;
	lcd_goto(12,0);
	HAL_Delay(250);
	x = le_AD();
	print_AD(x);
	HAL_Delay(250);
	GPIOB -> BRR = 1 << 0;
	vB = (3.3*(float)x)/4095;
	lcd_goto(9,1);
	printf("vB %2.3f",vB);
	
	GPIOC -> BSRR = 1 << 1;
	lcd_goto(12,0);
	HAL_Delay(250);
	x = le_AD();
	print_AD(x);
	HAL_Delay(250);
	GPIOC -> BRR = 1 << 1;
	vG = (3.3*(float)x)/4095;
	lcd_goto(9,1);
	printf("vG %2.3f",vG);
	if(vB > 2.6 && vR > 2.6 && vG > 2.6)
		return 'N';
		
	if(vB < 1.8 && vR < 2.0 && vG < 2.6)
		return 'B';
	else if(vR < 1.4 && vB < 2.5 && vG < 2.5)
		return 'R';
	else if(vG > 2.0 && vB > 2.0 && vR > 1.4)
		return 'G';
	else return '?';
	
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int x;
	char cor;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */
	lcd_init(cursor_blink);
	GPIOB->BSRR = (1 << 6);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
	/*	
		lcd_goto(12,0);
		int x = le_AD();
		print_AD(x);	
		float v = (3.3*(float)x)/4095;		
		lcd_goto(0,1);
		aonde = 'L';
		printf("%2.3f",v);
		print_color(v);
		
		
	HAL_Delay(500);
		
*/
		//calibra();
		cor = detectaCor();
		print_color(cor);
		if(cor == 'B')
			move0();
		else
			move90pos();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure for the selected ADC regular channel to be converted. 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, separador_Pin|green_Pin|GPIO_PIN_7|GPIO_PIN_10 
                          |GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, red_Pin|LD2_Pin|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, blue_Pin|GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : separador_Pin green_Pin PC7 */
  GPIO_InitStruct.Pin = separador_Pin|green_Pin|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : red_Pin LD2_Pin PA8 PA9 */
  GPIO_InitStruct.Pin = red_Pin|LD2_Pin|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : blue_Pin PB10 PB4 PB5 
                           PB6 */
  GPIO_InitStruct.Pin = blue_Pin|GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
