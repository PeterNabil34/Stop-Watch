/*
 * 	Mini_Project_2.c
 *
 *  Created on: September 15, 2023
 *  Author: Peter Nabil
 */

#include<avr/io.h>
#include<avr/interrupt.h>

/*------------------static functions because these functions can be used only in this file------------------*/
static void Timer1_Init(void);
static void INT0_Init(void);
static void INT1_Init(void);
static void INT2_Init(void);
static void IO_Init(void);
static void Stop_Watch_Display(void);

/*------------------static variables because these variables can be used only in this file------------------*/
static unsigned char seconds0 = 0;
static unsigned char seconds1 = 0;
static unsigned char minutes0 = 0;
static unsigned char minutes1 = 0;
static unsigned char hours0 = 0;
static unsigned char hours1 = 0;

int main(void)
{
	IO_Init();
	Timer1_Init();
	INT0_Init();
	INT1_Init();
	INT2_Init();

	while(1)
	{
		Stop_Watch_Display();
	}
	return 0;
}

ISR(TIMER1_COMPA_vect)
{
	/*-------------Increment 1 SECOND-------------*/
	seconds0++;

	if(seconds0 == 10)
	{
		seconds1++;
		seconds0 = 0;
	}
	if(seconds1 == 6)
	{
		minutes0++;
		seconds1 = 0;
	}
	if(minutes0 == 10)
	{
		minutes1++;
		minutes0 = 0;
	}
	if(minutes1 == 6)
	{
		hours0++;
		minutes1 = 0;
	}
	if(hours0 == 10)
	{
		hours1++;
		hours0 = 0;
	}
}

ISR(INT0_vect)
{
	/*----------Reset The Timer----------*/
	seconds0 = 0;
	seconds1 = 0;

	minutes0 = 0;
	minutes1 = 0;

	hours0 = 0;
	hours1 = 0;
}

ISR(INT1_vect)
{
	/*---------Stop Timer1 (No Clock)---------*/
	TCCR1B &= ~(1 << CS10);
	TCCR1B &= ~(1 << CS11);
	TCCR1B &= ~(1 << CS12);
}

ISR(INT2_vect)
{
	/*-----------Timer1 Resume (Activate The Clock)-----------*/
	TCCR1B |= (1 << CS12) | (1 << CS10);
}

static void IO_Init(void)
{
	DDRC |= 0x0F;  //First 4 pins in PORTC are OUTPUT to the 7447 Decoder
	PORTC &= 0xF0;  //Send 4 ZEROS to the 7447 Decoder

	DDRA |= 0x3F;  //First 6 pins in PORTA are OUTPUT (Enable pins for the six 7 Segments)
	PORTA |= 0x3F;  //Enable all the six 7 Segments

	DDRD &= ~(1 << PD2); //Button for INT0
	PORTD |= (1 << PD2); //Internal Pull Up

	DDRD &= ~(1 << PD3); //Button for INT1

	DDRB &= ~(1 << PB2); //Button for INT2
	PORTB |= (1 << PB2); //Internal Pull Up
}

static void Timer1_Init(void)
{
	TIMSK |= (1 << OCIE1A); //Enable Module(Timer1 in CTC Mode with OCR1A) Interrupt
	TCCR1A = (1 << FOC1A); //Disconnect OC1A and OC1B, non-PWM mode
	OCR1A = 1000; // ( 1 second / [ 1 / ( 1 MHz / 64 ) ] ) = 15625
	SREG |= (1 << 7);  //Enable Global Interrupt
	TCNT1 = 0;  //Initial Value of Timer1
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);  //CTC on TOP = OCR1A (Mode 4), Assume Prescaler = 1024
}

static void INT0_Init(void)
{
	SREG |= (1 << 7);  //Enable Global Interrupt
	MCUCR |= (1 << ISC01);  //Falling Edge
	GICR |= (1 << INT0);  //Enable Module(External Interrupt 0) Interrupt

}

static void INT1_Init(void)
{
	SREG |= (1 << 7);  //Enable Global Interrupt
	MCUCR |= (1 << ISC11) | (1 << ISC10);  //Rising Edge (This line must be written before the Enable Module Interrupt line (next line) to not generate an interrupt request according to the default settings (low level: ISC10 = 0 & ISC11 = 0 ))
	GICR |= (1 << INT1); //Enable Module(External Interrupt 1) Interrupt
}

static void INT2_Init(void)
{
	SREG |= (1 << 7);  //Enable Global Interrupt
	MCUCSR &= ~(1 << ISC2);  //Falling Edge
	GICR |= (1 << INT2); //Enable Module(External Interrupt 2) Interrupt
}

static void Stop_Watch_Display(void)
{
	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (seconds0 & 0x0F);  //Display the units digit of seconds
	PORTA |= (1 << PA0);  //Enable only the first 7 Segment

	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (seconds1 & 0x0F);  //Display the tenth digit of seconds
	PORTA |= (1 << PA1);  //Enable only the second 7 Segment

	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (minutes0 & 0x0F);  //Display the units digit of minutes
	PORTA |= (1 << PA2);  //Enable only the third 7 Segment

	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (minutes1 & 0x0F);  //Display the tenth digit of minutes
	PORTA |= (1 << PA3);  //Enable only the fourth 7 Segment

	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (hours0 & 0x0F);  //Display the units digit of hours
	PORTA |= (1 << PA4);  //Enable only the fifth 7 Segment

	PORTA &= 0xC0;  //Disable all the six 7 Segments
	PORTC = (PORTC & 0xF0) | (hours1 & 0x0F);  //Display the tenth digit of hours
	PORTA |= (1 << PA5);  //Enable only the sixth 7 Segment
}

