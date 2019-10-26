#include "fsl_device_registers.h"
void software_delay(unsigned long delay)
{
	while (delay > 0) delay--;
}
unsigned long Delay = 0x100000; /*Delay Value*/
unsigned int t1, t2, t3, pulse_width, period, duty_cycle;
unsigned int nr_overflows = 0;
unsigned int upper, lower;
unsigned int valueD, valueC;
unsigned long numArrayPortD[10] = {0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B};
unsigned long numArrayPortC[10] = {0xBE, 0x30, 0xAD, 0xB9, 0x33, 0x9B, 0x9F, 0xB0, 0xBF, 0xBB};

void FTM3_IRQHandler(void) {
	nr_overflows++;
	uint32_t SC_VAL = FTM3_SC;
	FTM3_SC &= 0x7F; // clear TOF
}

int main(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; /*Enable Port C Clock Gate Control*/
	SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK; // FTM3 clock enable
	PORTC_GPCLR = 0x01BF0100; /*Configure PORTC Pins 0-5 and 7-8 as GPIO*/
	PORTD_GPCLR = 0x00FF0100; /*Configure PORTD Pins 0-7 as GPIO*/
	PORTC_PCR10 = 0x300; // Port C Pin 10 as FTM3_CH6 (ALT3)
	FTM3_MODE = 0x5; // Enable FTM3
	FTM3_MOD = 0xFFFF;
	FTM3_SC = 0x0F;//stem clock / 64
	NVIC_EnableIRQ(FTM3_IRQn); // Enable FTM3 interrupts
	FTM3_SC |= 0x40; // Enable TOF
	GPIOC_PDDR = 0x000001BF; /*Configure PORTC Pins 0-5 and 7-8 as Output*/
	GPIOD_PDDR = 0x000000FF; /*Configure PORTD Pins 0-7 as Output*/

	while(1)
	{
		FTM3_CNT = 0; nr_overflows = 0; // initialize counters
		FTM3_C6SC = 0x8; // falling edge
		while(!(FTM3_C6SC & 0x80)); // wait for CHF
		FTM3_C6SC &= ~(1 << 7);
		t1 = FTM3_C6V; // first edge

		FTM3_C6SC = 0x4; // rising edge
		while(!(FTM3_C6SC & 0x80)); // wait for CHF
		FTM3_C6SC &= ~(1 << 7);
		t2 = FTM3_C6V; // second edge

		FTM3_C6SC = 0x8; // falling edge
		while(!(FTM3_C6SC & 0x80)); // wait for CHF
		FTM3_C6SC &= ~(1 << 7);
		FTM3_C6SC = 0; // stop C6
		t3 = FTM3_C6V; // third edge
		if (t2 >= t1)
			pulse_width = (nr_overflows >> 16) + (t2 - t1);
		else
			pulse_width = ((nr_overflows-1) >> 16) + (t2 - t1);
		period = t3 - t1;
		duty_cycle = 100 - 100 * pulse_width / period;
		upper = duty_cycle / 10;
		lower = duty_cycle % 10;
		valueD = numArrayPortD[upper];
		valueC = numArrayPortC[lower];
		GPIOD_PDOR = valueD;
		GPIOC_PDOR = valueC;
		software_delay(Delay); /*Wait Delay Value*/
	}
	return 0;
}

