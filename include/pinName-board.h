/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Bleeper STM32L151RD microcontroller pins definition

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __PIN_NAME_MCU_H__
#define __PIN_NAME_MCU_H__

/*!
 * STM32 Pin Names
 */
#define MCU_PINS \
    PA_0 = 0, PA_1, PA_2, PA_3, PA_4, PA_5, PA_6, PA_7, PA_8, PA_9, PA_10, PA_11, PA_12, PA_13, PA_14, PA_15, \
    PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7, PB_8, PB_9, PB_10, PB_11, PB_12, PB_13, PB_14, PB_15,     \
    PC_0, PC_1, PC_2, PC_3, PC_4, PC_5, PC_6, PC_7, PC_8, PC_9, PC_10, PC_11, PC_12, PC_13, PC_14, PC_15
    
#endif // __PIN_NAME_MCU_H__


/*
 * In this file we assign values for pins variables where PA_0 = 0, PA_1 = 1, .., PB_0 = 16, .. ,PC_15 = 47). these
 * values will be used then for shifting purposes in GpioMcuInit function. For example the function will check if
 * (MCU_PINS) & 0xF0 == 0x10 then it will initiate a port B pin and so on.
 */
