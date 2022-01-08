/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
static __IO uint32_t uwTimingDelay;
static __IO uint32_t uwTimingDelay2;
static __IO uint32_t uwTimingDelay3;
static __IO uint32_t uwTimingDelay4;

void TimingDelay_Decrement(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

#if 0
/* Includes ------------------------------------------------------------------*/
// #include "stm32f3_discovery.h"
// #include "stm32f3_discovery_accelerometer.h"
// #include "stm32f3_discovery_gyroscope.h"
#include "selftest.h"
// #include "usbd_desc.h"
// #include "usbd_hid.h" 
#include <stdio.h>
#include "stm32f3xx_hal.h"
#include "stm32f30x_usart.h"
#endif

#define	VSYNC_RISING	0x01
#define	HSYNC_RISING	0x02
#define	HSYNC_FALLING	0x04
#define	PCLK_RISING		0x08
#define	IMAGE_CAPUTRE_MODE	0x10
#define	ONE_FRAME_COMPLETE	0x20
#define	USER_BUTTON			0x40

#define	OFST		3

#if COLOR
#define	H_SIZE			1280		// 640
#define	V_SIZE			120
#else
#define	H_SIZE			640
#define	V_SIZE			120			//120
#endif

#define	CAL_LINE		80			// lji 20160428 calibration mode add
#define	CAL_WHITE_BAR	CAL_LINE			//48
#define	UPDATE_SIZE		11

#define	V_END			120			//192			// 128
#define	CAPTURE_START	0			// 72			// 8
#define LEFT_HALF	0
#define	RIGHT_HALF	0xff

#define	JPEG_PKT	6			//30			//20

#define	TRX_SIZE	P_SIZE+5		//1029
#define	A_STX		0
#define	A_CMD		1
#define	A_P_NO		2
#define	A_F_CS		P_SIZE+3		// 1027	
#define	A_P_CS		P_SIZE+4	// 1028		
#define	P_SIZE		5120		// 1024
#define	TRX_HDR		3

#define NORMAL_STATE 0
#define STX_STATE	 1	
#define NACK_STATE	 2
#define P_NO_STATE	 3
#define DATA_STATE	 4
#define CRC_STATE	 5
#define	FWDL_STATE	 6

#define	CHK_MODE	0x01

#define NACK_MODE	0xff

#define IMG_UPLOAD		'A'
#define FWDL_START_CMD	'D'
#define R_CMD			'C'

#define	STX	0x02
#define ACK	0x06
#define NAK	0x15
#define	END_ACK	0x86
#define	DUMMY_BYTES	0		//0x10

#define	DISCONNECT	0x1F

#define	X_ON	0x11
#define	X_OFF	0x13

#define COMn	2

#define	LED_4	0
#define	LED_6	3

#define PREC_CNT	5

#define	BTN_OFF	0
#define	BTN_ON	1
#define VBATDIV	1

#define	TXEIE	0x80	// USART2 Control 1 reg. Tx interrupt enable bit

#define	STOP_PERIOD		6	// 2				// sec
#define	STNBY_TIME	120		//30		// 300		// sec
#define	BT_PULSE_CNT	5

#define	LOW_ILLUMIN		0
#define	MIDDLE_ILLUMIN	1
#define	HIGH_ILLUMIN	2

#define	FLASH_ERROR		1
#define	FLASH_OK		0

/**
 * @brief BUTTON Types Definition
 */
typedef enum 
{
  BUTTON_USER = 0

}Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1

}ButtonMode_TypeDef;

/**
 * @brief push-button
 */
#define USER_BUTTON_PIN                  GPIO_Pin_0
#define USER_BUTTON_GPIO_PORT            GPIOA
#define USER_BUTTON_GPIO_CLK_ENABLE()    __GPIOA_CLK_ENABLE()
#define USER_BUTTON_GPIO_CLK_DISABLE()   __GPIOA_CLK_DISABLE()
#define USER_BUTTON_EXTI_IRQn            EXTI0_IRQn 

#define BUTTONx_GPIO_CLK_ENABLE(__BUTTON__)  (((__BUTTON__) == BUTTON_USER) ? USER_BUTTON_GPIO_CLK_ENABLE() : 0 )

#define BUTTONx_GPIO_CLK_DISABLE(__BUTTON__) (((__BUTTON__) == BUTTON_USER) ? USER_BUTTON_GPIO_CLK_DISABLE() : 0 )
#define BUTTONn                          1  

