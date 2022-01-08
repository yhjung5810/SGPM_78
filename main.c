/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main program body
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
#include "main.h"
// #include "jpeg_encoder.h"

// #define I2C_ADDRESS        0xA0				// AT24c16
#define BYTE unsigned char
#define SBYTE signed char
#define SWORD signed short int
#define WORD unsigned short int
#define DWORD unsigned long int
#define SDWORD signed long int

#define FWDL_FLAG_ADDRESS   (uint32_t)0x0801FFF0		// end of sector 4
// #define FWDL_FLAG_ADDRESS   (uint32_t)0x0801FFB0		// end of sector 4
#define FLASH_Sector_4     ((uint16_t)0x0020) /*!< Sector Number 4 */

//###### you should increase version if F/W is revised ###############

const uint8_t version_no[5] = "V1.0";

//########################################################

/* Private typedef -----------------------------------------------------------*/
// #define  PERIOD_VALUE       (uint32_t)(666 - 1)  			  /* Period Value  */
//#define  PERIOD_VALUE       (uint32_t)(51 - 1)  			  /* Period Value  */
#define  PERIOD_VALUE       (uint32_t)(41 - 1)  			  /* Period Value  */
//#define  PERIOD_VALUE       (uint32_t)(121 - 1)  			  /* Period Value  */
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE/2)        /* 50% Duty Capture Compare 1 Value  */
#define  PULSE2_VALUE       (uint32_t)(PERIOD_VALUE*37.5/100) /* Capture Compare 2 Value  */
#define  PULSE3_VALUE       (uint32_t)(PERIOD_VALUE/4)        /* Capture Compare 3 Value  */
#define  PULSE4_VALUE       (uint32_t)(PERIOD_VALUE*12.5/100) /* Capture Compare 4 Value  */

RCC_ClocksTypeDef RCC_Clocks;

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

/* Counter Prescaler value */
uint32_t PrescalerValue = 0;

/* Buffer used for transmission */
uint8_t aTxBuffer[20]=" UART1 TEST!!"; 

/* Buffer used for reception */
uint8_t aRxBuffer1[RXBUFFERSIZE];
uint8_t aRxBuffer[RXBUFFERSIZE];

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT}; 
const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN}; 
const uint8_t BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn};

const uint32_t fwdl_flag = 0x4657444C;		//'FWDL'

uint8_t read_LR_flag;
uint8_t eend_flag;
uint8_t tgl_cnt;
uint8_t bt_stat_cnt;

__IO ITStatus UartReady = RESET;

uint16_t s_id[2] = {0x0080, 0x0130};

#define IMG_SENSOR_SIZE	54+58+143+4
const uint16_t PO8030D[IMG_SENSOR_SIZE] = {
// dummy command& output Hi-z release ==> 2
		0x0300, 0x5B00
// BANK A ==> 27+9+2+8+4+4
		, 0x0300
#if OLD_M					// Old mechanism(Reverse), start line 1
		, 0x0600, 0x0789, 0x0c02, 0x0d80, 0x0e00, 0x0f78, 0x1000, 0x110A, 0x1200, 0x1382, 0x6a00	// start line 1
		, 0x3B00, 0x3C78  // aw_window							
		, 0x3D00, 0x3ED6, 0x3F01, 0x40AB, 0x4100, 0x4232, 0x4300, 0x4496
		, 0x1D06, 0x2111, 0x5E57, 0x60F0, 0x621D, 0x6350, 0x6500, 0x6CCF, 0x6E81, 0x9C01
		, 0x8A00, 0x8B00, 0x8C00, 0x8D80 // tgvstop
		, 0x9D5E, 0x9E01, 0x9F5E, 0xA001, 0xA16B, 0xA401, 0xA55E, 0xDF24, 0xE15C, 0xE25C
		, 0xE35C, 0xE45C, 0xE55C, 0xE65C, 0xE7FF, 0xE87F 
#else					// New mechanism, start line 144 => 160
//		, 0x0601, 0x0713, 0x0c02, 0x0d80, 0x0e00, 0x0f78, 0x1000, 0x1199, 0x1201, 0x1311, 0x6a00		// start line 144
		, 0x0601, 0x0723, 0x0c02, 0x0d80, 0x0e00, 0x0f78, 0x1000, 0x11A1, 0x1201, 0x1319, 0x6a00	// start line 152
//		, 0x0601, 0x0723, 0x0c02, 0x0d80, 0x0e00, 0x0f78, 0x1000, 0x11A9, 0x1201, 0x1321, 0x6a00	// start line 160 
		, 0x3B00, 0x3C78  // aw_window										
		, 0x3D00, 0x3ED6, 0x3F01, 0x40AB, 0x4100, 0x4232, 0x4300, 0x4496		
		, 0x1D06, 0x2111, 0x5E57, 0x60F0, 0x621D, 0x6350, 0x6500, 0x6C0F, 0x6E81, 0x9C01
//		, 0x8A00, 0x8B8F, 0x8C01, 0x8D0F 															// start line 144
		, 0x8A00, 0x8B97, 0x8C01, 0x8D17															// start line 152
//		, 0x8A00, 0x8B9F, 0x8C01, 0x8D1F															// start line 160
		, 0x9D5E, 0x9E01, 0x9F5E, 0xA001, 0xA16B, 0xA401, 0xA55E, 0xDF24, 0xE15C, 0xE25C
		, 0xE35C, 0xE45C, 0xE55C, 0xE65C, 0xE7FF, 0xE87F 
#endif
// #################### BANK B #################### ==> 54+1+3
		, 0x0301
		, 0x04F7, 0x0660, 0x4028, 0x4122, 0x4320, 0x4E00, 0x5114, 0x5C40, 0x5D30, 0x5E28, 0x624F	// lji output format ==> CbYCrY
		, 0x63AA, 0x6485, 0x658F, 0x663A, 0x678B, 0x6887, 0x69A8, 0x6A4F, 0x6D00, 0x6E01
		, 0x6F03, 0x7010, 0x711C, 0x7230, 0x7342, 0x7460, 0x7578, 0x769A, 0x77B6, 0x78CE
		, 0x79E0, 0x7AF0, 0x7BFF, 0x9900, 0x9D37, 0x9E10, 0x9FEB, 0xAA80, 0xAB80, 0xAC60, 0xB80C, 0xCB04, 0xCE10
		, 0xCF1F, 0xD01F, 0xD420, 0xD710, 0xD810, 0xD910, 0xE608, 0xF000, 0xF180, 0xF2DD
		, 0xF300, 0xF49A, 0xF5A3 

// #################### BANK C #################### ==> 141+2
		, 0x0302
		, 0x057D, 0x0687, 0x1601, 0x17FE, 0x1803, 0x19FE, 0x1A03, 0x1BFE, 0x1C00, 0x1D01	
		, 0x1E80, 0x1F00, 0x201F, 0x21F2, 0x2200, 0x231F, 0x24F2, 0x2570, 0x2600, 0x2700
		, 0x2880, 0x2BE8, 0x2CE8, 0x2D08, 0x3030, 0x3110, 0x3310, 0x3440, 0x3580, 0x36FF
		, 0x3A70, 0x3B70, 0x3C70, 0x3D70, 0x3E20, 0x3F30, 0x4002, 0x4108, 0x4212, 0x4325	
		, 0x4408, 0x4680, 0x480F, 0x490F, 0x4A06, 0x5629, 0x5743, 0x5833, 0x592B, 0x5A09, 0x5B13, 0x5CA1
		, 0x5D2C, 0x5E2B, 0x5F60, 0x6021, 0x6113, 0x6206, 0x630C, 0x64FA, 0x6508, 0x66FF
		, 0x6808, 0x6938, 0x6C7E, 0x6D7C, 0x6E80, 0x6F7A, 0x707E, 0x717D, 0x7233, 0x734F
		, 0x745D, 0x7808, 0x7902, 0x7B40, 0x7C2E, 0x7D58, 0x7E60, 0x7F40, 0x8040, 0x816B
		, 0x826B, 0x8300, 0x8408, 0x8500, 0x8640, 0x8820, 0x8900, 0x8A00, 0x8B20, 0x8C1C
		, 0x8D00, 0x8E00, 0x8F1C, 0x9024, 0x9100, 0x9200, 0x9310, 0x9430, 0x9520, 0x9628
		, 0x9720, 0x9830, 0x9920, 0x9A33, 0x9B4F, 0x9C5D, 0xA300, 0xA605, 0xA715, 0xA808
		, 0xAA18, 0xAD24, 0xAE2A, 0xAF3F, 0xB110, 0xB418, 0xB51F, 0xB63F, 0xCC10, 0xCD08
		, 0xD000, 0xD100, 0xD200, 0xD460, 0xD70C, 0xD808, 0xD900, 0xE100, 0xE200, 0xE504
		, 0xE604, 0xE708, 0xE900, 0xEC08, 0xED18, 0xEE1C, 0xF708, 0xFA14, 0xFB18, 0xFC20

// #################### BANK D #################### ==> 4+2+3
		, 0x0303
		, 0x5740, 0x5838, 0x5940 					// org
		
};

