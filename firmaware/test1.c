#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#define MUTEF PD4
#define MUTEC PD5

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

int main(void)
{
	//setup led outputs
	DDRD |= (1<<PWR_LED)|(1<<AUX_LED);
	PORTD |= (1<<PWR_LED)|(1<<AUX_LED); // 0 - enable, 1 - disable

	while (1)
	{
		PORTD ^= _BV(PWR_LED);
		_delay_ms(1000);
	}
	return 0;
}