#define ADCx					 ADC1
#define ADCx_CLK				 RCC_APB2Periph_ADC1
#define DMA_CHANNELx			 DMA_Channel_0
#define DMA_STREAMx 			 DMA2_Stream0
#define ADCx_DR_ADDRESS 		 ((uint32_t)0x4001204C)

/* User can use this section to tailor TIMx instance used and associated
   resources */
/* Definition for TIMx clock resources */
#if defined (STM32F303xC)
#define TIMx                           TIM3
#define TIMx_CLK_ENABLE                __TIM3_CLK_ENABLE

/* Definition for USARTx Pins */
#define TIMx_CHANNEL_GPIO_PORT()       __GPIOC_CLK_ENABLE()
#define TIMx_GPIO_PORT_CHANNEL1        GPIOC
#define TIMx_GPIO_PIN_CHANNEL1              GPIO_PIN_6
#define TIMx_GPIO_AF_CHANNEL1          GPIO_AF2_TIM3
#else
	
#define TIMx                           TIM16
#define TIMx_CLK_ENABLE()              __TIM16_CLK_ENABLE()
	
	/* Definition for TIMx Channel Pins */
#define TIMx_CHANNEL_GPIO_PORT()       __GPIOB_CLK_ENABLE()
#define TIMx_GPIO_PORT_CHANNEL1        GPIOB
#define TIMx_GPIO_PIN_CHANNEL1         GPIO_PIN_6
#define TIMx_GPIO_AF_CHANNEL1          GPIO_AF1_TIM16

#endif

/**
  * @brief  I2C EEPROM Interface pins
  */  
	/* Maximum Timeout values for flags and events waiting loops. These timeouts are
	   not based on accurate values, they just guarantee that the application will 
	   not remain stuck if the I2C communication is corrupted.
	   You may modify these timeout values depending on CPU frequency and application
	   conditions (interrupts routines ...). */   
#define sEE_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define sEE_LONG_TIMEOUT         ((uint32_t)(30 * sEE_FLAG_TIMEOUT))
	
#define PO8030D_I2C_ADDRESS        0x00DC				// 0x30F lji 141005 Pixelplus Image sensor address
// #define 2C_SLAVE_ADDRESS7      	0x00DC
#define I2C_SPEED						 100000

#define sEE_I2C                          I2C1
#define sEE_I2C_CLK                      RCC_APB1Periph_I2C1
#define sEE_I2C_SCL_PIN                  GPIO_Pin_8                  /* PB.06 */
#define sEE_I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SCL_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sEE_I2C_SCL_SOURCE               GPIO_PinSource8
#define sEE_I2C_SCL_AF                   GPIO_AF_I2C1
#define sEE_I2C_SDA_PIN                  GPIO_Pin_9                  /* PB.09 */
#define sEE_I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SDA_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sEE_I2C_SDA_SOURCE               GPIO_PinSource9
#define sEE_I2C_SDA_AF                   GPIO_AF_I2C1

#define sEE_I2C_DMA                      DMA1   
#define sEE_I2C_DMA_CHANNEL              DMA_Channel_1
#define sEE_I2C_DMA_STREAM_TX            DMA1_Stream6
#define sEE_I2C_DMA_STREAM_RX            DMA1_Stream0   
#define sEE_I2C_DMA_CLK                  RCC_AHB1Periph_DMA1
#define sEE_I2C_DR_Address               ((uint32_t)0x40005410)
#define sEE_USE_DMA
   
#define sEE_I2C_DMA_TX_IRQn              DMA1_Stream6_IRQn
#define sEE_I2C_DMA_RX_IRQn              DMA1_Stream0_IRQn
#define sEE_I2C_DMA_TX_IRQHandler        DMA1_Stream6_IRQHandler
#define sEE_I2C_DMA_RX_IRQHandler        DMA1_Stream0_IRQHandler   
#define sEE_I2C_DMA_PREPRIO              0
#define sEE_I2C_DMA_SUBPRIO              0   
   
