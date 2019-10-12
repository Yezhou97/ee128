#include "fsl_device_registers.h"
void software_delay(unsigned long delay)
{
	while (delay > 0) delay--;
}
int main(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; /*Enable Port B Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; /*Enable Port C Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/
	PORTB_GPCLR = 0x000C0100; /*Setting the pin 2 and 3 of the Port B as GPIO*/
	PORTC_GPCLR = 0x01BF0100; /*Setting the pin 0-5 and 7-8 of the Port C as GPIO */
	PORTD_GPCLR = 0x00FF0100; /*Setting the pin 0-7 of the Port C as GPIO*/
	GPIOB_PDDR = 0x00000000; /*Setting the port B as Input*/
	GPIOC_PDDR = 0x000001BF; /*Setting the port C as Output*/
	GPIOD_PDDR = 0x000000FF; /*Setting the port D as Output*/
	GPIOC_PDOR = 0x00000000; /*initialize port C to 0*/
	GPIOD_PDOR = 0x00000001; /*initialize port D to 1 bit is on*/
	unsigned long Delay = 0x100000; /*Delay Value*/
	uint8_t Counter = 0, Shifter = 0x01;
	uint16_t Input = 0, Output = 0, ROT_DIR = 0, CNT_DIR = 0;
	while (1) {
		Input = GPIOB_PDIR & 0x0C;/*Read and Store pin 2 and 3 of the Port B*/
		ROT_DIR = Input & 0x4;/*Store the pin 2 of the Port B*/
		CNT_DIR = Input & 0x8;/*Store the pin 3 of the Port B*/
		uint16_t CntHigher = 0, CntLower = 0;
		if(CNT_DIR) Counter += 1;
		else Counter -= 1;
		if(Counter >= 0x40){/*Check the if pin 6 of Port C is involved in display*/
			CntHigher = Counter & 0xC0;
			CntLower = Counter & 0x3F;/*take the Counter into 2 parts */
			CntHigher <<= 1;/*shift left the higher parts*/
			Output = CntLower | CntHigher;/*combine two parts*/
		}
		else Output = Counter;
		if(ROT_DIR) Shifter = (Shifter << 1) | (Shifter >> 7);/*shift left*/
		else Shifter = (Shifter >> 1)|(Shifter << 7);/*shift right*/
		GPIOC_PDOR = Output;/*output for Counter*/
		GPIOD_PDOR = Shifter;/*output for Rotator*/
		software_delay(Delay);/*Delay*/
	}
	return 0;
}
