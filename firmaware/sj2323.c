#include "i2chw/i2cmaster.h"

#include "sj2323.h"

uint8_t sj2323_select_input(uint8_t input)
{
	if ( (input&SJ2323_INPUT_MASK) == SJ2323_INPUT_MASK ) {
		uint8_t ret = i2c_start(SJ2323_ADDR);

		if ( ret == 0 ) {
			ret = i2c_write(input);
			i2c_stop();

			return 1;
		}
		else {
			//can't sj2323 connect
			return 0;
		}
	}
	else {
		//wrong input chanel
		return 0;
	}
}
