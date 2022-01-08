/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

uint8_t sensor_irq_flag;	// bit 0 : v_sync rising 
  							// bit 1 : h_sync rising
  							// bit 2 : h_sync falling
  							// bit 3 : pclk rising
  							// bit 4 : Image Capture Mode
  							// bit 5 : One Frame Capture Complete
  							// bit 6 : USER BUTTON
  							// bit 7 :
int 	h_pixel_cnt;
int		 hline_tune_cnt;	
uint16_t hline_pcnt;	
uint8_t even_flag = 0x00;		//0xff;

uint16_t tmpCC4[2] = {0, 0};
uint8_t rtc_wkup_flag;

extern uint8_t rsp_flag;
extern uint8_t rsp2_flag;

extern uint16_t rx_cnt1;
extern uint16_t rx_ppnt1;
extern uint16_t rx_gpnt1;

extern uint16_t rx_cnt;
extern uint16_t rx_ppnt;
extern uint16_t rx_gpnt;

extern uint16_t	tx1_ppnt;
extern uint16_t	tx1_gpnt;
extern uint16_t	tx2_ppnt;
extern uint16_t	tx2_gpnt;

uint8_t vsync_flag;
								
extern uint8_t jpeg_buf[V_SIZE][H_SIZE];
extern uint8_t aRxBuffer1[RXBUFFERSIZE];
extern uint8_t aRxBuffer[RXBUFFERSIZE];
extern uint8_t trx_buf[JPEG_PKT][TRX_SIZE];
extern uint8_t xon1_state;
extern uint8_t xon2_state;

extern uint8_t btn_flag;
extern uint8_t bt_stat_cnt;
extern uint8_t LED_toggle_flag;
extern uint8_t jpeg_tx_pnt;
extern uint8_t jpeg_pkt_cnt, tx_pkt_cnt;
extern uint8_t cal_flag;
extern uint32_t uwTimingDelay5;

extern __IO uint32_t   uwCaptureNumber; 
extern __IO uint32_t   uwPeriodValue;

 extern void TimingDelay_Decrement(void);
 
 extern void Set_Delay(__IO uint32_t nTime);
 extern void Set_Delay2(__IO uint32_t nTime);
 /******************************************************************************/
 /*            Cortex-M4 Processor Exceptions Handlers                         */
 /******************************************************************************/
 
 /**
   * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
	  if(!uwTimingDelay2) {
		LED_toggle_flag ^= 0xff;
		if(LED_toggle_flag) GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);	  // LED ON
		else				GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET); 	  // LED OFF
		Set_Delay2(50); 		  // 10ms 단위, LED toggle용 
	  }
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
	  if(!uwTimingDelay2) {
		LED_toggle_flag ^= 0xff;
		if(LED_toggle_flag) GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);	  // LED ON
		else				GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET); 	  // LED OFF
		Set_Delay2(50); 		  // 10ms 단위, LED toggle용 
	  }
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
	  if(!uwTimingDelay2) {
		LED_toggle_flag ^= 0xff;
		if(LED_toggle_flag) GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);	  // LED ON
		else				GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET); 	  // LED OFF
		Set_Delay2(50); 		  // 10ms 단위, LED toggle용 
	  }
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
	  if(!uwTimingDelay2) {
		LED_toggle_flag ^= 0xff;
		if(LED_toggle_flag) GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);	  // LED ON
		else				GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET); 	  // LED OFF
		Set_Delay2(50); 		  // 10ms 단위, LED toggle용 
	  }
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
   TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/
/**
  * @brief  This function handles ADC1 and ADC2 global interrupts requests.
  * @param  None
  * @retval None
  */
  #if 0
void ADC_IRQHandler(void)
{
  
  /* Clear ADC1 EOC pending interrupt bit */
  ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
  
  /* Get converted value of Channel14 converted by ADC1 */
  ADC1ConvertedValue = ADC_GetConversionValue(ADC1);
  
}
#endif

