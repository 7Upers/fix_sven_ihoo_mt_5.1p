#define SJ2323_ADDR 0x94 //audio input selector

#define SJ2323_MUTE_ALL 0xFF
#define SJ2323_MUTE_ON_FL 0xF1
#define SJ2323_MUTE_OFF_FL 0xF0
#define SJ2323_MUTE_ON_FR 0xF3
#define SJ2323_MUTE_OFF_FR 0xF2
#define SJ2323_MUTE_ON_CT 0xF5
#define SJ2323_MUTE_OFF_CT 0xF4
#define SJ2323_MUTE_ON_SUB 0xF7
#define SJ2323_MUTE_OFF_SUB 0xF6
#define SJ2323_MUTE_ON_SL 0xF9
#define SJ2323_MUTE_OFF_SL 0xF8
#define SJ2323_MUTE_ON_SR 0xFB
#define SJ2323_MUTE_OFF_SR 0xFA

#define SJ2323_INPUT_MASK 0xC0
#define SJ2323_INPUT_STEREO_GR1 0xCB //L1&R1
#define SJ2323_INPUT_STEREO_GR2 0xCA //L2&R2
#define SJ2323_INPUT_STEREO_GR3 0xC9 //L3&R3
#define SJ2323_INPUT_STEREO_GR4 0xC8 //L4&R4
#define SJ2323_INPUT_6CH 0xC7

#define SJ2323_SURROUND_ON 0xD0
#define SJ2323_SURROUND_OFF 0xD1
#define SJ2323_MIXED_CH_0DB 0x90
#define SJ2323_MIXED_CH_6DB 0x91

//prototipes
uint8_t sj2323_select_input(uint8_t input);
uint8_t sj2323_surround(uint8_t enable);
