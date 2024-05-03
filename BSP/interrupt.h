#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "main.h"
#include "stdbool.h"

struct keys
{
	u8 judge_sta;
	bool key_sta;
	bool single_flag;
	bool long_flag;
	u16 key_time;
};

extern struct keys key[];
extern uint8_t rx_dat,rx_data[30],rx_pointer;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif
