/*				     _
          _         | |
        _| |_ _____ | | _____  _____      ____
	   (_   _) ___ || |( ___ ||     | () / ___)
		 | |_| ____ | || ____ | | | || |( (___
     	  \__)_____)|_ )_____)|_|_|_||_| \____)
    (C)2017 telemic


*/
#include "board.h"
#include "sx1272.h"
#include "radio.h"

/*!
 * Vref values definition
 */
#define PDDADC_VREF_BANDGAP                             1224 // mV
#define PDDADC_MAX_VALUE                                4096
/*!
 * Battery level ratio (battery dependent)
 */
#define BATTERY_STEP_LEVEL                          0.23

/*!
 * Unique Devices IDs register set ( STM32L1xxx )
 */
#define         ID1                                 ( 0x1FF80050 )
#define         ID2                                 ( 0x1FF80054 )
#define         ID3                                 ( 0x1FF80064 )

/* MCU objects
 */
//Gpio_t PB_3;
COMP_HandleTypeDef hcomp;
Gpio_t HLpin;

TIM_HandleTypeDef htim21;


//No need to initiate the interrupt on IOD0 because its already initiated with command radio.init in the LoRaMac.c
//see sx1272-board.c all the sx1272 functions are defined in the Radio struct
//RadioEvents_t  *RadioEvents;

DioIrqHandler* DioIrq0[16] = { SX1272OnDio0Irq};

/*!
 * Initializes the unused GPIO to a know status
 */
static void BoardUnusedIoInit( void );

/*!
 * System Clock Configuration
 */
static void SystemClockConfig( void );

/*!l
 * Used to measure and calibrate the system wake-up time from STOP mode
 */
static void CalibrateSystemWakeupTime( void );

/*!
 * System Clock Re-Configuration when waking up from STOP mode
 */
static void SystemClockReConfig( void );

/*!
 * Timer used at first boot to calibrate the SystemWakeupTime
 */
static TimerEvent_t CalibrateSystemWakeupTimeTimer;
/*!
 * Timer used for changing between 3/4VREF and 1/4VREF
 */
//static TimerEvent_t Cap_DisCh_timer;

/*!
 * Flag to indicate if the MCU is Initialized
 */
static bool McuInitialized = false;

/*!
 * Flag to indicate if the SystemWakeupTime is Calibrated
 */
static bool SystemWakeupTimeCalibrated = false;

/*!
 * Callback indicating the end of the system wake-up time calibration
 */
static void OnCalibrateSystemWakeupTimeTimerEvent( void )
{
    SystemWakeupTimeCalibrated = true;
}


/*
void BoardInitPeriph( void )
{
    // Init the GPIO extender pins
	GpioInit( &Led4, LED_4, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1 );

     Switch LED 1, 2, 3, 4 OFF
     GpioWrite( &Led4, 0 );
}
*/
void BoardInitMcu( void )
{
    //GPIO_InitTypeDef GPIO_InitStruct;

    if( McuInitialized == false )
    {
        HAL_Init( );
        SystemClockConfig( );
        SpiInit( &SX1272.Spi, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, RADIO_NSS );
        SX1272IoInit( );
        RtcInit( );
        RTCCalibration();
        BoardUnusedIoInit( );
        //COMP_Init();
        GpioMcuInit( &HLpin, PB_3, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
        HAL_Delay(10000);

    }
    else
    {
        SystemClockReConfig( );
        SpiInit( &SX1272.Spi, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, RADIO_NSS );
        SX1272IoInit( );
    }

    SX1272IoIrqInit(DioIrq0);

    if( McuInitialized == false )
    {
        McuInitialized = true;
        if( GetBoardPowerSource( ) == BATTERY_POWER )
        {
            CalibrateSystemWakeupTime( );
        }
    }
}



void BoardDeInitMcu( void )
{

    SpiDeInit( &SX1272.Spi );
    SX1272IoDeInit( );

    //GpioInit( &ioPin, OSC_HSE_IN, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 1 );
    //GpioInit( &ioPin, OSC_HSE_OUT, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 1 );

    //GpioMcuInit( &ioPin, OSC_LSE_IN, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_DOWN, 1 );
    //GpioMcuInit( &ioPin, OSC_LSE_OUT, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_DOWN, 1 );

}

uint32_t BoardGetRandomSeed( void )
{
    return ( ( *( uint32_t* )ID1 ) ^ ( *( uint32_t* )ID2 ) ^ ( *( uint32_t* )ID3 ) );
}

void BoardGetUniqueId( uint8_t *id )
{
    id[7] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 24;
    id[6] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 16;
    id[5] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 8;
    id[4] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) );
    id[3] = ( ( *( uint32_t* )ID2 ) ) >> 24;
    id[2] = ( ( *( uint32_t* )ID2 ) ) >> 16;
    id[1] = ( ( *( uint32_t* )ID2 ) ) >> 8;
    id[0] = ( ( *( uint32_t* )ID2 ) );
}