/**
  * @brief  This function handles RTC Wakeup global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_WKUP_IRQHandler(void)
{
  if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
  {
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line22);
	rtc_wkup_flag = 0xff;
  } 
}

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{ 
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);

	if((bt_stat_cnt > 10) && (rsp2_flag&0x80)) { // lji 150710  BT on && 전송모드가 아니면 
//		GPIOC->BSRRH = GPIO_Pin_5;
		GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);	  // Sensor ready
			sensor_irq_flag |= USER_BUTTON; 
		jpeg_pkt_cnt = jpeg_tx_pnt = rx_cnt = tx_pkt_cnt = 0;	// lji 150805 전송중 hang-up 리셋용
		Set_Delay(STOP_PERIOD* 100);		// lji 150811 key 동작 시 주기 재설정, sensor standby 연속발생방지
	  }
  	}
}

/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{ 

	/* PCLK interrupt detected */
	 if(EXTI->PR & PCLK_PIN) {
		EXTI->PR = PCLK_PIN;
		even_flag ^= 0xff;
#if COLOR
		if((sensor_irq_flag &  IMAGE_CAPUTRE_MODE) && (hline_tune_cnt>=0)) { // button을 누르고 V_SYNC이후 Capture Mode 진입, Mono Mode
#else
//		if((sensor_irq_flag &  IMAGE_CAPUTRE_MODE) && (even_flag) && (hline_tune_cnt>=0)) { // button을 누르고 V_SYNC이후 Capture Mode 진입
		if((sensor_irq_flag &  IMAGE_CAPUTRE_MODE) && even_flag) { // button을 누르고 V_SYNC이후 Capture Mode 진입
//		if(even_flag) { // button을 누르고 V_SYNC이후 Capture Mode 진입
#endif
#if DISCOVERY
			uint8_t tmp;

			tmp = GPIOD->IDR&0x08;						// lji 150114 for discovery kit.
			jpeg_buf[hline_tune_cnt][h_pixel_cnt++] = ((GPIOB->IDR&0xf7) | tmp);
#else
			jpeg_buf[hline_tune_cnt][h_pixel_cnt++] = GPIOB->IDR;
#endif
		}
	 }
//	GPIOC->BSRRL = GPIO_Pin_5;
		/* HSYNC falling interrupt detected */
 	if(EXTI->PR & HSYNC_PIN)	{
	  	EXTI->PR = HSYNC_PIN;
		if(sensor_irq_flag &  IMAGE_CAPUTRE_MODE) { // button을 누르고 V_SYNC이후 Capture Mode 진입
//			GPIOC->BSRRH = GPIO_Pin_4;
			if(h_pixel_cnt >= H_SIZE) {
			  h_pixel_cnt = 0;
			  even_flag = 0x00;			// 0xff;
			}
			hline_pcnt++;
			hline_tune_cnt = hline_pcnt - CAPTURE_START;		// lji 150129

			if(hline_pcnt >= (V_END-1)) {
			  sensor_irq_flag |= ONE_FRAME_COMPLETE;
			  hline_pcnt = V_END-1;
			  sensor_irq_flag &= ~IMAGE_CAPUTRE_MODE;
//			  GPIOC->BSRRL = GPIO_Pin_4;
		}
 	}  
 	}  
 /* VSYNC interrupt detected */
	 if(EXTI->PR & VSYNC_PIN)	{
		 //  GPIOC->BSRRH = GPIO_Pin_3;
	  	EXTI->PR = VSYNC_PIN;
//		if(sensor_irq_flag & USER_BUTTON) { 	// button을 누르면 Capture Mode 진입
		if((sensor_irq_flag & 0x60) == USER_BUTTON) { 	// lji 150810 test
			hline_pcnt = h_pixel_cnt = 0;
			hline_tune_cnt = (int)hline_pcnt - CAPTURE_START;
//			sensor_irq_flag &= ~ONE_FRAME_COMPLETE;
			sensor_irq_flag |= IMAGE_CAPUTRE_MODE;
			sensor_irq_flag &= ~USER_BUTTON;
//			GPIOC->BSRRL = GPIO_Pin_5;
		}
		//	GPIOC->BSRRL = GPIO_Pin_3;
 	}
}

#if 0
// lji 150610 BT_STAT interrupt,  슬립모드 진입시간 게산시, 접속과정(펄스)의 시간은 제외토록 수정
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)
	{
	  
	  /* Clear the EXTI line 11 pending bit */
	  EXTI_ClearITPendingBit(EXTI_Line11);
	  if(GPIOA->IDR&0x0800) {
//	  	tmp_uwTimingDelay = uwTimingDelay;
//		Set_Delay(2000);			// 20 sec 후 sleep
	  }
	  else {
//		uwTimingDelay = tmp_uwTimingDelay;
	  }
	}
}
#endif

void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  { uint8_t ch;
  
    /* Read one byte from the receive data register */
    ch = (USART1->DR & 0xFF);
    aRxBuffer1[rx_ppnt1++] = ch;
	rx_cnt1++;
	
	if(rx_ppnt1 >= RXBUFFERSIZE) rx_ppnt1 = 0;
  }
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
#if BT_TEST
	if(tx1_gpnt < tx1_ppnt)
	  USART1->DR = trx_buf[1][tx1_gpnt++];
	else		  /* Disable the Tx buffer empty interrupt */
	  USART1->CR1 &= 0xff7f;
#else
	  GPIOB->BSRRH = GPIO_Pin_15;
	  if(tx2_gpnt < tx2_ppnt)
		  USART1->DR = trx_buf[jpeg_tx_pnt][tx2_gpnt++];
	  else {
	  USART1->CR1 &= 0xff7f;
		  jpeg_tx_pnt++;
	}
	  GPIOB->BSRRL = GPIO_Pin_15;
#endif
  }
}

void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  { uint8_t ch;
  
    /* Read one byte from the receive data register */
    ch = (USART2->DR & 0xFF);
	  aRxBuffer[rx_ppnt++] = ch;
	  rx_cnt++;

	  if(rx_ppnt >= RXBUFFERSIZE) rx_ppnt = 0;
  }
  
  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  {
#if BT_TEST
	if(tx2_gpnt < tx2_ppnt)
		USART2->DR = trx_buf[0][tx2_gpnt++];
	else		/* Disable the Tx buffer empty interrupt */
		USART2->CR1 &= 0xff7f;
#else
//	  while(!(USART2->SR & USART_FLAG_TXE)) ;
	if(tx2_gpnt < tx2_ppnt)
		USART2->DR = trx_buf[jpeg_tx_pnt][tx2_gpnt++];

	else {
		/* Disable the Tx buffer empty interrupt */
			USART2->CR1 &= 0xff7f;
			if(jpeg_tx_pnt<(JPEG_PKT-1)) 
				jpeg_tx_pnt++;
	}
#endif
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
