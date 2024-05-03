#include "interrupt.h"
#include "tim.h"

struct keys key[4];

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//回调函数
{
	if(htim->Instance == TIM6)
	{
		key[0].key_sta = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
		key[1].key_sta = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
		key[2].key_sta = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
		key[3].key_sta = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
		
		for(int i = 0;i < 4 ; i++)
		{
			switch(key[i].judge_sta)
			{
				case 0: //按键按下
				{
					if(key[i].key_sta == 0) 
					{
						key[i].judge_sta = 1;
						key[i].key_time = 0;
					}
				}
				break;
				case 1: //消抖处理
				{
					if(key[i].key_sta == 0)
					{
						key[i].judge_sta = 2;
					}
					else
					{
						key[i].judge_sta = 0;
					}
				}
				break;
				case 2: // 松手检测
				{
					if(key[i].key_sta == 1)
					{
						key[i].judge_sta = 0;
						
						if(key[i].key_time < 70)
						{
							key[i].single_flag = 1; 
							key[i].long_flag = 0;
						}
					}
					else 
					{
						key[i].key_time++;
						if(key[i].key_time > 70)
						{
							key[i].long_flag = 1;
							key[i].single_flag = 0; 
						}
					}
				}
				break;
			}
		}
	}
}

//串口
uint8_t rx_dat,rx_data[30],rx_pointer;
extern UART_HandleTypeDef huart1;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	rx_data[rx_pointer++] = rx_dat;
	HAL_UART_Receive_IT(&huart1,&rx_dat,1);
}
