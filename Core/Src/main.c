#include "main.h"
#include "cmsis_os.h"
#include "mfrc522.h"
#include <stdio.h>

osThreadId_t mfrc522TaskHandle;
const osThreadAttr_t mfrc522Task_attributes = { .name = "mfrc522Task", .stack_size =
        128 * 4, .priority = (osPriority_t) osPriorityNormal, };

osThreadId_t servoTaskHandle;
const osThreadAttr_t servoTask_attributes = { .name = "servoTask", .stack_size = 128
        * 4, .priority = (osPriority_t) osPriorityNormal, };

UART_HandleTypeDef UART_InitStruct;
GPIO_InitTypeDef GPIO_InitStruct;
SPI_HandleTypeDef SPI_InitStruct;
TIM_HandleTypeDef TIM_InitStruct = { 0 };

uint8_t AllowedCardID[4] = { 0x4D, 0xAF, 0x84, 0x59 };
uint8_t allowed = 0; // For communication with the servo motor

void SystemClock_Config(void);
void StartMFRC522Task(void *argument);
void StartServoTask(void *argument);
void LCD_Init(void);
void UART_Init(void);
void SPI_Init(void);
void Servo_Init(void);

/* Redirect printf and similar functions to UART */
int _write(int fd, char *ptr, int len) {
	HAL_UART_Transmit(&UART_InitStruct, (uint8_t*) ptr, len, HAL_MAX_DELAY);
	return len;
}

int main(void) {
	/* Enable I-Cache */
	SCB_EnableICache();

	/* Enable D-Cache */
	SCB_EnableDCache();

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	BSP_LED_Init(LED_GREEN);
	BSP_LED_Init(LED_RED);
	BSP_LED_On(LED_RED);

	UART_Init();
	printf("-------------------------------\r\n");

	LCD_Init();
	SPI_Init();
	MFRC522_Init();
	Servo_Init();

	/* Init scheduler */
	osKernelInitialize();

	mfrc522TaskHandle = osThreadNew(StartMFRC522Task, NULL, &mfrc522Task_attributes);
	servoTaskHandle = osThreadNew(StartServoTask, NULL, &servoTask_attributes);

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	for (;;)
		;
}

void StartMFRC522Task(void *argument) {
	/* Recognized card ID */
	uint8_t CardID[4];
	uint8_t type;
	char *result;
	int status;
	uint16_t line = 0;
	uint8_t lcd_msg_1[50];
	uint8_t lcd_msg_2[50];

	printf("Started MFRC522 task\r\n");

	for (;;) {
		status = MFRC522_CheckCard(CardID, &type);
		if (status == RFID_OK) {
			MFRC522_PrettyPrint((unsigned char*) CardID, sizeof(CardID), &result);
			printf("Found tag: %s\r\n", result);

			// Clear the display and start at line 1 again
			if (line >= 20) {
				BSP_LCD_Clear(LCD_COLOR_WHITE);
				line = 0;
			}

			snprintf(lcd_msg_1, sizeof(lcd_msg_1), "Found tag: %s", result);
			BSP_LCD_DisplayStringAtLine(line, lcd_msg_1);
			line++;

			MFRC522_PrettyPrint((unsigned char*) &type, 1, &result);
			printf("Type is: %s\r\n", result);

			// Check if card is allowed
			if (MFRC522_CompareIDs(CardID, AllowedCardID) == RFID_OK) {
				printf("Known ID, access is allowed\r\n");

				snprintf(lcd_msg_2, sizeof(lcd_msg_2), "Access is allowed");
				BSP_LCD_DisplayStringAtLine(line, lcd_msg_2);
				BSP_LED_On(LED_GREEN);
				BSP_LED_Off(LED_RED);
				allowed = 1;
				line++;
			} else {
				printf("Unknown ID, access is denied\r\n");

				snprintf(lcd_msg_2, sizeof(lcd_msg_2), "Access is denied");
				BSP_LCD_DisplayStringAtLine(line, lcd_msg_2);
				line++;
			}
		} else {
			if (status == RFID_ERR) {
				printf("Error\r\n");
			}
		}

		osDelay(1);
	}
}

void StartServoTask(void *argument) {
	for (;;) {
		if (allowed) {
			__HAL_TIM_SET_COMPARE(&TIM_InitStruct, TIM_CHANNEL_1, 1);
			osDelay(3000);
			__HAL_TIM_SET_COMPARE(&TIM_InitStruct, TIM_CHANNEL_1, 15);
			osDelay(200);
			__HAL_TIM_SET_COMPARE(&TIM_InitStruct, TIM_CHANNEL_1, 0);
			allowed = 0;

			// Clear LEDs
			BSP_LED_Off(LED_GREEN);
			BSP_LED_On(LED_RED);
		}

		osDelay(20);
	}
}

void LCD_Init(void) {
	uint32_t ts_status = TS_OK;

	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_Clear(LCD_COLOR_WHITE);

	ts_status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
	while (ts_status != TS_OK)
		;

	ts_status = BSP_TS_ITConfig();
	while (ts_status != TS_OK)
		;

	BSP_LCD_SetFont(&Font24);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	printf("Finished LCD initialization\r\n");
}