#define SENSOR_SIZE_2	14
const uint16_t illumin_cntrl[SENSOR_SIZE_2] = {
// #################### BANK C #################### ==> 14
		  0x0302
		, 0x049B								// , 0x049D
		, 0x1601, 0x170E						// exp Frm
		, 0x1801, 0x190E						// midframeheight
		, 0x1A01, 0x1B0E						// maxfrmheight
		, 0x1F00, 0x201A, 0x21C0				// midexp
		, 0x2200, 0x231A, 0x24C0				// max exp
};

const uint16_t illumin[3][6] = {
		{0x0300, 0x1700, 0x1880, 0x19DD, 0x1A34, 0x1B40},				// low
		{0x0300, 0x1700, 0x1880, 0x19DD, 0x1A08, 0x1B40},				// middle
		{0x0300, 0x1700, 0x1820, 0x1983, 0x1A00, 0x1B40}				// high
};

const uint16_t set_frame_height[3] = {
#if OLD_M					// Old mechanism(Reverse), start line 1
	0x0300, 0x0600, 0x0789
#else
	0x0300, 0x0601, 0x0723
#endif
};

/** @addtogroup STM32F3-Discovery_Demo STM32F3-Discovery_Demo
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t UserButtonPressed = 0x00;
__IO uint32_t PressCount = 0;

uint16_t rx_cnt1;
uint16_t rx_ppnt1;
uint16_t rx_gpnt1;
uint8_t rx1_data_state;
uint8_t t1_state;
uint8_t p_no, get_cnt;
uint16_t data_cnt;
uint8_t xon1_state = 0xff;
uint8_t rsp_flag;
uint8_t rsp2_flag = 0x80;
uint8_t end_pkt_flag;			// lji 20151228 added for dummy bytes tx

uint16_t rx_cnt;
uint16_t rx_ppnt;
uint16_t rx_gpnt;
uint8_t rx2_data_state;
uint8_t xon2_state = 0xff;
uint8_t jpeg_pkt_cnt, jpeg_tx_pnt;
uint8_t retry_cnt, tx_pkt_cnt;
uint8_t t2_no;

uint8_t t2_state;
uint8_t t2_cnt;

uint16_t	tx1_ppnt;
uint16_t	tx1_gpnt;
uint16_t	tx2_ppnt;
uint16_t	tx2_gpnt;

uint8_t LED_toggle_flag;
uint8_t btn_flag;

uint8_t tst_cnt;

__IO uint16_t uhADCConvertedValue;
__IO uint16_t ADC1ConvertedValue;
__IO uint32_t uwVBATVoltage;

uint32_t rtc_val;
uint32_t val, val0;
uint8_t  bat_val, bat_tmp, bat_old;
uint8_t  bat_cnt, bat_cnt1;

RTC_InitTypeDef RTC_InitStructure;
__IO uint32_t   uwLsiFreq = 0;
__IO uint32_t   uwCaptureNumber = 0; 
__IO uint32_t   uwPeriodValue = 0;

uint8_t 	trx_buf[JPEG_PKT][TRX_SIZE];
uint8_t		pkt_cs;
uint8_t		pkt_cs0;
uint8_t		file_cs;
uint8_t		file_pkt_cs;
uint8_t		pkt_cnt;
uint16_t	trx_cnt;

uint8_t		tmp_cs;
uint8_t		standby_flag;

uint8_t		illumin_flag = 0x20;

uint32_t f_setup[4] = { 00,			// f_setup[0] ==> version no
						00,			// f_setup[1][0] ==> illumination flag
						00,			// f_setup[1][2,3,4] ==> reserved.
						00};		// f_setup{2-4][1-4] ==> reserved
uint32_t f_dest;
uint8_t g_tmp;

uint8_t cal_flag;
uint32_t cal_sum[CAL_LINE];
uint8_t white_cnt, black_cnt, cal_point;
uint8_t start_line_pnt;
const uint16_t cal_psoi_buf[11][UPDATE_SIZE] = { // 152 라인
		{0x0300, 0x0601, 0x0723, 0x1000, 0x1179, 0x1200, 0x13F1, 0x8A00, 0x8B6F, 0x8C00, 0x8DEF}, 	// 112
		{0x0300, 0x0601, 0x0723, 0x1000, 0x1181, 0x1200, 0x13F9, 0x8A00, 0x8B77, 0x8C00, 0x8DF7}, 	//120 line
		{0x0300, 0x0601, 0x0723, 0x1000, 0x1189, 0x1201, 0x1301, 0x8A00, 0x8B7F, 0x8C00, 0x8DFF}, 	//128
		{0x0300, 0x0601, 0x0723, 0x1000, 0x1191, 0x1201, 0x1309, 0x8A00, 0x8B87, 0x8C01, 0x8D07}, 	// 136
		{0x0300, 0x0601, 0x0723, 0x1000, 0x1199, 0x1201, 0x1311, 0x8A00, 0x8B8F, 0x8C01, 0x8D0F},	// 144 line
		{0x0300, 0x0601, 0x0723, 0x1000, 0x11A1, 0x1201, 0x1319, 0x8A00, 0x8B97, 0x8C01, 0x8D17},	// 152 line
		{0x0300, 0x0601, 0x072B, 0x1000, 0x11A9, 0x1201, 0x1321, 0x8A00, 0x8B9F, 0x8C01, 0x8D1F}, 	// 160 line
		{0x0300, 0x0601, 0x0733, 0x1000, 0x11B1, 0x1201, 0x1329, 0x8A00, 0x8BA7, 0x8C01, 0x8D27}, 	// 168
		{0x0300, 0x0601, 0x073B, 0x1000, 0x11B9, 0x1201, 0x1331, 0x8A00, 0x8BAF, 0x8C01, 0x8D2F}, 	// 176
		{0x0300, 0x0601, 0x0743, 0x1000, 0x11C1, 0x1201, 0x1339, 0x8A00, 0x8BB7, 0x8C01, 0x8D37}, 	// 184
		{0x0300, 0x0601, 0x074B, 0x1000, 0x11C9, 0x1201, 0x1341, 0x8A00, 0x8BBF, 0x8C01, 0x8D3F} 	// 192
	};
uint32_t uwTimingDelay5;
const char st_line[] = "Start line = ";
const char error[] = "Er";

extern uint8_t jpeg_buf[V_SIZE][H_SIZE];
extern uint8_t rtc_wkup_flag;
extern uint8_t sensor_irq_flag;

/* Private function prototypes -----------------------------------------------*/
//static void SystemClock_Config(void);
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nTime);
void Delay2(__IO uint32_t nTime);
void Set_Delay(__IO uint32_t nTime);
void Set_Delay2(__IO uint32_t nTime);
void Set_Delay3(__IO uint32_t nTime);
void Set_Delay4(__IO uint32_t nTime);

