#include "i2chw/i2cmaster.h"

#include "sj2258.h"

uint8_t change_volume(uint8_t volume)
{
	if ( volume < SJ2258_VOLUME_MIN ) {
		return 0;
	}

	if ( volume > SJ2258_VOLUME_MAX ) {
		return 0;
	}

	//example volume=31 - atten=41
	uint8_t atten = 72 - volume; //ослабление
		//example 41/10=4
	uint8_t ten = atten/10;
		//example 0000 0100 & 0000 0111 = 0000 0100 | 1101 0000 = 1101 0100 = D4
	uint8_t tenb = (ten&0x07)|0xD0;
		//example 41-(10*4) = 1
	uint8_t unit = atten-(10*ten);
		//example 0000 0001 & 0000 1111 = 0000 0001 | 1110 0000 = 1110 0001 = E1
	uint8_t unitb = (unit&0x0F)|0xE0;

	uint8_t ret = i2c_start(SJ2258_ADDR);

	if ( ret == 0 )
	{
		ret = i2c_write(tenb);
		ret = i2c_write(unitb);
		i2c_stop();
		return 1;
	}

	i2c_stop();
	return 0;
}
