/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE BEGIN Includes */

//#include "sh1106.h"
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
CAN_HandleTypeDef hcan;

TIM_HandleTypeDef htim4;

osThreadId cfgTask_OS_1msHandle;
osThreadId cfgTask_OS_5msHandle;
osThreadId cfgTask_OS_10msHandle;
osThreadId cfgTask_OS_100mHandle;
/* USER CODE BEGIN PV */
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/**/
CAN_TxHeaderTypeDef   TxHeader;

uint8_t               TxData[8];

uint32_t              TxMailbox;


CAN_RxHeaderTypeDef rxHeader; //CAN Bus Transmit Header
CAN_TxHeaderTypeDef txHeader; //CAN Bus Receive Header
uint8_t canRX[8] = {0,0,0,0,0,0,0,0};  //CAN Bus Receive Buffer
CAN_FilterTypeDef canfil; //CAN Bus Filter
uint32_t canMailbox; //CAN Bus Mail box variable


extern uint8_t Rxcount;
extern uint32_t dataSize;
extern uint8_t check;
extern uint32_t time;
char Hello[]="Hello world \n";

//
uint8_t LED_BlinkMode = 10;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_CAN_Init(void);
void Task_OS_1ms(void const * argument);
void Task_OS_5ms(void const * argument);
void Task_OS_10ms(void const * argument);
void Task_OS_100ms(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE BEGIN 0 */

// Override the weak call back function
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//      if (htim->Instance == TIM4) {
//              HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//              CDC_Transmit_FS((uint8_t *)Hello, strlen(Hello));
//      }
//}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	int x, y;
	x=-1;
	y=0;
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
  MX_TIM4_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */
  // Fire up the timer
//  HAL_TIM_Base_Start_IT(&htim4);
  // Start the timer in PWM mode - output will be on PB6
   HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

   // CAN
   canfil.FilterBank = 0;
    canfil.FilterMode = CAN_FILTERMODE_IDMASK;
    canfil.FilterFIFOAssignment = CAN_RX_FIFO0;
    canfil.FilterIdHigh = 0;
    canfil.FilterIdLow = 0;
    canfil.FilterMaskIdHigh = 0;
    canfil.FilterMaskIdLow = 0;
    canfil.FilterScale = CAN_FILTERSCALE_32BIT;
    canfil.FilterActivation = ENABLE;
    canfil.SlaveStartFilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan,&canfil); //Initialize CAN Filter
   HAL_CAN_Start(&hcan); //Initialize CAN Bus
   HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);// Initialize CAN Bus Rx Interrupt

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of cfgTask_OS_1ms */
  osThreadDef(cfgTask_OS_1ms, Task_OS_1ms, osPriorityHigh, 0, 128);
  cfgTask_OS_1msHandle = osThreadCreate(osThread(cfgTask_OS_1ms), NULL);

  /* definition and creation of cfgTask_OS_5ms */
  osThreadDef(cfgTask_OS_5ms, Task_OS_5ms, osPriorityAboveNormal, 0, 128);
  cfgTask_OS_5msHandle = osThreadCreate(osThread(cfgTask_OS_5ms), NULL);

  /* definition and creation of cfgTask_OS_10ms */
  osThreadDef(cfgTask_OS_10ms, Task_OS_10ms, osPriorityNormal, 0, 128);
  cfgTask_OS_10msHandle = osThreadCreate(osThread(cfgTask_OS_10ms), NULL);

  /* definition and creation of cfgTask_OS_100m */
  osThreadDef(cfgTask_OS_100m, Task_OS_100ms, osPriorityNormal, 0, 128);
  cfgTask_OS_100mHandle = osThreadCreate(osThread(cfgTask_OS_100m), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */



	  		x=x+8;
//	  		if(x>SSD1306_WIDTH-1)
//	  		{
//	  			x=-1;
//	  			y++;
//	  			SSD1306_ToggleInvert();
//	  			SSD1306_UpdateScreen();
//	  		}

//	  		HAL_Delay(1000);
//
//		for ( int x = 1; x <= 10000 ; x++ )
//			{
//				itoa(x, snum, 10);
//				SSD1306_GotoXY (0, 30);
//				SSD1306_Puts ("             ", &Font_16x26, 1);
//				SSD1306_UpdateScreen();
//				if(x < 10) {
//					SSD1306_GotoXY (53, 30);  // 1 DIGIT
//				}
//				else if (x < 100 ) {
//					SSD1306_GotoXY (45, 30);  // 2 DIGITS
//				}
//				else if (x < 1000 ) {
//					SSD1306_GotoXY (37, 30);  // 3 DIGITS
//				}
//				else {
//					SSD1306_GotoXY (30, 30);  // 4 DIGIS
//				}
//				SSD1306_Puts (snum, &Font_16x26, 1);
//				SSD1306_UpdateScreen();
//				HAL_Delay (500);
//			    }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = T4_PRE;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = T4_CNT;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = PWM_1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void App_Change_Dutycycle_PWM_100ms_Proc(void)
{
	static int cnt=0;
	if(cnt==49)
	{
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 2499);
	}
	else if(cnt==99)
	{
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 999);
	}
	else if(cnt==149)
	{
		cnt=0;
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 9999);
	}
	cnt++;
}