void timer_init(void);
void uart1_init(void);
void uart2_init(void);
void spi_init(void);
void I2c_init(void);
void Image_sensor_init(void);
void sensor_ctrl_Init();
static void EXTI0_Config(void);
static void EXTI5_9_Config(void);
void BSP_PB_Init(Button_TypeDef, ButtonMode_TypeDef);
uint8_t rx1_data_proc(void);
uint8_t rx2_data_proc(void);
void fw_data_gathering(void);
void COM_Init(COM_TypeDef, USART_InitTypeDef*);
void gpio_init(void);
void StandbyRTCMode_Enter(void);

extern void encoding_8line();
extern void pre_encode();
extern void post_encode();
extern void sEE_WriteBuffer(uint8_t*, uint16_t, uint16_t);
extern uint32_t sEE_ReadBuffer(uint8_t*, uint16_t, uint16_t *);
extern void USART_SendData(USART_TypeDef*, uint16_t);
extern uint16_t USART_ReceiveData(USART_TypeDef*);

void writebyte(uint8_t);
void raw_tx(void);								// lji 150119 for debugging

void send_resp(USART_TypeDef*, uint8_t);
char flash_wrt(__IO uint32_t*, uint32_t);
void ADC_Config(void);
void wwdg_irq_setting(void);
void sensor_illumin_setting(uint8_t);

void cal_val_resetting(uint8_t);
void cal_mode(void);

static void     RTC_Config(uint32_t);
static void SYSCLKConfig_STOP(void);

char tx_proc(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */ 
int main(void)
{ uint8_t ch, i, tmp;
  uint32_t flag;
//  uint16_t i, j;

  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/
       startup_stm32f429_439xx.s/startup_stm32f401xx.s or startup_stm32f411xe.s)
       before to branch to application main.
       To reconfigure the default setting of SystemInit() function, 
       refer to system_stm32f4xx.c file */

	/* SysTick end of count event each 10ms */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
  
#if 0
	/* HSI clock selected to output on MCO1 pin(PA8)*/
	RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_1);
#endif

//**************  version no. write in flash ****************
	f_dest = FWDL_FLAG_ADDRESS;

	f_setup[0] = *(uint32_t*)f_dest;				// read ver.
	f_setup[1] = *(uint32_t*)(f_dest+4);			// illumination
	f_setup[2] = *(uint32_t*)(f_dest+8);			// lji 20160428 cal_info
	start_line_pnt = f_setup[2];
	if(start_line_pnt > 10)	start_line_pnt = 5;	// default 152 line
	
	f_setup[3] = *(uint32_t*)(f_dest+12);
	
	flag = (uint32_t)version_no[0]; 	
	flag |= (uint32_t)version_no[1]<<8;
	flag |= (uint32_t)version_no[2]<<16;
	flag |= (uint32_t)version_no[3]<<24;
	
	if(f_setup[0] != flag) {					// lji 150723 반드시 버젼이 다를경우 write 해야 함
		f_setup[0] = flag;				// read ver.
		if (FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3) != FLASH_COMPLETE)
		{
		  /* Error occurred while page erase */
		  return (1);
		}
		for(i=0;i<4;i++) {		
//			ch = flash_wrt(&f_dest, *(f_setup+i/4));
			ch = flash_wrt(&f_dest, f_setup[i]);
			if(ch) 
			  return(ch);		  // flash write error
			f_dest += 4;
		}
	}
	
	gpio_init();
	Delay(1);			// 150126 lji, must delay 10ms after Sensor & BT power sw on
	timer_init();
	Image_sensor_init();
	ch = f_setup[1] & 0x60;			// 00, 20, 40
	if(ch < 0x60)
		sensor_illumin_setting(ch>>5);	// 00 => Low, 01 => Middle, 10 => High
	cal_val_resetting(start_line_pnt);		// lji 20160428 start line setting
	
	uart1_init();
	uart2_init();

	while((ch = st_line[i++]) != 0) {
		USART_SendData(USART1, ch);		// lji 20160501 for debugging
		Delay(1);
	}		
	USART_SendData(USART1, (uint16_t)(start_line_pnt + '0'));		// lji 20160501 for debugging
	Delay(1);
	USART_SendData(USART1, (uint16_t)0x0A);
	
	/* RTC RE-configuration */
	/* Disable the RTC Wakeup Interrupt */
	RTC_ITConfig(RTC_IT_WUT, DISABLE);
	
	/* Disable Wakeup Counter */
	RTC_WakeUpCmd(DISABLE);
	
	 RTC_Config(STNBY_TIME);
		  
  if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
  {
	  GPIOB->BSRRL = GPIO_Pin_14;			  // High lji 160324 for debugging
    /* Clear StandBy flag */
    PWR_ClearFlag(PWR_FLAG_SB);
  
    /* Turn LED4 & LED6 On */
//    BSP_LED_On(LED_4);
//    BSP_LED_On(LED_6);
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET); 	// LED ON
   }
  else {
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET); 	  // LED OFF
  }
  /* Enable WKUP pin	*/
  PWR_WakeUpPinCmd(ENABLE);
  
 
