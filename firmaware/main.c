//#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "i2chw/i2cmaster.h"

#include "sj2323.h"

//#ifdef REMOTE
//#include "remote.h"
//#endif

#define ATTENUATOR 0x88

#define MUTEF PD4
#define MUTEC PD1

#define ADC0 PC0
#define BUTTONS ADC0

#define PWR_LED PD6
#define AUX_LED PD7
#define DVD_LED PB0
#define NORMAL_LED PB1
#define PROLOGIC_LED PB4
#define VOLUME_DOWN_LED PB3
#define VOLUME_UP_LED PB2

#define IR PC2
#define STANDBY PC3

#define SYS_LED PB5

//#define BAUD 19200
//#define MYUBRR F_CPU/16/BAUD-1


#define PWRN PORTD&=~_BV(PWR_LED) //on
#define PWRF PORTD|=_BV(PWR_LED) //off
#define PWRS (~PORTD)&_BV(PWR_LED) //state

//amplifier
#define AMPN PORTC|=_BV(STANDBY) //on
#define AMPF PORTC&=~_BV(STANDBY) //off

//input
volatile uint8_t aux = 1;
#define AUXN PORTD&=~_BV(AUX_LED) //on
#define AUXF PORTD|=_BV(AUX_LED) //off
#define DVDN PORTB&=~_BV(DVD_LED) //on
#define DVDF PORTB|=_BV(DVD_LED) //off

//sound effect
volatile uint8_t nor = 1;
#define NORN PORTB&=~_BV(NORMAL_LED) //on
#define NORF PORTB|=_BV(NORMAL_LED) //off
#define PRON PORTB&=~_BV(PROLOGIC_LED) //on
#define PROF PORTB|=_BV(PROLOGIC_LED) //off

//g.volume
uint8_t gvol = 0;
#define VUPN PORTB&=~_BV(VOLUME_UP_LED) //on
#define VUPF PORTB|=_BV(VOLUME_UP_LED) //off
#define VDNN PORTB&=~_BV(VOLUME_DOWN_LED) //on
#define VDNF PORTB|=_BV(VOLUME_DOWN_LED) //off

/*
void uart_init(unsigned int ubrr)
{
	//set speed
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)(ubrr);
	// Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Set frame format: 8data, 1stop bit - default
}

char uart_getchar(void)
{
	// Wait for data to be received
	while (!(UCSR0A & (1<<RXC0)));
	printf("%c",UDR0);
	return UDR0;
}

void uart_putchar(char c)
{
	while (!(UCSR0A & (1<<UDRE0))); // Wait until transmission ready.
	UDR0 = c;
}
*/

void setup_input(uint8_t innum)
{
	uint8_t ret = i2c_start(SJ2323_ADDR);

	if ( ret == 0 )
	{
		//printf("selector ACK\r\n");

		if ( innum == 1 )
		{
			ret = i2c_write(SJ2323_INPUT_STEREO_GR1);
//			if ( ret == 0 ) printf("selector stereo channel1 (AUX) ACK\r\n");
		}
		if ( innum == 5 )
		{
			ret = i2c_write(SJ2323_INPUT_6CH);
//			if ( ret == 0 ) printf("selector 5+1 channel (DVD) ACK\r\n");

		}
	}
//	else
//	{
//		printf("selector connection fail\r\n");
//	}

	i2c_stop();
}

void ch_input(void)
{
	if ( aux )
	{
		//is aux
		aux = 0;
		AUXF;
		DVDN;
		//printf("DVD MODE\r\n");
		setup_input(5);
	}
	else
	{
		//is dvd
		aux = 1;
		DVDF;
		AUXN;
		//printf("AUX MODE\r\n");
		setup_input(1);
	}
}

void ch_eff(void)
{
	if ( nor )
	{
		//is normal
		nor = 0;
		NORF;
		PRON;
		//printf("PRO.LOGIC effect\r\n");
	}
	else
	{
		//is prologic
		nor = 1;
		PROF;
		NORN;
		//printf("NORMAL effect\r\n");
	}
}

void setup_vol(void)
{
	//example gvol=31 - atten=41
	uint8_t atten = 72 - gvol; //ослабление
		//example 41/10=4
	uint8_t ten = atten/10;
		//example 0000 0100 & 0000 0111 = 0000 0100 | 1101 0000 = 1101 0100 = D4
	uint8_t tenb = (ten&0x07)|0xD0;
		//example 41-(10*4) = 1
	uint8_t unit = atten-(10*ten);
		//example 0000 0001 & 0000 1111 = 0000 0001 | 1110 0000 = 1110 0001 = E1
	uint8_t unitb = (unit&0x0F)|0xE0;

	uint8_t ret = i2c_start(ATTENUATOR);

	if ( ret == 0 )
	{
		//printf("attenuator ACK\r\n");
		ret = i2c_write(tenb);
//		if ( ret == 0 ) printf("attenuator volume ten=%d byte=%d ACK\r\n",ten,tenb);
		ret = i2c_write(unitb);
//		if ( ret == 0 ) printf("attenuator volume unit=%d byte=%d ACK\r\n",unit,unitb);
	}
//	else
//	{
//		printf("attenuator connection fail\r\n");
//	}

	i2c_stop();
}

void gvol_inc(void)
{
	if ( gvol < 72 )
	{
		gvol++;
	}
	//printf("G.Volume=%d\r\n",gvol);
	setup_vol();
}

void gvol_dec(void)
{
	if ( gvol > 0 )
	{
		gvol--;
	}
	//printf("G.Volume=%d\r\n",gvol);
	setup_vol();
}


//FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main(void)
{
/*	uart_init(MYUBRR);
	stdin = &uart_io;
	stdout = &uart_io;
*/
	//setup led outputs
	DDRD |= (1<<PWR_LED)|(1<<AUX_LED);
	PORTD |= (1<<PWR_LED)|(1<<AUX_LED); // 0 - enable, 1 - disable
	DDRB |= (1<<DVD_LED)|(1<<NORMAL_LED)|(1<<PROLOGIC_LED)|(1<<VOLUME_DOWN_LED)|(1<<VOLUME_UP_LED);
	PORTB |= (1<<DVD_LED)|(1<<NORMAL_LED)|(1<<PROLOGIC_LED)|(1<<VOLUME_DOWN_LED)|(1<<VOLUME_UP_LED);

	//setup sys_led output
	DDRB |= (1<<SYS_LED);
	PORTB &= ~(1<<SYS_LED);

	//standby
	DDRC |= (1<<STANDBY);
	PORTC &= ~(1<<STANDBY);

	//mute
	DDRD |= (1<<MUTEF)|(1<<MUTEC);
	PORTD &= ~((1<<MUTEF)|(1<<MUTEC));
//	PORTD |= (1<<MUTEF)|(1<<MUTEC);

	//input
	DDRC &= ~((1<<BUTTONS)|(1<<IR));
	PORTC &= ~((1<<BUTTONS)|(1<<IR)); //disable pull up resistors

	// configure ADC
	ADMUX = 0x00;
	ADMUX |= (1<<ADLAR); // move to high bite
	ADMUX |= (1<<REFS0); // reference pin - AVcc with external capacitor at AREF pin
	uint8_t button = 255;

	ADCSRA |= (1 << ADPS0)|(1 << ADPS1)|(1 << ADPS2); // set prscaller 128
	//if F_CPU too big, ADCH will be 0xFF, clock for ADC should be about before 200kHz

	ADCSRA |= (1 << ADEN); // Analog-Digital enable bit

	ADCSRA |= (1 << ADSC); // Discarte first conversion
	while (ADCSRA & (1 << ADSC)); // wait until conversion is done

	sei();
	i2c_init();
	printf("I2C bus inited\r\n");

//	uint8_t pressed = 0;

#ifdef REMOTE
	uint8_t cmd = 0;
	RemoteInit();
#endif


	//main loop
	while (1)
	{
#ifdef REMOTE
		//get remote control data
		cmd = GetRemoteCmd(1);
//		printf("\rir cmd=%d",cmd);
		if ( cmd != 0 )
		{
			PORTB ^= (1<<SYS_LED);
		}
//		cmd = 0;
#endif


		//get button
		ADCSRA |= (1 << ADSC); // start single conversion
		while (ADCSRA & (1 << ADSC)) // wait until conversion is done

		button = ADCH;
		printf("adc=%d\r\n",button);
		if ( button < 250 )
		{
//			if ( pressed == 0 )
//			{
//				pressed = 1;
				//disable repeat action holden button

				//printf("\rpressed button (%d) ",button);

				if ( button < 5 )
				{
					//printf("PWR\r\n");

//					if ( (PORTD&(1<<PWR_LED)) == (1<<PWR_LED) )
					if ( PWRS )
					{
						//is on, need turn off

						//disable inputs
						AUXF;
						DVDF;

						//disable sount effect
						NORF;
						PROF;

						PWRF;
						printf("TURN OFF\r\n");
						//disable power of amplifier
						AMPF;
					}
					else
					{
						//is off, need turn on
						PWRN;
						printf("TURN ON\r\n");
						//enable power of amplifier
						AMPN;

						setup_vol();

						if ( aux )
						{
							//is aux
							AUXN;
							DVDF;
							printf("AUX MODE\r\n");
							setup_input(1);
						}
						else
						{
							//is dvd
							DVDN;
							AUXF;
							printf("DVD MODE\r\n");
							setup_input(5);
						}

						if ( nor )
						{
							//is normal
							NORN;
							PROF;
							printf("NORMAL effect\r\n");
						}
						else
						{
							//is prologic
							PRON;
							NORF;
							printf("PRO.LOGIC effect\r\n");
						}
					}

				}

				if ( PWRS )
				{
					//если включено - отслеживаем нажатия остальных кнопок
					if (( button > 20 )&&( button < 30 ))
					{
						//printf("INPUT\r\n");
						ch_input();
					}

					if (( button > 50 )&&( button < 60 ))
					{
						//printf("SOUND\r\n");
						ch_eff();
					}

					if (( button > 104 )&&( button < 114 ))
					{
						//printf("VOLUME-\r\n");
						VDNN;
						gvol_dec();
						VDNF;
//						pressed = 0;
//						_delay_ms(50);
					}

					if (( button > 135 )&&( button < 145 ))
					{
						//printf("VOLUME+\r\n");
						VUPN;
						gvol_inc();
						VUPF;
//						pressed = 0;
//						_delay_ms(50);
					}
				}
//			}
		_delay_ms(100);
		}
		else
		{
			//unpress all buttons
//			pressed = 0;
		}

	}
/*
	sei();
	i2c_init();
	printf("I2C bus inited\r\n");

	unsigned char ret;
	unsigned short int device = 256;
	unsigned char address = 0;

	while (1)
	{
		while ( address <= 127 )
		{
			//led turn on
			PORTB |= (1<<LED);

			ret = i2c_start(address<<1);

			if ( ret == 0 ) device = address<<1;

			//print value
			printf("0x%02X access: %d\r\n",address<<1,ret);

			i2c_stop();

			if ( ret == 0 ) _delay_ms(1000);

			address++;

			//led turn off
			PORTB &= ~(1<<LED);
//			_delay_ms(100);
		}

		if ( device != 256 ) printf ("founded device address=%02X\r\n",device);
		_delay_ms(10000);

		address = 0;
		device = 256;
	}
	*/

	return 0;
}
