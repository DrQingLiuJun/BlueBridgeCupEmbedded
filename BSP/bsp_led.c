#include "bsp_led.h"

__IO u32 LedShanshuo_Tick;
extern u8 House_Used_1,House_Used_2,House_Used_3,House_Used_4,House_Used_5,House_Used_6,House_Used_7,House_Used_8,House_Used;

void led_init(void)
{
	HAL_GPIO_WritePin(GPIOC,0xff<<8,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void led_ctrl(u8 led_ctrl)
{
	HAL_GPIO_WritePin(GPIOC,0xff<<8,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,led_ctrl<<8,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void LED_Shansho(u8 led_ctrl)
{
	if(uwTick - LedShanshuo_Tick < 500) return;
	LedShanshuo_Tick = uwTick;
	
	led_ctrl ^= House_Used;
	
	HAL_GPIO_WritePin(GPIOC,0xff<<8,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,led_ctrl<<8,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}