//  Set_Delay2(3); 			  // 30ms, 500ms delay
//  while(uwTimingDelay2) ;
  
  Set_Delay(STOP_PERIOD * 100); 			  // 10ms 단위, sleep mode 진입 타이머, 2초 설정

//  Set_Delay2(50); 			  // 5초,
  Set_Delay3(1); 			  // 10ms 단위, ADC read용 
  
  ADC_Config();
  
  EXTI0_Config();
  EXTI5_9_Config();  

  tx1_gpnt = tx1_ppnt = tx2_gpnt = tx2_ppnt = 0;
  rx_gpnt1 = rx_ppnt1 = rx_gpnt = rx_ppnt = 0;
		
  while (1)
  {
#if 0 
  if(!uwTimingDelay2) {
	LED_toggle_flag ^= 0xff;
	if(LED_toggle_flag) GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET); 	  // LED ON
	else				GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET); 	  // LED OFF
	Set_Delay2(50);			  // 10ms 단위, LED toggle용 
	tgl_cnt++;
  }
#endif

/*The VBAT pin is internally connected to a bridge divider by 2 */
// uwVBATVoltage = (ADC1ConvertedValue*VBATDIV)*3300/0xFFF;
 if(!uwTimingDelay3) {
	Set_Delay3(1);			 // 10ms 단위, ADC read용 
 	val += ADC1ConvertedValue;
 	bat_cnt++;
 }
 if(!((bat_cnt+1)%64)) {		// 640ms마다 밧데리 전압 refresh
 	 val0 = val / 63;
	 ch = (uint32_t)val0%100;
	 bat_tmp = (val0/100)+(ch>49?1:0);
	  if(bat_val == 0)
	  	bat_val = bat_tmp;

// lji 150702 밧데리 전압의 ADC 출력이 불안한 문제 ==> 편법처리
	  if(bat_tmp == bat_old) bat_cnt1++;
	  else {
	  	bat_old = bat_tmp;
		bat_cnt1 = 0;
	  }
	  
	  if(bat_cnt1 > PREC_CNT) {
			bat_val = bat_tmp;
	  	bat_cnt1 = 0;
 }
	  val = bat_cnt = 0;
 }

#if CAL
	if(rx_ppnt1-rx_gpnt1)
		ch = rx1_data_proc();
#endif

	if(sensor_irq_flag & ONE_FRAME_COMPLETE) {
		GPIOC->BSRRH = GPIO_Pin_3;

		GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET); 	  // Sensor standby
		EXTI->IMR &= ~(EXTI_Line0 | EXTI_Line5 | EXTI_Line7| EXTI_Line8);			// exti disable
				
		rsp_flag = rsp2_flag = ACK;
		tx1_gpnt = tx1_ppnt = tx2_gpnt = tx2_ppnt = 0;
//		jpeg_pkt_cnt = jpeg_tx_pnt = rx_cnt = tx_pkt_cnt = 0;
		
#if RAW_DATA
		raw_tx();	// lji 150119 for debugging
#else
		pre_encode();
		encoding_8line();	 // lji 14/11/27
		post_encode();
		
#endif
		sensor_irq_flag = 0;

		EXTI->IMR |= (EXTI_Line0 | EXTI_Line5 | EXTI_Line7| EXTI_Line8);		// exit enable
		
//		Set_Delay(STOP_PERIOD* 100);			// 10ms 단위, sleep mode 진입 타이머, 10초 설정
		GPIOC->BSRRL = GPIO_Pin_3;
	}
	
	Set_Delay4(STOP_PERIOD * 100);			// lji 20160421 tx timeout for android 6.0  기존 2초에서 stop mode 주기로, stop 주기도 2->6초로 늘림
	while(tx_pkt_cnt || (rx_ppnt-rx_gpnt)) {
		tx_proc();
//		if(!uwTimingDelay4) {			// lji 20160420 for debugging
		if((!uwTimingDelay4) || (!tx_pkt_cnt)) {
			tx_pkt_cnt = rx_ppnt = rx_gpnt = 0;
			rsp2_flag = 0x80;
			break;
		}
	}

#if STOP							// lji 150723 rum mode에서 2초 후 stop mode로 들어감
	if(!uwTimingDelay) {

		EXTI->IMR &= ~(EXTI_Line5 | EXTI_Line7| EXTI_Line8);			// exti disable
		
		GPIOB->BSRRH = GPIO_Pin_14; 			// Low lji 160324 for debugging
			GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET); 	  // Sensor standby
								
			/* Enter Stop Mode */
			PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
			
			/* Configures system clock after wake-up from STOP: enable HSE, PLL and select 
			   PLL as system clock source (HSE and PLL are disabled in STOP mode) */
			SYSCLKConfig_STOP();
			
			/* RTC RE-configuration */
			/* Disable the RTC Wakeup Interrupt */
			RTC_ITConfig(RTC_IT_WUT, DISABLE);
			/* Disable Wakeup Counter */
			RTC_WakeUpCmd(DISABLE);
			
			EXTI->IMR |= (EXTI_Line5 | EXTI_Line7| EXTI_Line8);		// exit enable
							
			GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET); 	  // Sensor ready
			
			for(i=0;i<11;i++) {					// lji 160326 BT module 연결상태 체크=> 연결 안된 상태면 BT module power off
				Delay(1);
				if(GPIOA->IDR&0x0800) {
					continue;
				}
				else {
					bt_stat_cnt = 0;
					break;
				}
			}
			if(i == 11) {
				bt_stat_cnt = 0xff;
				rtc_wkup_flag = 0;
				
				RTC_Config(STNBY_TIME);
			}
			else if(rtc_wkup_flag <= 1) {
				RTC_Config(STNBY_TIME/2);
			}
			else {			// lji 160326 rtc_wkup_flag>1, BT 접속 안될경우 stop mode에서 BT Module power off(RTC wakeup 2회시점)
//			else if(rtc_wkup_flag>1){			// lji 160326 BT 접속 안될경우 stop mode에서 BT Module power off(RTC wakeup 2회시점)
				rtc_wkup_flag = 0;
				/* Clear Power WakeUp (CWUF) pending flag */
				PWR_ClearFlag(PWR_FLAG_WU);
					
				  /* Clear WakeUp (WUTF) pending flag */
				  RTC_ClearFlag(RTC_FLAG_WUTF);
				  
				  /* Disable the RTC Wakeup Interrupt */
				  RTC_ITConfig(RTC_IT_WUT, DISABLE);
				  
				  /* Disable Wakeup Counter */
				  RTC_WakeUpCmd(DISABLE);
				  GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);		// BT & Sensor Power OFF
				  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);		// Sensor standby
				  
				  /* Enter Stop Mode */
				  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
				  
				  /* Configures system clock after wake-up from STOP: enable HSE, PLL and select 
					 PLL as system clock source (HSE and PLL are disabled in STOP mode) */
				  SYSCLKConfig_STOP();
				  /* RTC RE-configuration */
				  /* Disable the RTC Wakeup Interrupt */
				  RTC_ITConfig(RTC_IT_WUT, DISABLE);
				  
				  /* Disable Wakeup Counter */
				  RTC_WakeUpCmd(DISABLE);
				  
				  RTC_Config(STNBY_TIME);
				  GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);		// BT & Sensor Power ON
				  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET); 	// Sensor ready
			}

			Set_Delay(STOP_PERIOD * 100);			// 10ms 단위, sleep mode 진입 타이머, 5초 설정
			rsp2_flag = 0x80;
			GPIOB->BSRRL = GPIO_Pin_14;				// High lji 160324 for debugging
	  }
