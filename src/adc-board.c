/*				     _
          _         | |
        _| |_ _____ | | _____  _____      ____
	   (_   _) ___ || |( ___ ||     | () / ___)
		 | |_| ____ | || ____ | | | || |( (___
     	  \__)_____)|_ )_____)|_|_|_||_| \____)
    (C)2017 telemic


*/
#include "board.h"
#include "adc-board.h"


uint16_t ADCMeasure()
{
  __HAL_RCC_ADC1_CLK_ENABLE();

  ADC1->ISR=0xFFFF;
  ADC1->CR=ADC_CR_ADEN;
  ADC->CCR=(0x0010<<18) | (1<<22);
  ADC1->SMPR=0x100;
  ADC1->CHSELR=(1<<17);

  unsigned int i;

  while(!(ADC1->ISR & ADC_ISR_ADRDY));
  ADC1->ISR = ADC_ISR_ADRDY;

  for(i=0; i<4; i++){
	  ADC1->CR|=ADC_CR_ADSTART;
	  while(!(ADC1->ISR & ADC_ISR_EOC));
	  ADC1->ISR = ADC_ISR_EOC;
  }

  uint16_t retVal = ADC1->DR;

  ADC1->CR=ADC_CR_ADDIS;
  while(ADC1->CR & ADC_CR_ADEN);

  ADC1->CR=0;
  ADC->CCR=0;

  __HAL_RCC_ADC1_CLK_DISABLE();

  return retVal;

}