#define sEE_TX_DMA_FLAG_FEIF             DMA_FLAG_FEIF6
#define sEE_TX_DMA_FLAG_DMEIF            DMA_FLAG_DMEIF6
#define sEE_TX_DMA_FLAG_TEIF             DMA_FLAG_TEIF6
#define sEE_TX_DMA_FLAG_HTIF             DMA_FLAG_HTIF6
#define sEE_TX_DMA_FLAG_TCIF             DMA_FLAG_TCIF6
#define sEE_RX_DMA_FLAG_FEIF             DMA_FLAG_FEIF0
#define sEE_RX_DMA_FLAG_DMEIF            DMA_FLAG_DMEIF0
#define sEE_RX_DMA_FLAG_TEIF             DMA_FLAG_TEIF0
#define sEE_RX_DMA_FLAG_HTIF             DMA_FLAG_HTIF0
#define sEE_RX_DMA_FLAG_TCIF             DMA_FLAG_TCIF0
   
#define sEE_DIRECTION_TX                 0
#define sEE_DIRECTION_RX                 1  

#define sEE_OK                    0
#define sEE_PAGESIZE		   16
#define sEE_MAX_TRIALS_NUMBER     300

/* Time constant for the delay caclulation allowing to have a millisecond 
   incrementing counter. This value should be equal to (System Clock / 1000).
   ie. if system clock = 168MHz then sEE_TIME_CONST should be 168. */
#define sEE_TIME_CONST                   84 

/* Size of Transmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      256

#define PCLK_PIN                GPIO_Pin_8                       /* PA8 */
#define HSYNC_PIN                GPIO_Pin_7                       /* PA7 */
#define VSYNC_PIN                GPIO_Pin_5                       /* PA5 */

/* */
#define	SENSOR_PORTA		GPIOA			// A1~A7(bit0~bit6)
#define	SENSOR_PORTB		GPIOB			// B7(bit7)
#define SENSOR_PORTA_CLK_ENABLE()		 __GPIOA_CLK_ENABLE()
#define SENSOR_PORTB_CLK_ENABLE()		 __GPIOB_CLK_ENABLE()

/* Size of buffer */
#define BUFFERSIZE                       (COUNTOF(aTxBuffer) - 1)

/* User can use this section to tailor USARTx/UARTx instance used and associated 
   resources */
/* Definition for USARTx clock resources */
/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define EVAL_COM1                        USART1
#define USART1_CLK                    RCC_APB2Periph_USART1
#define USART1_TX_PIN                 GPIO_Pin_9
#define USART1_TX_GPIO_PORT           GPIOA
#define USART1_TX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define USART1_TX_SOURCE              GPIO_PinSource9
#define USART1_TX_AF                  GPIO_AF_USART1
#define USART1_RX_PIN                 GPIO_Pin_10
#define USART1_RX_GPIO_PORT           GPIOA
#define USART1_RX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define USART1_RX_SOURCE              GPIO_PinSource10
#define USART1_RX_AF                  GPIO_AF_USART1
#define USART1_CLK_INIT               RCC_APB2PeriphClockCmd

#define EVAL_COM2                        USART2
#define USART2_CLK                    RCC_APB1Periph_USART2
#define USART2_TX_AF                  GPIO_AF_USART2
#define USART2_RX_AF                  GPIO_AF_USART2
#define USART2_CLK_INIT               RCC_APB1PeriphClockCmd
#if DISCOVERY
#define USART2_TX_PIN                 GPIO_Pin_5
#define USART2_TX_GPIO_PORT           GPIOD
#define USART2_TX_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define USART2_TX_SOURCE              GPIO_PinSource5
#define USART2_RX_PIN                 GPIO_Pin_6
#define USART2_RX_GPIO_PORT           GPIOD
#define USART2_RX_GPIO_CLK            RCC_AHB1Periph_GPIOD
#define USART2_RX_SOURCE              GPIO_PinSource6
#else
#define USART2_TX_PIN                 GPIO_Pin_2
#define USART2_TX_GPIO_PORT           GPIOA
#define USART2_TX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define USART2_TX_SOURCE              GPIO_PinSource2
#define USART2_RX_PIN                 GPIO_Pin_3
#define USART2_RX_GPIO_PORT           GPIOA
#define USART2_RX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define USART2_RX_SOURCE              GPIO_PinSource3
#endif

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define KEY_PRESSED     0x01
#define KEY_NOT_PRESSED 0x00

typedef enum 
{
  COM1 = 0,
  COM2 = 1,
} COM_TypeDef;

/* Exported macro ------------------------------------------------------------*/
#define ABS(x)         (x < 0) ? (-x) : x

/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/ 