#endif

#if 0					// lji 160324 standby mode 다시 추가 => BT 접속이 안되는 상태로 1분 이후에는 standby mode 진입
	  if((bt_stat_cnt != 0xff) && (rtc_wkup_flag>1)) { 		// BT접속 안된상태에서 RTC int. ==> standy mode로 들어감
			rtc_wkup_flag = 0;
  			standby_flag = 0xff;
	  }
#endif

// lji 150723 reset후 120초지나서, stop mode 상태에서RTC wake-up에 의하여 stop mode를 벗어난 직후 standby mode로 들어감
#if STANDBY
	  if(standby_flag) {
		  GPIOA->BSRRH = GPIO_Pin_13;		  // Low lji 160324 for debugging
	  /* Clear Power WakeUp (CWUF) pending flag */
	  PWR_ClearFlag(PWR_FLAG_WU);
		  
		/* Clear WakeUp (WUTF) pending flag */
		RTC_ClearFlag(RTC_FLAG_WUTF);
		
		/* Disable the RTC Wakeup Interrupt */
		RTC_ITConfig(RTC_IT_WUT, DISABLE);
		
		/* Disable Wakeup Counter */
		RTC_WakeUpCmd(DISABLE);
		
			
	  send_resp(USART2, DISCONNECT);
	  while((USART2->CR1 & TXEIE)) ;			  // TX 전송 중이면 기다림.
	  GPIOA->BSRRL = GPIO_Pin_13;			// High lji 160324 for debugging
	  Set_Delay(70);			  // BT module에서의 송신을 위해 700 msec 동안 대기
	  while(uwTimingDelay) ;
// standby mode로 들어가기전에 BT 모듈과 이미지센서의 3.3V를 OFF 시킴lji 150123
	  GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);		// BT & Sensor Power OFF
	  
	  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);		// Sensor standby
	  
	  /* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
	  PWR_EnterSTANDBYMode();
	}
#endif
  }
}

/**
  * @brief  Configure the TIM3 Ouput Channels.
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* GPIOA clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* GPIOA Configuration: TIM1 CH1 (PA6)  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

  /* Connect TIM3 pins to AF2 */  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
}


void timer_init(void)
{
	/* -----------------------------------------------------------------------
	  TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles.
	  
	  In this example TIM3 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1), 
	  since APB1 prescaler is different from 1.   
		TIM3CLK = 2 * PCLK1  
		PCLK1 = HCLK / 4 
		=> TIM3CLK = HCLK / 2 = SystemCoreClock /2
			
	  To get TIM3 counter clock at 21 MHz, the prescaler is computed as follows:
		 Prescaler = (TIM3CLK / TIM3 counter clock) - 1
		 Prescaler = ((SystemCoreClock /2) /21 MHz) - 1
												
	  To get TIM3 output clock at 30 KHz, the period (ARR)) is computed as follows:
		 ARR = (TIM3 counter clock / TIM3 output clock) - 1
			 = 665
					
	  TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
	  TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
	  TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
	  TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%
	
	  Note: 
	   SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
	   Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	   function to update SystemCoreClock variable value. Otherwise, any configuration
	   based on this variable will be incorrect.	
	----------------------------------------------------------------------- */	 

	TIM_Config();
	
	
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 21000000) - 1;
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = PERIOD_VALUE;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = PULSE1_VALUE;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
		
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}

void Image_sensor_init(void)
{ int i;
  uint8_t *tmp1;

	I2c_init();
	
	sensor_ctrl_Init();

	/* Image Sensor init. CMD */

#if 1
	for(i=0;i<IMG_SENSOR_SIZE;i++) {
		tmp1 = (uint8_t *)(PO8030D+i);
		sEE_WriteBuffer(tmp1, *(tmp1+1), 1); 
	}
#endif

}

void sensor_ctrl_Init()
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /* GPIOB Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  #if 1
  /* lji 141118, PB13 ==> Sensor Reset pin */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;

  GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET); 		// sensor_reset
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;

  GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET); 		// standby
  
  GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET); 		// 타이밍이 중요함
  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET); 	// standby
  #endif
  }

void sensor_illumin_setting(uint8_t t)
{ uint8_t i, *tmp1;
		
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);	  // Sensor ready
	Set_Delay3(2);			 // 20ms delay => sensor ready
		while(uwTimingDelay3) ;

	for(i=0;i<SENSOR_SIZE_2;i++) {
		tmp1 = (uint8_t *)(illumin_cntrl+i);
		sEE_WriteBuffer(tmp1, *(tmp1+1), 1); 
	}

	for(i=0;i<6;i++) {								// 3단게 밝기조절 설정
		tmp1 = (uint8_t *)(illumin[t]+i);
		sEE_WriteBuffer(tmp1, *(tmp1+1), 1); 
	}
}

/**
  * @brief Configure the USART Device
  * @param  None
  * @retval None
  */
