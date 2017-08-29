/*				     _
          _         | |
        _| |_ _____ | | _____  _____      ____
	   (_   _) ___ || |( ___ ||     | () / ___)
		 | |_| ____ | || ____ | | | || |( (___
     	  \__)_____)|_ )_____)|_|_|_||_| \____)
    (C)2016 telemic


*/
#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32l0xx.h"
#include "stm32l0xx_hal.h"
#include "utilities.h"
#include "timer.h"
#include "delay.h"
#include "gpio-board.h"
#include "spi-board.h"
#include "radio.h"
#include "sx1272.h"
#include "adc-board.h"
#include "rtc-board.h"
#include "sx1272-board.h"

/*!
 * Generic definition
 */
#ifndef SUCCESS
#define SUCCESS                                     1
#endif

#ifndef FAIL
#define FAIL                                        0
#endif

/*!
 * Enables the choice between Led1 and Potentiometer.
 * LED1 and Potentiometer are exclusive.
 * \remark When using Potentimeter don't forget  that the connection between
 *         ADC input pin of iM880A and the Demoboard Poti requires a connection
 *         between X5:11 - X5:18.
 *         Remove the original jumpers for that. 
 *         On SK-iM880A X5 is the 20 pin header close to the DIP SW and Buttons
 */


/*!
 * Board MCU pins definitions
 */

#define RADIO_RESET                                 PA_15

#define RADIO_MOSI                                  PA_7
#define RADIO_MISO                                  PA_6
#define RADIO_SCLK                                  PA_5
#define RADIO_NSS                                   PA_4

#define RADIO_DIO_0                                 PA_0 //Interrupt from the modem
//#define RADIO_DIO_1                                 PB_10
//#define RADIO_DIO_2                                 PB_11
//#define RADIO_DIO_3                                 PB_7
//#define RADIO_DIO_4                                 PB_5
#define RADIO_DIO_5                                 PA_1 // Clock source from the modem
#define RED_LED                                     PA_2
#define Water_HL                                    PB_3

//#define RADIO_ANT_SWITCH_RX                         PC_13
//#define RADIO_ANT_SWITCH_TX                         PA_4

//#define OSC_LSE_IN                                  PC_14
//#define OSC_LSE_OUT                                 PC_15

//#define OSC_HSE_IN                                  PH_0
//#define OSC_HSE_OUT                                 PH_1


//#define UART_TX                                     PA_9
//#define UART_RX                                     PA_10

//#if ( USE_POTENTIOMETER == 1 )
//#define POTI                                        PA_3
//#else
//#define LED_1                                       PA_3
//#endif
//#define LED_2                                       PA_0
//#define LED_3                                       PA_1
//#define LED_4                                       PA_8

/*!
 * LED GPIO pins objects
 */
//#if ( USE_POTENTIOMETER == 0 )
//extern Gpio_t Led1;
//#endif
//extern Gpio_t Led2;
//extern Gpio_t Led3;
//extern Gpio_t Led4;

/*!
 * MCU objects
 */
extern Adc_t Adc;
extern COMP_HandleTypeDef hcomp;
enum BoardPowerSource
{
    USB_POWER = 0,
    BATTERY_POWER
};

/*!
 * \brief Initializes the target board peripherals.
 */
void BoardInitMcu( void );

/*!
 * \brief Initializes the boards peripherals.
 */
void BoardInitPeriph( void );

/*!
 * \brief De-initializes the target board peripherals to decrease power
 *        consumption.
 */
void BoardDeInitMcu( void );

/*!
 * \brief Measure the Potentiometer level
 *
 * \retval value  Potentiometer level ( value in percent )
 */

void TIM21_interruptHandle(void);

/*!
 * \brief Measure the VDD voltage
 *
 * \retval value  VDD voltage in milivolts
 */
uint16_t BoardMeasureVdd( void );

/*!
 * \brief Get the current battery level
 *
 * \retval value  battery level ( 0: very low, 254: fully charged )
 */
uint8_t BoardGetBatteryLevel( void );

/*!
 * Returns a pseudo random seed generated using the MCU Unique ID
 *
 * \retval seed Generated pseudo random seed
 */
uint32_t BoardGetRandomSeed( void );

/*!
 * \brief Gets the board 64 bits unique ID
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId( uint8_t *id );

/*!
 * \brief Get the board power source
 *
 * \retval value  power source ( 0: USB_POWER,  1: BATTERY_POWER )
 */
uint8_t GetBoardPowerSource( void );

/*
 * Comparator for measuring the water level with an internal Vref
 */
void Error_Handler(void);


void COMP_Init(void);
/*
 * Function to measure the water level using the comparator COMP
 */
uint32_t Water_Level(void);


#endif // __BOARD_H__
