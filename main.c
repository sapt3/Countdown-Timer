/*
 * Countdown Timer.c
 *
 * Created: 5/26/2016 9:32:00 PM
 * Author : Saptarshi Ghosh
 
 //Need to download "myutils.h" for compilation
 */
/*Design note 1:
Lower clock frequency is equivalent to lower power consumption
which isn't really an issue here.
*/

/*
Design Note 2:
This is not a reconfigurable timer clock.
The countdown time is set only and exclusively by the program code
and under no circumstance can be changed later externally other than
completely reprogramming it
See Timer Reconfiguration module -
*/

#define F_CPU 16000000UL // Define the clock speed as 8 MHz

//Include the required libraries
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <Custom/lcd.h> // Include all the necessary functions to initialize  the LCD
//LCD Library by eXtremeelectronics 


/*       ***************Timer reconfiguration module***************
Simply comment out the selection of time you want
*/

 //defining the data types as volatile because of the fact that time cannot be negative

//volatile unsigned int minutes = 30;
//volatile unsigned int minutes = 15;
//volatile unsigned int minutes = 45;
//volatile unsigned int minutes = 60;
volatile unsigned int minutes = 10;
//volatile unsigned int minutes = 5;

volatile unsigned int milliseconds = 0; //Don't change
volatile unsigned int seconds = 0; //Don't change
 //Beginning of main code
int main(void)
{
	/************************************************************************/
	/* PORT initialization block                                                                     */
	/************************************************************************/
	//Start of main
    DDRB |= (1<<PB1); // Define the relay toggle port as output
	DDRC |= (1<<PC3); //Initialize LED
	PORTB &= ~(1<<PB1); //Turn of the load
	PORTC &= ~(1<<PC3); //Turn of the LED Indicator
	
   /************************************************************************/
   /* LCD Initialization Block                                                                     */
   /************************************************************************/
   	LCDInit(LS_BLINK | LS_ULINE); // Initialize the LCD
	LCDClear();
	
	/************************************************************************/
	/* Timer initialization block                                                                     */
	/************************************************************************/	
	TCCR1B |= (1<<WGM12); //Set the timer to CTC mode The timer resets on overflow
	TCCR1B |= (1<<CS10) |(1<<CS11); //Set the prescaler to 64x
	OCR1A = 250; //After applying pre-scaler the clock cycles will be 12500, so for 1 ms OCR1A - 125
	
	/************************************************************************/
	/* Interrupts initialization block                                                                     */
	/************************************************************************/
	sei(); //Set up global interrupts
	TIMSK |= (1<<OCIE1A);//Set the required registers for interrupt driven code
	//TIFR |= (1<<OCF1A); *Not needed*
	
	/************************************************************************/
	/* LCD UI Block     Delay tiem has been set ats 400 ms       
	While changing the time of the countdown the below code also has to be modified                                                   */
	/************************************************************************/
	LCDWriteStringXY(2,0, "Timer set :");
	LCDWriteStringXY(2,1, "10 Minutes");//Display data to the LCD
	_delay_ms(400); //Wait for sometime and then begin countdown
	LCDClear();
	LCDWriteStringXY(0,0,"****************");
	LCDWriteStringXY(0,1,"Starting Timer:");
	PORTB |= (1<<PB1); //Turn on load
	PORTC |= (1<<PC3); //Turn on LED Indicator
	_delay_ms(400);
	LCDClear();
	LCDWriteStringXY(0,0, "Time remaining:");
	LCDWriteStringXY(8,1,":");
	
    while (1) 
    {   //Continuous update of the remaining time
		
		//Time update code
		LCDWriteIntXY(6,1,minutes,2);
		LCDWriteStringXY(8,1,":");
		LCDWriteIntXY(9,1,seconds,2);
		
		if((minutes ==  0) && (seconds ==0)) //When the desired time is reached
		{
			LCDClear();
			LCDWriteStringXY(0,0, "****Time over***");
			PORTB &= ~(1<<PB1); //Turn off the load
			PORTC &= ~(1<<PC3); //Turn off the indicator LED
			break;
		}
		
			
				
    }
}

//Interrupt Service Routine
ISR(TIMER1_COMPA_vect)
{
	//When the period of 1 millisecond is over execute the following code
	milliseconds++; //Increment the count of milliseconds
	if(milliseconds == 1000)
	{ //When the period of 1 second is over execute the following code
		if(seconds == 0)
		{
			seconds = 59;
			if (minutes>0)
			{
				minutes--;
			} //End of if loop
			
		}//End of inner if loop
		else seconds--;
		
		milliseconds = 0; // Reset the milliseconds counter to 0
		
	}//End of outer if loop
	
	
}//End of ISR 

