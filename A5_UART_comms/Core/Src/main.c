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
#define BRR_num 35

void SystemClock_Config(void);
void UART_init(void);
void UART_print(char* );
void USART_ESC_Code(char*);

int main(void)
{


  HAL_Init();
  SystemClock_Config();
  UART_init();
  __enable_irq();

  // clear screen and reset cursor
  USART_ESC_Code("[2J");
  USART_ESC_Code("[H");

  // move down 3 rows and right 5 spaces
  USART_ESC_Code("[3B");
  USART_ESC_Code("[5C");
  UART_print("All good students read the");

  // move down 1 row and left 21 spaces, and make text blink
  USART_ESC_Code("[1B");
  USART_ESC_Code("[21D");
  USART_ESC_Code("[5m");
  UART_print("Reference Manual");

  // reset cursor and turn off blinking
  USART_ESC_Code("[H");
  USART_ESC_Code("[0m");
  UART_print("Input: ");

  while (1)
  {

  }

}

void USART2_IRQHandler(void) {

	if (USART2->ISR & USART_ISR_RXNE) {
		// read keyboard input
		uint16_t input = USART2->RDR;
		switch (input) {
			case 'r':
				// if r typed then text will be red
				USART_ESC_Code("[31m");
				break;
			case 'b':
				// if b typed then text will be blue
				USART_ESC_Code("[34m");
				break;
			case 'g':
				// if g typed then text will be green
				USART_ESC_Code("[32m");
				break;
			case 'w':
				// if w typed then text will be white
				USART_ESC_Code("[37m");
				break;
			default:
				// wait for TX buffer to be empty then write to terminal
				while(!(USART2->ISR & USART_ISR_TXE));
				USART2->TDR = input;
				break;

		}
		// clear flag
		USART2->RQR |= USART_RQR_RXFRQ;
	}

}

void USART_ESC_Code(char* data) {
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = '\x1B';  // esc char

    while (*data) {
           while (!(USART2->ISR & USART_ISR_TXE));
           USART2->TDR = *data++;  // Send character and get the next one too
       }
}

void UART_print(char *data) {
	// wait for TXE then write character and advanve till null
    while (*data) {
        while (!(USART2->ISR & USART_ISR_TXE));
        USART2->TDR = *data++;
    }
}


// initializing USART2
void UART_init(void) {
	// enable GPIOA and USART clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	// AF7 configuration for PA2 and PA3
	GPIOA->AFR[0] &= ~((0xF << GPIO_AFRL_AFSEL2_Pos)|(0xF << GPIO_AFRL_AFSEL3_Pos)); // clear
	GPIOA->AFR[0] |= ((0x7 << GPIO_AFRL_AFSEL2_Pos)|(0x7 << GPIO_AFRL_AFSEL3_Pos)); // set
	//turn PA2 and PA3 on
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	GPIOA->MODER |= (2 << GPIO_MODER_MODE2_Pos) | (2 << GPIO_MODER_MODE3_Pos); // Set to AF mode (0b10)
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);

	// define word length, 8 bits
	USART2->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1);
	// set baud rate
	USART2->BRR = BRR_num;
	// 1 stop bit
	USART2->CR2 &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0);
	// enable transmit and receive
	USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE);
	// enable RX interrupts
	USART2->CR1 |= (USART_CR1_RXNEIE);
	//enable USART and interrupt
	USART2->CR1 |= USART_CR1_UE;
	NVIC->ISER[1] |= (1 << (USART2_IRQn & 0x1F));
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