void uart1_init(void)
{ USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(USART1_TX_GPIO_CLK | USART1_RX_GPIO_CLK, ENABLE);
	
	/* Enable USART clock */
	USART1_CLK_INIT(USART1_CLK, ENABLE);
	
	/* Connect USART pins to AF7 */
	GPIO_PinAFConfig(USART1_TX_GPIO_PORT, USART1_TX_SOURCE, USART1_TX_AF);
	GPIO_PinAFConfig(USART1_RX_GPIO_PORT, USART1_RX_SOURCE, USART1_RX_AF);
	
	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;		//GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN;
	GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
	GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStructure);


	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART1, &USART_InitStructure);
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
	
	/* Enable the Rx interrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void uart2_init(void)
{ USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(USART2_TX_GPIO_CLK | USART2_RX_GPIO_CLK, ENABLE);
	
	/* Enable USART clock */
	USART2_CLK_INIT(USART2_CLK, ENABLE);
	
	/* Connect USART pins to AF7 */
	GPIO_PinAFConfig(USART2_TX_GPIO_PORT, USART2_TX_SOURCE, USART2_TX_AF);
	GPIO_PinAFConfig(USART2_RX_GPIO_PORT, USART2_RX_SOURCE, USART2_RX_AF);
	
	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = USART2_TX_PIN;
	GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = USART2_RX_PIN;
	GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStructure);
	
#if DISCOVERY
	USART_InitStructure.USART_BaudRate = 460800;			//921600;			//115200;
#else
	USART_InitStructure.USART_BaudRate = 460800;			//115200;
#endif
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART2, &USART_InitStructure);
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USART */
	USART_Cmd(USART2, ENABLE);
	
	/* Enable the Rx interrupt */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

// lji 20160502 for calibration
uint8_t rx1_data_proc()
{ char ch;
  uint8_t i;

	ch = aRxBuffer1[rx_gpnt1++];
	rx_cnt1--;
	if(rx_gpnt1 >= RXBUFFERSIZE) rx_gpnt1 = 0;

	switch(t1_state) {
		case NORMAL_STATE:
			if(ch == 'U')
				t1_state = STX_STATE;
			break;
		case STX_STATE:
			if(ch == 'U')
				t1_state = NACK_STATE;
			else 
				t1_state = NORMAL_STATE;
			break;
		case NACK_STATE:
			if((ch >= '0') && (ch <= ':')) {
				USART_SendData(USART1, ch); 	// lji 20160501 for debugging
				Delay(1);
				USART_SendData(USART1, (uint16_t)0x0A);
			
				cal_val_resetting(ch - '0');
				
			// flash write
				f_dest = FWDL_FLAG_ADDRESS;
				
				f_setup[0] = *(uint32_t*)f_dest;				// read ver.
				f_setup[1] = *(uint32_t*)(f_dest+4);			// illumination
				f_setup[2] = *(uint32_t*)(f_dest+8);			// start line pointer
				f_setup[2] = ch - '0';
				f_setup[3] = *(uint32_t*)(f_dest+12);
				
				if (FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3) != FLASH_COMPLETE)
				{
				  /* Error occurred while page erase */
				}
				for(i=0;i<4;i++) {		
					flash_wrt(&f_dest, f_setup[i]);
					f_dest += 4;
				}
			}
			else {
				i = 0;
				while((ch = error[i++]) != 0) {
					USART_SendData(USART1, ch); 	// lji 20160501 for debugging
					Delay(1);
				}
				USART_SendData(USART1, (uint16_t)0x0A);
			}
			t1_state  = NORMAL_STATE;
			break;
		default :
			break;
	}
	return(ch);
}

uint8_t rx2_data_proc()
{ char ch, i;

    ch = aRxBuffer[rx_gpnt++];
	rx_cnt--;
	if(rx_gpnt >= RXBUFFERSIZE) rx_gpnt = 0;
		
	switch(t2_state) {
		case NORMAL_STATE :
			if(ch == STX) {
				t2_state = STX_STATE;
				t2_cnt = 0;
			}
			break;
		case STX_STATE:
			if(ch == R_CMD)
				t2_state = NACK_STATE;
				else {
				t2_state = NORMAL_STATE;
				}
			break;
		case NACK_STATE:		// 이미지 전송모드의 응답코드
			t2_cnt++;
			if(t2_cnt == 1) t2_no =  ch;
			else if(t2_cnt >= 2) {
				if(ch == ACK)	rsp2_flag = ACK;
				else if(ch == NAK)	rsp2_flag = NAK;
				t2_cnt = 0;
				t2_state = NORMAL_STATE;
				if(t2_no&0x80) {
					g_tmp = t2_no & 0x60;				// bit 5,6 ==> illumination flag 
					if( g_tmp != (uint8_t)(f_setup[1]&0x60)) {						
						f_dest = FWDL_FLAG_ADDRESS;
						
						f_setup[0] = *(uint32_t*)f_dest;				// read ver.
						
						f_setup[1] = *(uint32_t*)(f_dest+4);
						f_setup[1] &= 0xffffff00;
						f_setup[1] |= g_tmp;
						
						f_setup[2] = *(uint32_t*)(f_dest+8);
						f_setup[3] = *(uint32_t*)(f_dest+12);
						
						if (FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3) != FLASH_COMPLETE)
						{
						  /* Error occurred while page erase */
						  return (1);
						}
						for(i=0;i<4;i++) {		
//							ch = flash_wrt(&f_dest, *(f_setup+i/4));
							ch = flash_wrt(&f_dest, f_setup[i]);
							if(ch) {
							  GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);	// for debugging
//							  return(ch);		  // flash write error
							}
							f_dest += 4;
						}
						sensor_illumin_setting(g_tmp>>5);	// 00 => Low, 01 => Middle, 10 => High
	//					sensor_illumin_setting(2);	// 00 => Low, 01 => Middle, 10 => High
					}
					rsp2_flag |= 0x80;
				}
			}
			else ;
			break;
		default :
			t2_cnt = 0;
			t2_state = NORMAL_STATE;				// lji 150612 debugging ???
			break;
	}
	return(ch);
}

//#pragma optimize=s low no_code_motion
void writebyte(uint8_t ch)
{
  		
		trx_buf[jpeg_pkt_cnt][TRX_HDR+trx_cnt]= ch;
		//		PACKET CS
		pkt_cs += ch;
		//		FILE CS
		file_cs += ch;
		
		trx_cnt++;
		
		if(trx_cnt == P_SIZE) {
			trx_cnt = 0;
			trx_buf[jpeg_pkt_cnt][A_STX] = STX;
			trx_buf[jpeg_pkt_cnt][A_CMD] = 'A';
			trx_buf[jpeg_pkt_cnt][A_P_NO] = pkt_cnt;
			trx_buf[jpeg_pkt_cnt][A_F_CS] = bat_val;		// lji 20150504
	
	//		PACKET CS
			pkt_cs += trx_buf[jpeg_pkt_cnt][1]+trx_buf[jpeg_pkt_cnt][2]+trx_buf[jpeg_pkt_cnt][A_F_CS];
			trx_buf[jpeg_pkt_cnt][A_P_CS] = ~pkt_cs+1;
			if(jpeg_pkt_cnt<(JPEG_PKT-1)) {
				pkt_cnt++;
				jpeg_pkt_cnt++;
				tx_pkt_cnt++;
			}
			pkt_cs = 0;
	}
	tx_proc();
}