uint16_t BoardMeasureVdd( void )
{
    uint16_t MeasuredLevel = 0;
    uint32_t milliVolt = 0;


    // Read the current Voltage
    MeasuredLevel = ADCMeasure();


    // We don't use the VREF from calibValues here.
    // calculate the Voltage in miliVolt
    milliVolt = ( uint32_t )PDDADC_VREF_BANDGAP * ( uint32_t )PDDADC_MAX_VALUE;
    milliVolt = milliVolt / ( uint32_t ) MeasuredLevel;

    return ( uint16_t ) milliVolt;
}




uint8_t BoardGetBatteryLevel( void )
{
    uint8_t batteryLevel = 0;
    uint16_t measuredLevel = 0;

    measuredLevel = BoardMeasureVdd( );

    if( measuredLevel >= 3000 )
    {
        batteryLevel = 254;
    }
    else if( measuredLevel <= 2400 )
    {
        batteryLevel = 1;
    }
    else
    {
        batteryLevel = ( measuredLevel - 2400 ) * BATTERY_STEP_LEVEL;
    }
    return batteryLevel;
}

static void BoardUnusedIoInit( void )
{

#if defined( USE_DEBUGGER )
    HAL_DBGMCU_EnableDBGStopMode( );
    HAL_DBGMCU_EnableDBGSleepMode( );
    HAL_DBGMCU_EnableDBGStandbyMode( );
#else
    HAL_DBGMCU_DisableDBGSleepMode( );
    HAL_DBGMCU_DisableDBGStopMode( );
    HAL_DBGMCU_DisableDBGStandbyMode( );

#endif
}

void SystemClockConfig( void )
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    __HAL_RCC_PWR_CLK_ENABLE( );

    __HAL_PWR_VOLTAGESCALING_CONFIG( PWR_REGULATOR_VOLTAGE_SCALE1 );

    //TODO: update this
    RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI
    								  | RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 0x08;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_6;
    RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
    HAL_RCC_OscConfig( &RCC_OscInitStruct );

    RCC_ClkInitStruct.ClockType = ( RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 );
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_1 );

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    HAL_RCCEx_PeriphCLKConfig( &PeriphClkInit );

    HAL_SYSTICK_Config( HAL_RCC_GetHCLKFreq( ) / 1000 );

    HAL_SYSTICK_CLKSourceConfig( SYSTICK_CLKSOURCE_HCLK );

    /*    HAL_NVIC_GetPriorityGrouping*/
    //HAL_NVIC_SetPriorityGrouping( NVIC_PRIORITYGROUP_4 );

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority( SysTick_IRQn, 0, 0 );
}

void CalibrateSystemWakeupTime( void )
{
    if( SystemWakeupTimeCalibrated == false )
    {
        TimerInit( &CalibrateSystemWakeupTimeTimer, OnCalibrateSystemWakeupTimeTimerEvent );
        TimerSetValue( &CalibrateSystemWakeupTimeTimer, 1000 );
        TimerStart( &CalibrateSystemWakeupTimeTimer );
        while( SystemWakeupTimeCalibrated == false )
        {
            TimerLowPowerHandler( );
        }
    }
}

