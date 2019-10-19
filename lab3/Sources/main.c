#include "fsl_device_registers.h"
void software_delay(unsigned long delay)
{
	while (delay > 0) delay--;
}

unsigned long Delay = 0x100000; /*Delay Value*/
unsigned long CNT_DIR = 0x00000000;
unsigned long MODE_SW = 0x00000000;
unsigned long counter1 = 0x00000000;
unsigned long counter2 = 0x00000000;
unsigned long voltagevalue =0x000000;
unsigned long adcdata = 0x00000000;
unsigned long numArrayPortD[10] = {0xFE, 0xB0, 0xED, 0xF9, 0xB3, 0xDB, 0xDF, 0xF0, 0xFF, 0xFB};
unsigned long numArrayPortC[10] = {0xBE, 0x30, 0xAD, 0xB9, 0x33, 0x9B, 0x9F, 0xB0, 0xBF, 0xBB};
unsigned long upperADC = 0x00000000;
unsigned long lowerADC = 0x00000000;
unsigned long ADCvalueD = 0x00000000;
unsigned long ADCvalueC = 0x00000000;

int main(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; /*Enable Port B Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; /*Enable Port C Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK; /*Enable Port A Clock Gate Control*/
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK; /*Enable ADC Clock Gate Control*/
	ADC0_CFG1 = 0x0C; //16bits ADC; Bus Clock
	ADC0_SC1A = 0x1F; //Disable the module, ADCH = 11111

	PORTA_PCR1 = 0xA0100; /*Configure PORTA Pin 1 as Interrupt on the falling edge*/
	PORTB_PCR2 = 0x0100; /*Configure PORTB Pin 2 as GPIO*/
	PORTB_PCR3 = 0x0100; /*Configure PORTB Pin 3 as GPIO*/
	PORTC_GPCLR = 0x01BF0100; /*Configure PORTC Pins 0-5 and 7-8 as GPIO*/
	PORTD_GPCLR = 0x00FF0100; /*Configure PORTD Pins 0-7 as GPIO*/

	PORTA_ISFR = (1 << 1); /* Clear ISFR for Port A, Pin 1*/
	NVIC_EnableIRQ(PORTA_IRQn); /*Configure PORTA to enable interrupt/unmasked*/
	GPIOB_PDDR = 0x00000000; /*Configure PORTB Pin 2 and 3 as Input*/
	GPIOC_PDDR = 0x000001BF; /*Configure PORTC Pins 0-5 and 7-8 as Output*/
	GPIOD_PDDR = 0x000000FF; /*Configure PORTD Pins 0-7 as Output*/

	while(1)
	{
		software_delay(Delay); /*Wait Delay Value*/
		PORTA_IRQHandler();
	}
	return 0;
}

int ADC_read16b(void)
{
	ADC0_SC1A = 0x00; //Write to SC1A to start conversion from ADC_0
	while(ADC0_SC2 & ADC_SC2_ADACT_MASK); //Conversion in progress
	while(!(ADC0_SC1A & ADC_SC1_COCO_MASK)); //Until conversion complete
	return ADC0_RA;
}

void PORTA_IRQHandler(void)
{
	GPIOD_PDOR ^= (1UL << 7); //Toggle decimal point bit 7
	adcdata = ADC_read16b();
	MODE_SW = GPIOB_PDIR & 0x04;
	CNT_DIR = GPIOB_PDIR & 0x08;

	if(MODE_SW == 0)
	{
		voltagevalue = (adcdata*33)/((2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2)-1);
		upperADC = voltagevalue/10;
		lowerADC = voltagevalue-10*upperADC;

		for(int n = 0; n < 10; n++)
		{
			if(upperADC == n)
			{
				ADCvalueD = numArrayPortD[n];
			}
		}
		for(int n = 0; n < 10; n++)
		{
			if(lowerADC == n)
			{
				ADCvalueC = numArrayPortC[n];
			}
		}
		GPIOD_PDOR = ADCvalueD;
		GPIOC_PDOR = ADCvalueC;
		software_delay(Delay);
	}
	else
	{
		if(CNT_DIR == 0)
		{
			if(counter1 == 9 && counter2 == 9)
			{
				counter1 = 0;
				counter2 = 0;
			}
			else if(counter2 == 9)
			{
				counter1++;
				counter2 = 0;
			}
			else
			{
				counter2++;
			}
			GPIOC_PDOR = numArrayPortC[counter2];
			GPIOD_PDOR = numArrayPortD[counter1];
		}
		else
		{
			if(counter1 == 0 && counter2 == 0)
			{
				counter1 = 9;
				counter2 = 9;
			}
			else if(counter2 == 0)
			{
				counter1--;
				counter2 = 9;
			}
			else
			{
				counter2--;
			}
			GPIOC_PDOR = numArrayPortC[counter2];
			GPIOD_PDOR = numArrayPortD[counter1];
		}
	}
	PORTA_ISFR = (1 << 1); /* Clear ISFR for Port A, Pin 1*/
}