void raw_tx()								// lji 150119 for debugging
{ 
}

void fw_data_gathering()
{
}

void ADC_Config()
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	ADC_InitTypeDef 	  ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef 	  DMA_InitStructure;
	
      /* GPIOA Peripheral clock enable */
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
      
      /* lji 150217 Configure PA.01 (ADC1 Channel14) as analog input -------------------------*/
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
      GPIO_Init(GPIOA, &GPIO_InitStruct);
      
      GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
      GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
      GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;	// lji 150226 ADC GND
      GPIO_Init(GPIOA, &GPIO_InitStruct);
      
      GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET); 	  // lji 150217 LOW (ADC1의 GND)

	  /* Enable peripheral clocks *************************************************/
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	  RCC_APB2PeriphClockCmd(ADCx_CLK, ENABLE);
	  
	  /* DMA2_Stream0 channel0 configuration **************************************/
	  DMA_DeInit(DMA2_Stream0);
	  DMA_InitStructure.DMA_Channel = DMA_CHANNELx;  
	  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADCx_DR_ADDRESS;
	  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1ConvertedValue;
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	  DMA_InitStructure.DMA_BufferSize = 1;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;		 
	  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  DMA_Init(DMA_STREAMx, &DMA_InitStructure);
	  /* DMA2_Stream0 enable */
	  DMA_Cmd(DMA_STREAMx, ENABLE);
		
	  /* ADC Common Init **********************************************************/
	  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	  ADC_CommonInit(&ADC_CommonInitStructure);
	  
	  /* ADC1 Init ****************************************************************/
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_NbrOfConversion = 1;
	  ADC_Init(ADCx, &ADC_InitStructure);
	  
	  /* Enable ADC1 DMA */
	  ADC_DMACmd(ADC1, ENABLE);
	  
	  /* ADC1 regular channel1 configuration ******************************/
//	  ADC_RegularChannelConfig(ADCx, ADC_Channel_Vbat, 1, ADC_SampleTime_15Cycles);
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_28Cycles);
	  	  
	  /* Enable DMA request after last transfer (Single-ADC mode) */
	  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	  
	  /* Enable ADC1 **************************************************************/
	  ADC_Cmd(ADC1, ENABLE);
	  
	  /* Start ADC1 Software Conversion */ 
//	  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	  ADC_SoftwareStartConv(ADC1);
}

/**
  * @brief  Configures EXTI Line9-5 (connected to PE6 pin) in interrupt mode
  * @param  None
  * @retval None
  */
static void EXTI0_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
	
	/* Enable GPIOA clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Configure PA0 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Connect EXTI Line0 to PA0 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	
	/* Configure EXTI Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;			//EXTI_Trigger_Rising;	
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* Enable and set EXTI Line0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures EXTI Line9-5 (connected to PE6 pin) in interrupt mode
  * @param  None
  * @retval None
  */
static void EXTI5_9_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOA clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

#if 1
	/* Configure PA5 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect EXTI Line5 to PA5 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5);

	/* Configure EXTI Line5  VSYNC */
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		// EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
#endif
	
#if 1
	/* Configure PA7 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect EXTI Line7 to PA7 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource7);

	/* Configure EXTI Line7  H_SYNC */
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
#endif

#if 1  
	/* Configure PA8 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect EXTI Line8 to PA8 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);

	/* Configure EXTI Line8 PIXEL CLK */
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		// EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
#endif	  
	
	/* Enable and set EXTI9_5 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;		//0x0F; // lji 150808 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;				//0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  
	NVIC_Init(&NVIC_InitStructure);
}

void gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	/* GPIOA Peripheral clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* lji 150217 Configure PA.01 (ADC1 Channel14) as analog input -------------------------*/
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
//	GPIO_Init(GPIOA, &GPIO_InitStruct);
#if 1		
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;	// lji 150116 PA12(BT, Sensor Power Ctrl)
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET); 	  // BT & Sensor Power OFF
	GPIO_WriteBit(GPIOA, GPIO_Pin_13, Bit_SET);	  // 
	GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET); 	  // BT & Sensor Power ON
#endif	
	/* Configure PA11 pin as input floating */
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
#if DISCOVERY
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
#else
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;	// Pin_11 => BT conn. stat, Pin_13 => Charger STAT.
#endif
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* GPIOB Peripheral clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStruct.GPIO_Pin = 0x00ff;					// lji 141122 image data B0~B7 Inupt setting
	GPIO_Init(GPIOB, &GPIO_InitStruct);
		
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;		// Sensor LED, debug port ( FLT )
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET); 	  // LED reset
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);		  // debug port

#if 1	/* GPIOC Peripheral clock enable, debugging pin */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_3, Bit_SET);  
	GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_SET); 
	GPIO_WriteBit(GPIOC, GPIO_Pin_5, Bit_SET); 
#endif

#if DISCOVERY
/* GPIOD data bit 3 ==> charger stat signal.  for discovery kit debugging*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
#endif
}

/**
  * @brief  Configures the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void RTC_Config(uint32_t val)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  EXTI_InitTypeDef EXTI_InitStructure;

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* LSI used as RTC source clock */
	/* The RTC Clock may varies due to LSI frequency dispersion. */   
	/* Enable the LSI OSC */ 
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	/* Calendar Configuration with LSI supposed at 32KHz */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv  = 0xFF; /* (32KHz / 128) - 1 = 0xFF*/
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);  

	/* EXTI configuration *******************************************************/
	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the RTC Wakeup Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  

	/* Configure the RTC WakeUp Clock source: CK_SPRE (1Hz) */
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
//	RTC_SetWakeUpCounter(0x0);
	RTC_SetWakeUpCounter(val); // lji 150723, 최초 RTC Wake-up 시간 ==> value * 2초

	/* Enable the RTC Wakeup Interrupt */
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	/* Enable Wakeup Counter */
	RTC_WakeUpCmd(ENABLE);
}

/**
  * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
  *         and select PLL as system clock source.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
  /* After wake-up from STOP reconfigure the system clock */
  /* Enable HSE */
//  RCC_HSEConfig(RCC_HSE_ON);
  RCC_HSICmd(ENABLE);
  
  /* Wait till HSE is ready */