void SystemClockReConfig( void )
{
    __HAL_RCC_PWR_CLK_ENABLE( );
    __HAL_PWR_VOLTAGESCALING_CONFIG( PWR_REGULATOR_VOLTAGE_SCALE1 );

    /* Enable HSI */
    __HAL_RCC_HSI_CONFIG( RCC_HSI_ON );

    /* Wait till HSI is ready */
    while( __HAL_RCC_GET_FLAG( RCC_FLAG_HSIRDY ) == RESET )
    {
    }

    /* Enable PLL */
    __HAL_RCC_PLL_ENABLE( );

    __HAL_RCC_PLL_CONFIG(RCC_PLLSOURCE_HSI, 6, 3);

    /* Wait till PLL is ready */
    while( __HAL_RCC_GET_FLAG( RCC_FLAG_PLLRDY ) == RESET )
    {
    }

    /* Select PLL as system clock source */
    __HAL_RCC_SYSCLK_CONFIG ( RCC_SYSCLKSOURCE_PLLCLK );

    /* Wait till PLL is used as system clock source */
    while( __HAL_RCC_GET_SYSCLK_SOURCE( ) != RCC_SYSCLKSOURCE_STATUS_PLLCLK )
    {
    }
}


//water level 0 low, 1 high. This is assuming that we will send message every given time.
// The other option is to send a message only when the water level is high/low.

uint32_t  Water_Level(void)
{

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	  hcomp.Instance = COMP2;
	  hcomp.Init.InvertingInput = COMP_INPUT_MINUS_VREFINT;	//internal: COMP_INPUT_MINUS_VREFINT; external: COMP_INPUT_MINUS_IO2
	  hcomp.Init.NonInvertingInput = COMP_INPUT_PLUS_IO4;		//
	  hcomp.Init.LPTIMConnection = COMP_LPTIMCONNECTION_DISABLED;
	  hcomp.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
	  hcomp.Init.Mode = COMP_POWERMODE_ULTRALOWPOWER;
	  hcomp.Init.WindowMode = COMP_WINDOWMODE_DISABLE;
	  hcomp.Init.TriggerMode = COMP_TRIGGERMODE_NONE;
	  if (HAL_COMP_Init(&hcomp) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  COMP2 -> CSR |= 0x9;

	GpioMcuWrite(&HLpin, 0);
	HAL_Delay(1);

    uint32_t ChTime = 0;

    GpioMcuWrite(&HLpin, 1);
	while (!((COMP2 ->CSR) & (0x1 << 30)))
	{    ChTime++;  }

	/*COMP2 ->CSR = 0;
	COMP2 ->CSR = (0x4 << 4) | 0x8;
	COMP2 ->CSR |= 0x1;
	GpioMcuWrite(&HLpin, 0);
	//TimerStart(&Cap_DisCh_timer);
	while(((COMP2 ->CSR) & (0x1 << 30)));
	{	DisChTime++;  }
	//TimerStop(&Cap_DisCh_timer);

	//DisChTime = TimerGetValue();
	//TimerReset(&Cap_DisCh_timer);*/

	GpioMcuWrite(&HLpin, 0);
	COMP2 ->CSR = 0;

	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_SYSCFG_CLK_DISABLE();


	//V_c = BoardMeasureVdd() * (1-e(-DisChTime/rc))
	return ChTime;
}

void SysTick_Handler( void )
{
    HAL_IncTick( );
    HAL_SYSTICK_IRQHandler( );
}

uint8_t GetBoardPowerSource( void )
{
    return BATTERY_POWER;
	//return USB_POWER;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT
/*
 * Function Name  : assert_failed
 * Description    : Reports the name of the source file and the source line number
 *                  where the assert_param error has occurred.
 * Input          : - file: pointer to the source file name
 *                  - line: assert_param error line source number
 * Output         : None
 * Return         : None
 */
void assert_failed( uint8_t* file, uint32_t line )
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while( 1 )
    {
    }
}
#endif
