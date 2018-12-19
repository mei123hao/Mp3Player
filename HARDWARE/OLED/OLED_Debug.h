#ifndef __QZM_OLED_DEBUG_H__
#define __QZM_OLED_DEBUG_H__

#include "stm32f10x.h"
#include "ZY_key.h"
#include "OLED_I2C.h"

#define   location               85//箭头的位置
#define   NumLocation            30//数字的位置
//*********************************************************************************************************************

extern u8 mode;
extern u8 rec_mode;
void interface(void);
void interface_2(void);

#endif //__QZM_OLED_DEBUG_H__
