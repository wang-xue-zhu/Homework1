/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "mpu6050.h"
#include <math.h>
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

/* USER CODE BEGIN PV */
MPU6050_Data d;
float bias_gx, bias_gy, bias_gz;
float bias_ax, bias_ay, bias_az;
float roll = 0.0, pitch = 0.0;
uint32_t last_tick;
float alpha = 0.98f;
float roll_acc = 0.0;
float pitch_acc = 0.0;
float roll_gyro = 0.0;
float pitch_gyro = 0.0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  int cnt = 0;
  setvbuf(stdout, NULL, _IONBF, 0);
  printf("init = %d\r\n", MPU6050_Init(&hi2c2));
  for (int i=0; i < 500; i++) {
    MPU6050_Data bias ={0};
    MPU6050_Read(&hi2c2,&bias);
    bias_gx += bias.gx;
    bias_gy += bias.gy;
    bias_gz += bias.gz;
    bias_ax += bias.ax;
    bias_ay += bias.ay;
    bias_az += bias.az;

    HAL_Delay(4);
  }
  bias_gx /= 500.0f;
  bias_gy /= 500.0f;
  bias_gz /= 500.0f;

  bias_ax /= 500.0f;
  bias_ay /= 500.0f;
  bias_az /= 500.0f;
  bias_az -= 1.0f;

  MPU6050_Read(&hi2c2, &d);
  d.ax -= bias_ax;
  d.ay -= bias_ay;
  d.az -= bias_az;
  roll  = atan2f(d.ay, d.az) * 57.29578f;
  pitch = atan2f(-d.ax, sqrtf(d.ay * d.ay +d.az * d.az)) * 57.29578f;
  roll_gyro = roll;
  pitch_gyro = pitch;
  last_tick = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint32_t now = HAL_GetTick();
    float dt = (now - last_tick) / 1000.0f;
    last_tick = now;
    MPU6050_Read(&hi2c2, &d);
    d.gx -= bias_gx;
    d.gy -= bias_gy;
    d.gz -= bias_gz;
    d.ax -= bias_ax;
    d.ay -= bias_ay;
    d.az -= bias_az;
    roll_acc  = atan2f(d.ay, d.az) * 57.29578f;
    pitch_acc = atan2f(-d.ax, sqrtf(d.ay * d.ay +d.az * d.az)) * 57.29578f;
    roll_gyro += d.gx * dt;
    pitch_gyro += d.gy * dt;
    roll  = alpha * (roll  + d.gx * dt) + (1 - alpha) * roll_acc;
    pitch = alpha * (pitch + d.gy * dt) + (1 - alpha) * pitch_acc;

    if (++cnt >= 10) {
      cnt = 0;
      printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\r\n",
       roll_acc, pitch_acc, roll_gyro, pitch_gyro, roll, pitch);
    }
    HAL_Delay(5);
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

/* USER CODE BEGIN 4 */
int __io_putchar(int ch) {
  HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 10);
  return ch;
}
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
#ifdef USE_FULL_ASSERT
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
