#include "fsl_device_registers.h"
void software_delay(unsigned long delay)
{
	while (delay > 0) delay--;
}

unsigned long CNT_DIR = 0x00;
unsigned long MODE_SW = 0x00;
unsigned long counter = 0x00;
unsigned long voltagevalue =0x00;
unsigned long adcdata = 0x00;
unsigned long numArrayPortD[10] = {0xFE, 0xB0, 0xED, 0xF9, 0xB3, 0xDB, 0xDF, 0xF0, 0xFF, 0xFB};
unsigned long numArrayPortC[10] = {0xBE, 0x30, 0xAD, 0xB9, 0x33, 0x9B, 0x9F, 0xB0, 0xBF, 0xBB};
unsigned long upperADC = 0x00;
unsigned long lowerADC = 0x00;
unsigned long ADCvalueD = 0x00;
unsigned long ADCvalueC = 0x00;
unsigned long upperCNT = 0x00;
unsigned long lowerCNT = 0x00;
unsigned long CNTvalueD = 0x00;
unsigned long CNTvalueC = 0x00;


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

	unsigned long Delay = 0x100000; /*Delay Value*/

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
	adcdata = ADC_read16b();//read value from AD convert
	MODE_SW = GPIOB_PDIR & 0x04;//check Mode_sw
	CNT_DIR = GPIOB_PDIR & 0x08;//check CNT_DIP
	if(MODE_SW == 0)
	{
		voltagevalue = (adcdata*33)/65535;//conversion
		upperADC = voltagevalue/10;//calculate the first digital for port D
		lowerADC = voltagevalue-10*upperADC;//calculate the second digital for Port D
		ADCvalueD = numArrayPortD[upperADC];//find the corresponding number figure to display
		ADCvalueC = numArrayPortC[lowerADC];//find the corresponding number figure to display
		GPIOD_PDOR = ADCvalueD;
		GPIOC_PDOR = ADCvalueC;//output
		software_delay(Delay);
	}
	else
	{
		if(CNT_DIR == 0)
		{
			counter+=1;
			if (counter == 0x64)
			{
				counter = 0;//reset counter if counter = 100;
			}
			upperCNT = counter/10;//same procedure to display
			lowerCNT = counter%10;
			CNTvalueD = numarrayPortD[upperCNT];
			CNTvalueC = numarrayPortC[lowerCNt];
			GPIOD_PDOR = CNTvalueD;
			GPIOC_PDOR = CNTvalueC;
			software_delay(Delay);
		}
	}
	PORTA_ISFR = (1 << 1); /* Clear ISFR for Port A, Pin 1*/
}