void App_ScreenUpdate_10ms_Proc(void)
{



}


void App_LED_100ms_Proc(void)
{
	static uint8_t cnt_scr=0;

	if(cnt_scr==LED_BlinkMode)
	{
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		cnt_scr=0;
	}
	cnt_scr++;
}

void App_CAN_Tx_100ms_Proc(void)
{
	static uint8_t cntc_scr=0;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.StdId = 0x7E8;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 8;

	 txHeader.DLC = 8; // Number of bites to be transmitted max- 8
	  txHeader.IDE = CAN_ID_STD;
	  txHeader.RTR = CAN_RTR_DATA;
	  txHeader.StdId = 0x030;
	  txHeader.ExtId = 0x02;
	  txHeader.TransmitGlobalTime = DISABLE;


	if(cntc_scr==10)
	{
		TxData[0] = 0x01; //gui 1 byte di
		TxData[1] = 0x01; //gui 1 byte di
		TxData[2] = 0x01; //gui 1 byte di
		TxData[3] = 0x01; //gui 1 byte di
		TxData[4] = 0x01; //gui 1 byte di
		TxData[5] = 0x01; //gui 1 byte di
		TxData[6] = 0x01; //gui 1 byte di
		TxData[7] = 0x01; //gui 1 byte di

		if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
		{
		   //Error_Handler ();
		   LED_BlinkMode = 2;
		}
		else
		{
			LED_BlinkMode = 20;
		}
	}
	cntc_scr++;
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan1)
{
	HAL_CAN_GetRxMessage(hcan1, CAN_RX_FIFO0, &rxHeader, canRX); //Receive CAN bus message to canRX buffer
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);// toggle PA3 LED
	LED_BlinkMode = 10;
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_Task_OS_1ms */
/**
  * @brief  Function implementing the cfgTask_OS_1ms thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Task_OS_1ms */
void Task_OS_1ms(void const * argument)
{
  /* USER CODE BEGIN 5 */
	TickType_t lastWakeTime;
	const TickType_t frequency=1;
	lastWakeTime=xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  osDelayUntil(&lastWakeTime,frequency);

  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Task_OS_5ms */
/**
* @brief Function implementing the cfgTask_OS_5ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_OS_5ms */
void Task_OS_5ms(void const * argument)
{
  /* USER CODE BEGIN Task_OS_5ms */
	TickType_t lastWakeTime;
	const TickType_t frequency=5;
	lastWakeTime=xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  osDelayUntil(&lastWakeTime,frequency);

  }
  /* USER CODE END Task_OS_5ms */
}

/* USER CODE BEGIN Header_Task_OS_10ms */
/**
* @brief Function implementing the cfgTask_OS_10ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_OS_10ms */
void Task_OS_10ms(void const * argument)
{
  /* USER CODE BEGIN Task_OS_10ms */
	TickType_t lastWakeTime;
	const TickType_t frequency=10;
	lastWakeTime=xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	osDelayUntil(&lastWakeTime,frequency);
	App_ScreenUpdate_10ms_Proc();
  }
  /* USER CODE END Task_OS_10ms */
}

/* USER CODE BEGIN Header_Task_OS_100ms */
/**
* @brief Function implementing the cfgTask_OS_100m thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_OS_100ms */
void Task_OS_100ms(void const * argument)
{
  /* USER CODE BEGIN Task_OS_100ms */
	TickType_t lastWakeTime;
	const TickType_t frequency=100;
	lastWakeTime=xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  osDelayUntil(&lastWakeTime,frequency);
	  App_Change_Dutycycle_PWM_100ms_Proc();
	  App_LED_100ms_Proc();
	  App_CAN_Tx_100ms_Proc();
  }
  /* USER CODE END Task_OS_100ms */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