//  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {
  }
  
  /* Enable PLL */
  RCC_PLLCmd(ENABLE);
  
  /* Wait till PLL is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }
  
  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  /* Wait till PLL is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x08)
  {
  }
}

char flash_wrt(__IO uint32_t* dl_flag_addr, uint32_t flag)
{
	FLASH_Unlock(); 
	
	/* Clear pending flags (if any) */	
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
					FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	if (FLASH_ProgramWord(*dl_flag_addr, flag) == FLASH_COMPLETE) {
		  Delay2(1);
		 /* Check the written value */
		  if (*(uint32_t*)*dl_flag_addr != flag)
		  {
			/* Flash content doesn't match SRAM content */
			return(1);
		  }
		  return(0);		// SUCCESS
	}
	else	return(1);
}

void send_resp(USART_TypeDef* USARTx, uint8_t ch)
{	
	jpeg_tx_pnt = 0;
	tx2_ppnt = 0;
	tx2_gpnt = 0;
	
	trx_buf	[jpeg_tx_pnt][tx2_ppnt++] = STX;
	trx_buf[jpeg_tx_pnt][tx2_ppnt++] = 'C';
	trx_buf[jpeg_tx_pnt][tx2_ppnt++] = 0x7e;			// 0xfe;
	trx_buf[jpeg_tx_pnt][tx2_ppnt++] = ch&0x7f;
	while((USARTx->CR1 & TXEIE)) ;				// TX 전송 중이면 기다림.
	
/* Enable the Tx buffer empty interrupt */
	USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
}

// #pragma optimize=s low no_code_motion
char tx_proc(void)
{ uint8_t i;

//	if(rx_cnt)				// lji 150805 rx_cnt 값이 0이 되는 경우가 잇음
	if(rx_ppnt-rx_gpnt)
		rx2_data_proc();
  
//	if(((jpeg_pkt_cnt>jpeg_tx_pnt) && !(USART1->CR1 & TXEIE)) || (rsp2_flag == NAK)) {
	if((tx_pkt_cnt && !(USART1->CR1 & TXEIE)) || ((rsp2_flag&0x7f) == NAK)) {
//		tx_proc();
		if(rsp2_flag == ACK) {
			tx2_ppnt = TRX_SIZE;
			if(trx_buf[jpeg_tx_pnt][A_P_NO] & 0x80) {		// lji 20151228 마지막 패킷
				for(i=0;i<DUMMY_BYTES;i++)
					trx_buf[jpeg_tx_pnt+1][i] = 0xfe;
				tx2_ppnt = TRX_SIZE+DUMMY_BYTES;		// 마지막 패킷에서는 더미바이트를 더 보냄
			}
			tx2_gpnt = 0;
			rsp2_flag = 0;
			if(tx_pkt_cnt)
			tx_pkt_cnt--;
			/* Enable the Tx buffer empty interrupt */
#if BT_MODE
			USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
#else
			USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
#endif
		}
		else if(rsp2_flag == NAK) {
			retry_cnt++;
			if(retry_cnt > 2) {
				retry_cnt = 0;
				rsp2_flag = 0;
				return 1;
			}
			else {
				tx2_ppnt = TRX_SIZE;
				if(trx_buf[jpeg_tx_pnt][A_P_NO] & 0x80) {		// lji 20151228
					for(i=0;i<DUMMY_BYTES;i++)
						trx_buf[jpeg_tx_pnt+1][i] = 0xfe;
					tx2_ppnt = TRX_SIZE+DUMMY_BYTES;		// 마지막 패킷에서는 더미바이트를 더 보냄
				}
				tx2_gpnt = 0;
				rsp2_flag = 0;
				if(jpeg_tx_pnt)
				jpeg_tx_pnt--;					// NAK 이면 바로 전 패킷을 전송
			}
			/* Enable the Tx buffer empty interrupt */
#if BT_MODE
			USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
#else
			USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
#endif
		}
		else ;
	}
	return 0;	
}

void wwdg_irq_setting(void)
{ NVIC_InitTypeDef   NVIC_InitStructure;

	/* Configure one bit for preemption priority */
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

 /* Enable the WWDG Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}

void Delay2(__IO uint32_t nTime)
{ 
  uwTimingDelay2 = nTime;

  while(uwTimingDelay2 != 0);
}

void Set_Delay(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;
}

void Set_Delay2(__IO uint32_t nTime)
{ 
  uwTimingDelay2 = nTime;
}

void Set_Delay3(__IO uint32_t nTime)
{ 
  uwTimingDelay3 = nTime;
}

void Set_Delay4(__IO uint32_t nTime)
{ 
  uwTimingDelay4 = nTime;
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
#if 0
	// lji 150612 BT_STAT line은 200ms pulse(100ms H, 100ms L), 10ms timer로 20ms마다 체크해서 H구간이 10번이상이면 연결
	if(uwTimingDelay%2) {		// every 20ms
#if DISCOVERY
		if(GPIOA->IDR&0x8000) {			// PA15 High
#else
		if(GPIOA->IDR&0x0800) { 		// PA11 High
#endif
			if(bt_stat_cnt <= BT_PULSE_CNT)
				bt_stat_cnt++;
		}
		else {							// PA11(BT_STAT) signal Low ==> BT disconnection
			if(bt_stat_cnt == 0xff) {	// lji 160324 BT 연결후 disconnect 된 시점에서 RTC 재설정 함
				/* RTC RE-configuration */
				/* Disable the RTC Wakeup Interrupt */
				RTC_ITConfig(RTC_IT_WUT, DISABLE);
				/* Disable Wakeup Counter */
				RTC_WakeUpCmd(DISABLE);
				/* RTC 재설정 */
				RTC_Config();
			}
			bt_stat_cnt = 0;
		}
	}
#endif  
  }
	
  if (uwTimingDelay2 != 0x00)
  { 
    uwTimingDelay2--;
  }
 
  if (uwTimingDelay3 != 0x00)
  { 
    uwTimingDelay3--;
  }
  
  if (uwTimingDelay4 != 0x00)
  { 
    uwTimingDelay4--;
  }
  if (uwTimingDelay5 != 0x00)
    uwTimingDelay5++;
}
  
void cal_val_resetting(uint8_t n)
  { uint8_t i, *tmp1;
  
	  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET); 	// Sensor ready
	  Set_Delay3(2);		   // 20ms delay => sensor ready
		  while(uwTimingDelay3) ;
  
	  for(i=0;i<UPDATE_SIZE;i++) {
		  tmp1 = (uint8_t *)(cal_psoi_buf[n]+i);
		  sEE_WriteBuffer(tmp1, *(tmp1+1), 1); 
	  }
}

/**
  * @brief  This function handles the test program fail.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
//  while(1)
//  {
//  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
