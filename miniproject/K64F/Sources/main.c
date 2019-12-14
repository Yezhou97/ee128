/* ###################################################################
 **     Filename    : main.c
 **     Project     : Lab6_Part2
 **     Processor   : MK64FN1M0VLL12
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2019-11-03, 17:50, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @version 01.01
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Pins1.h"
#include "CsIO1.h"
#include "IO1.h"
#include "SM1.h"
#include "WAIT1.h"
#include "MCUC1.h"
#include "IFsh1.h"
#include "IntFlashLdd1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
#include "MK64F12.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
unsigned char write[512];
unsigned char keypad[4][4] = {/*define the keypad*/
		{'1','2','3','A'},
		{'4','5','6','B'},
		{'7','8','9','C'},
		{'*','0','#','D'}
};
void software_delay(unsigned long delay)
{
	while (delay > 0) delay--;
}
unsigned char keypad_scan(void){
	int row, col;
	uint32_t col_status;
	uint32_t row_status;
	//set the col
	GPIOC_PDOR = 0x00000390;// b001110010000 set cols high, rows low
	col_status = GPIOC_PDIR;// read Port data
	if(col_status != 0x0390){// if detect a possible input
		WAIT1_Waitms(10);//delay 10ms to confirm
		col_status = GPIOC_PDIR;// read again
		if (col_status != 0x0390){// determine which key is pressed by scanning
			if (col_status == 0x0380){//b001110000000
				col=0;
			}
			else if (col_status == 0x0310){//b001100010000
				col=1;
			}
			else if (col_status == 0x0290){//b001010010000
				col=2;
			}
			else if (col_status == 0x0190){//b000110010000
				col=3;
			}
			else col=4;// mark as false triggering
		}
		//set the row
		if (col < 4){// only scan row when it is true triggering
			GPIOC_PDOR = 0x0000000F;//set rows high, cols low
			row_status = GPIOC_PDIR;//read data
			if(row_status == 0x000E){//start scanning, b000000001110
				row=0;
			}
			else if(row_status == 0x000D){//b000000001101
				row=1;
			}
			else if(row_status == 0x000B){//b000000001011
				row=2;
			}
			else if(row_status == 0x0007){//b000000001110
				row=3;
			}
			else row=4;//otherwise it is false triggering
		}
		if((row<4)&(col<4)){
			return keypad[row][col];// return the value
		}
		else return 'N';// tell the main program to keep polling with keypad
	}
	else return 'N';// no input
}
int motor_rot_cw(){
	GPIOD_PDOR=0x03;
	for(int i=0;i<10;i++){
		GPIOB_PDOR = 0x408;
		WAIT1_Waitms(100);
		GPIOB_PDOR = 0x404;
		WAIT1_Waitms(100);
		GPIOB_PDOR = 0x804;
		WAIT1_Waitms(100);
		GPIOB_PDOR = 0x808;
		WAIT1_Waitms(100);
	}
	GPIOD_PDOR=0x00;
	return 0;
}
int motor_rot_ccw(){
	GPIOD_PDOR=0x03;
	for(int i=0;i<10;i++){
		GPIOB_PDOR = 0x808;
		WAIT1_Waitms(100);
		GPIOB_PDOR = 0x804;
		WAIT1_Waitms(100);
		GPIOB_PDOR = 0x404;
		WAIT1_Waitms(100);
		GPIOB_PDOR = 0x408;
		WAIT1_Waitms(100);
	}
	GPIOD_PDOR=0x00;
	return 0;
}
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/
	/* Write your code here */
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; /*Enable Port C Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK; /*Enable Port C Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; /*Enable Port D Clock Gate Control*/
	PORTB_GPCLR = 0x0C0C0100; /*Setting the pin 0-4 and 7-9 of the Port C as GPIO*/
	PORTC_GPCLR = 0x039F0100; /*Setting the pin 0-4 and 7-9 of the Port C as GPIO*/
	PORTD_GPCLR = 0x00030100; /*Setting the pin 0-1 and pin 4-7 of the Port D as GPIO*/
	GPIOB_PDDR = 0x00000C0C; /*Setting the port C as OUtput*/
	GPIOC_PDDR = 0x0000039F; /*Setting the port C as OUtput*/
	GPIOD_PDDR = 0x00000003; /*Setting the port D as Output*/
	unsigned char correct_psw[16];
	int address=0xFFC00;/*the address point to area 5 in MCU*/
	for(int i=0;i<16;i++){
		IFsh1_GetByteFlash(address,correct_psw+i);/*read operation*/
		address+=8;/*point to next address*/
	}
	unsigned char input[16]={0};/*input array initialization*/
	unsigned char admin[16]="6666";/*initialize admin password*/
	uint32_t delay;
	uint8_t status=0x0;/*status is used to tell arduino what should be display*/
	int length=0x00;/*index input*/
	int failure=0;/*record failure times*/
	unsigned char typein;/*the value keypad scan return*/
	int len;
	int reset=0;/*flag for reset password*/
	motor_rot_cw();// door should be lokced when initializing

	LDD_TDeviceData *SM1_DeviceData;
	SM1_DeviceData = SM1_Init(NULL);

	for(;;) {
		typein = keypad_scan();/*get the scanning return*/
		if(typein == 'N' ){/*false triggering or nothing*/
			status=0x0;//0x0-keep polling
		}
		else{
			if(typein != '#'){/*keep waiting entering*/
				if (typein == '*'){/*delete*/
					if(length>0){/*only when input is not empty*/
						input[length - 1]='\0';/*delete last one*/
						length = length - 1;/*index-1*/
					}
				}
				else {/**/
					if(length<15){/*limit the length of password*/
						input[length]=typein;/*store the new value*/
						length = length + 1;/*index+1*/
					}
				}
				status=0x0;// and keep polling
			}
			else {/*the entry is over*/
				if(reset==1){/*if it's changing password*/
					strcpy(correct_psw,input);/*copy the input to password array*/
					reset=0;/*clear the flag*/
					memset(input,0,sizeof(input)/sizeof(char));/*empty the input array*/
					length=0;/*set index to 0*/
					address=0xFFC00;/*set address to start position of area 5*/
					for(int i=0;i<16;i++){
						IFsh1_SetByteFlash(address,correct_psw[i]);// write new password to EEPROM
						address+=8;/*next address*/
					}
					status=0x4;/*0x4-password reset*/
				}
				else if(strcmp(admin,input)==0){/*match the admin password*/
					reset=1;/*set the flag*/
					memset(input,0,sizeof(input)/sizeof(char));/*empty the input array*/
					length=0;/*set index to 0*/

				}
				else if(strcmp(correct_psw,input)==0){/*match the current passowrd*/
					failure=0;/*reset failure times*/
					status=0x1;/*0x01-password match*/
					memset(input,0,sizeof(input)/sizeof(char));/*empty the input array*/
					length=0;
					//unlock the door
				}
				else{/*the input doesn't match*/
					failure+=1;
					memset(input,0,sizeof(input)/sizeof(char));/*empty the input array*/
					status=0x2;/*0x02- wrong password*/
					length=0;
					if(failure>=5){/*fail too many times*/
						status=0x5;/*0x5-too many failures*/
						int penalty=30;/*penalty time*/
						for(int i=penalty;i>0;i--){
							printf("%-16d%x%02d\n",i,status,length);
							len = sprintf(write,"%-16d%x%02d\n",i,status,length);
							SM1_SendBlock(SM1_DeviceData, &write, len);/*update the penalty time left*/
							WAIT1_Waitms(1000);/*delay 1s*/
						}
						failure=0;/*reset failure times*/
					}
				}
			}
		}
		if(reset==1){
			status=0x3;/*it is changing the password*/
		}
		printf("%-16s%x%02d\n",input,status,length);
		len = sprintf(write,"%-16s%x%02d\n",input,status,length);
		SM1_SendBlock(SM1_DeviceData, &write, len);/*send the current input+status+length to arduino*/
		software_delay(300000); /*delay*/
		if(status==0x1){
			motor_rot_ccw();// unlock the door
			WAIT1_Waitms(5000);//wait user come in 
			motor_rot_cw();// then lock the door
		}
		if((status==0x2)|(status==0x4)){/*wait 2s for arduino to display information to user*/
			WAIT1_Waitms(2000);/*because arduino's delay function has problem when SPI enabled*/
		}
	}
	/* For example: for(;;) { } */

	/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
	/*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
#ifdef PEX_RTOS_START
	PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
#endif
	/*** End of RTOS startup code.  ***/
	/*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
	for(;;){}
	/*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.4 [05.11]
 **     for the Freescale Kinetis series of microcontrollers.
 **a
 ** ###################################################################
 */
