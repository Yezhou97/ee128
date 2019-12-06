#include <SPI.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);//pin3-7 used for lcd display
char buff [255];
volatile byte indx;
volatile boolean process;
char sta;
int len;
char psw[16];
char sec[16];
int tim;
char add[3];//for last additional 3 characters*
 
void setup (void) {
   Serial.begin (115200);
   lcd.begin(16, 2);
   pinMode(MISO, OUTPUT); // have to send on master in so it set as output
   SPCR |= _BV(SPE); // turn on SPI in slave mode
   indx = 0; // buffer empty
   process = false;
   SPI.attachInterrupt(); // turn on interrupt
   }
   
 
ISR (SPI_STC_vect) // SPI interrupt routine 
{ 
   byte c = SPDR; // read byte from SPI Data Register
   
   if (indx < sizeof(buff)) {
      buff[indx++] = c; // save data in the next index in the array buff
      if (c == '\n') { 
        buff[indx - 1] = 0; // replace newline ('\n') with end of string (0)
        process = true;
      }
   }   
}
 
void loop (void) {
   if (process) {
      lcd.clear();//clear the lcd otherwise arduino will not automatically clear screen
      memset(sec,'\0',len);//empty the second line
      process = false; //reset the process
      strncpy(psw,buff,16);//first 16 characters is password
      strncpy(add,buff+17,2);//last 2 characters is length
      len=atoi(add);// convert to dec int
      sta=buff[16]-'0';// convert to int
      memset(sec,'*',len);//generate corresponding *
        if (sta==0x00|sta==0x03){// then display the Message TEXT
          lcd.setCursor(0,0);
          if(sta==0x00){
          lcd.print("Enter password:");
          }
          else{
            lcd.print("New password");
          }
          lcd.setCursor(0,1);
          lcd.print(sec);
        }
        else if (sta==0x01){
          lcd.setCursor(0,0);
          lcd.print("correct password");
          lcd.setCursor(0,1);
          lcd.print("please come in");
        }
        else if (sta==0x02){
          lcd.setCursor(0,0);
          lcd.print("wrong password");
          lcd.setCursor(0,1);
          lcd.print("please re-enter");
        }
        else if(sta==0x04){
          lcd.print("password reset:");
        }
        else if(sta=0x05){
          tim=atoi(psw);
          lcd.setCursor(0,0);
          lcd.print("please retry in");
          lcd.setCursor(0,1);
          lcd.print(tim);
          lcd.setCursor(8,1);
          lcd.print("Seconds");
        }
      indx= 0; //reset button to zero
   }
}
