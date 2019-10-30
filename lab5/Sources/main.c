/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_device_registers.h"
void software_delay(unsigned long delay)
{
	while (delay > 0) delay--;
}
int main(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; /*Enable Port B Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/
	PORTB_GPCLR = 0x000C0100; /*Setting the pin 2 and 3 of the Port B as GPIO*/
	PORTD_GPCLR = 0x00FF0100; /*Setting the pin 0-7 of the Port D as GPIO*/
	GPIOB_PDDR = 0x00000000; /*Setting the port B as Input*/
	GPIOD_PDDR = 0x000000FF; /*Setting the port D as Output*/
	GPIOD_PDOR = 0x00000000; /*initialize port D to 0*/
	unsigned long Delay;
	unsigned long Input=0x00;
	unsigned long ROT_DIR=0x00;
	unsigned long ROT_SPD=0x00;
	while (1) {
		Input = GPIOB_PDIR & 0x0C;/*Read and Store pin 2 and 3 of the Port B*/
		ROT_DIR = Input & 0x4;/*Store the pin 2 of the Port B*/
		ROT_SPD = Input & 0x8;/*Store the pin 3 of the Port B*/
		if (ROT_SPD == 0){
			Delay = 20000;
		}
		else  Delay = 160000;
		if (ROT_DIR == 0){//CounterClockWise
			GPIOD_PDOR = 0x36;
			software_delay(Delay);
			GPIOD_PDOR = 0x35;
			software_delay(Delay);
			GPIOD_PDOR = 0x39;
			software_delay(Delay);
			GPIOD_PDOR = 0x3A;
			software_delay(Delay);
		}
		else {//Clockwise
			GPIOD_PDOR = 0x3A;
			software_delay(Delay);
			GPIOD_PDOR = 0x39;
			software_delay(Delay);
			GPIOD_PDOR = 0x35;
			software_delay(Delay);
			GPIOD_PDOR = 0x36;
			software_delay(Delay);
		}
	}
	return 0;
}