void UART_Init(void) {
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	UART_InitStruct.Instance = USART1;
	UART_InitStruct.Init.Mode = UART_MODE_TX_RX;
	UART_InitStruct.Init.BaudRate = 115200;
	UART_InitStruct.Init.WordLength = UART_WORDLENGTH_8B;
	UART_InitStruct.Init.Parity = UART_PARITY_NONE;
	UART_InitStruct.Init.StopBits = UART_STOPBITS_1;
	UART_InitStruct.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	if (HAL_UART_Init(&UART_InitStruct) != HAL_OK) {
		Error_Handler(); // Lights up LEDs to indicate a UART failure
	}

	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SPI_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct1;

	/* Enable GPIO TX/RX clock */
	SPIx_SCK_GPIO_CLK_ENABLE();
	SPIx_MISO_GPIO_CLK_ENABLE();
	SPIx_MOSI_GPIO_CLK_ENABLE();
	SPIx_CS_GPIO_CLK_ENABLE();

	/* SPI SCK GPIO pin configuration  */
	GPIO_InitStruct.Pin = SPIx_SCK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = SPIx_SCK_AF;
	HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MISO GPIO pin configuration; MISO line should be floating */
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Pin = SPIx_MISO_PIN;
	GPIO_InitStruct.Alternate = SPIx_MISO_AF;
	HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MOSI GPIO pin configuration  */
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
	GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
	HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);

	/* SPI CS GPIO pin configuration  */
	GPIO_InitStruct1.Pin = SPIx_CS_PIN;
	GPIO_InitStruct1.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct1.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct1.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(SPIx_CS_GPIO_PORT, &GPIO_InitStruct1);

	SPIx_FORCE_RESET();
	SPIx_RELEASE_RESET();
	SPIx_CLK_ENABLE();

	SPI_InitStruct.Instance = SPIx;
	SPI_InitStruct.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	SPI_InitStruct.Init.Direction = SPI_DIRECTION_2LINES;
	SPI_InitStruct.Init.CLKPhase = SPI_PHASE_1EDGE;
	SPI_InitStruct.Init.CLKPolarity = SPI_POLARITY_LOW;
	SPI_InitStruct.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_InitStruct.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SPI_InitStruct.Init.TIMode = SPI_TIMODE_DISABLE;
	SPI_InitStruct.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI_InitStruct.Init.NSS = SPI_NSS_SOFT;
	SPI_InitStruct.Init.Mode = SPI_MODE_MASTER;

	HAL_Delay(5);

	if (HAL_SPI_Init(&SPI_InitStruct) != HAL_OK) {
		printf("Error initializing SPI\r\n");
	} else {
		printf("Finished SPI initialization\r\n");
	}
}

void Servo_Init(void) {
	__HAL_RCC_TIM11_CLK_ENABLE();

	TIM_InitStruct.Instance = TIM11;
	TIM_InitStruct.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_InitStruct.Init.Period = 100 - 1;
	TIM_InitStruct.Init.Prescaler = 19200 - 1;
	HAL_TIM_PWM_Init(&TIM_InitStruct);

	TIM_OC_InitTypeDef TIM_OC_InitStruct = { 0 };
	TIM_OC_InitStruct.OCMode = TIM_OCMODE_PWM1;
	TIM_OC_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OC_InitStruct.Pulse = 7.5;
	HAL_TIM_PWM_ConfigChannel(&TIM_InitStruct, &TIM_OC_InitStruct, TIM_CHANNEL_1);

	__HAL_RCC_GPIOF_CLK_ENABLE();

	GPIO_InitTypeDef Servo_InitStruct = { 0 };
	Servo_InitStruct.Pin = GPIO_PIN_7;
	Servo_InitStruct.Mode = GPIO_MODE_AF_PP;
	Servo_InitStruct.Pull = GPIO_NOPULL;
	Servo_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	Servo_InitStruct.Alternate = GPIO_AF3_TIM11;
	HAL_GPIO_Init(GPIOF, &Servo_InitStruct);

	HAL_TIM_PWM_Start(&TIM_InitStruct, TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&TIM_InitStruct, TIM_CHANNEL_1, 0);
}

/* If LEDs flash it means UART has failed to initialize */
void Error_Handler(void) {
	BSP_LED_Init(LED1);
	BSP_LED_Off(LED1);

	for (;;) {
		BSP_LED_Toggle(LED1);
		HAL_Delay(1000);
	}
}

void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* The voltage scaling allows optimizing the power consumption when the device is
	 clocked below the maximum system frequency, to update the voltage scaling value
	 regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 432;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 9;
	RCC_OscInitStruct.PLL.PLLR = 7;

	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}

	/* Activate the OverDrive to reach the 216 MHz Frequency */
	ret = HAL_PWREx_EnableOverDrive();
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
	        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
	if (ret != HAL_OK) {
		while (1) {
			;
		}
	}
}
