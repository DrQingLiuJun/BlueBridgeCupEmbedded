/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stdio.h"
#include "interrupt.h"
#include "bsp_led.h"
#include "string.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//减速函数
__IO u32 lcdTick,ledTick,usartTick,uwSetingTime_Tick,delayTick;

//LED
u8 led_once;

//LCD
u8 lcd_buff[30];
u8 lcd_once = 1;
u8 Seting_Time_Ctrl = 0;
u8 jiemian,Once = 1;
s8 cow;

//KEY
u8 set_flag;

/*RTC*/
u8 Interface_Num;
RTC_TimeTypeDef time;
RTC_DateTypeDef date;

/*USART*/
u8 USATR_CtrlTransmitTimes;
u8 Usart_Once = 1;
u8 usart_state = 1,search_wares[20];
u8 Find_OK;
u8 alarm_setting;

//ADC
uint32_t adc_r37,adc_r38;
uint8_t Set_House = 1;

//货物信息
struct Date
{
	uint8_t WeekDay;
	uint8_t Month;   
	uint8_t Date;  
	uint8_t Year;	
};

struct wares
{
	uint8_t name[20];
	uint8_t type[20];
	uint32_t number;
	struct Date date;
	uint8_t from[20];
	uint8_t to[20];
	uint8_t place;
	uint8_t time[20];
	int temperature;
	int temperature_up;
	int temperature_down;
	int humidity;
	int humidity_up;
	int humidity_down;
}Wares[8];
u8 Ware_Select = 0,abnormal_type = 0;

//库房信息
u8 House_Used_1,House_Used_2,House_Used_3,House_Used_4,House_Used_5,House_Used_6,House_Used_7,House_Used_8,House_Used;
u32 OutBound_Number;
u8 OutBound_Name[20],OutBound_Num;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void LCD_Proc(void);
void KEY_Proc(void);
void USART_Proc(void);
void LED_Proc(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
	
	LCD_Init();
    led_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
    HAL_TIM_Base_Start_IT(&htim6);
	HAL_UART_Receive_IT(&huart1,&rx_dat,1);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    LCD_Proc();
    KEY_Proc();
    USART_Proc();
	LED_Proc();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV32;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void LED_Proc(void)
{
	if(uwTick - ledTick < 500) return;
    ledTick = uwTick;
	
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if(Wares[i].place == j + 1)// 1号：0x01,2号：0x02,3号:0x04,4号0x08
			{
				if(Wares[i].temperature > Wares[i].temperature_up ||  Wares[i].temperature < Wares[i].temperature_down || Wares[i].humidity < Wares[i].humidity_down || Wares[i].humidity > Wares[i].humidity_up)
				{
					if(j + 1 == 1) House_Used ^= 0x01;
					if(j + 1 == 2) House_Used ^= 0x02;
					if(j + 1 == 3) House_Used ^= 0x04;
					if(j + 1 == 4) House_Used ^= 0x08;
					if(j + 1 == 5) House_Used ^= 0x10;
					if(j + 1 == 6) House_Used ^= 0x20;
					if(j + 1 == 7) House_Used ^= 0x40;
					if(j + 1 == 8) House_Used ^= 0x80;
				}
				else
				{
					if(j + 1 == 1) House_Used |= House_Used_1;
					if(j + 1 == 2) House_Used |= House_Used_2;
					if(j + 1 == 3) House_Used |= House_Used_3;
					if(j + 1 == 4) House_Used |= House_Used_4;
					if(j + 1 == 5) House_Used |= House_Used_5;
					if(j + 1 == 6) House_Used |= House_Used_6;
					if(j + 1 == 7) House_Used |= House_Used_7;
					if(j + 1 == 8) House_Used |= House_Used_8;
				}
			}
		}
	}
	led_ctrl(House_Used);
}

void LCD_Proc(void)
{
    if(uwTick - lcdTick < 10) return;
    lcdTick = uwTick;
	
	/*获取日期时间*/
	HAL_RTC_GetTime(&hrtc,&time,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&date,RTC_FORMAT_BIN);
	sprintf((char *)lcd_buff,"20%02d-%02d-%02d  %02d:%02d:%02d",date.Year,date.Month,date.Date,time.Hours,time.Minutes,time.Seconds);//时间
	LCD_DisplayStringLine(Line9,lcd_buff);
	
    /*开机界面*/
	if (jiemian == 0)
	{
		while(Once)
		{
			delayTick = uwTick;
			Once = 0;
		}
		
		LCD_DrawLine(12,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		
		LCD_DisplayStringLine(Line1,(uint8_t *)"---Welcome To Use---");//Welcome to use intelligent logistics warehouse management system
		if(uwTick - delayTick > 500)
		{
			LCD_DisplayStringLine(Line3,(uint8_t *)"Intelligent Logistics");
		}
		if(uwTick - delayTick > 1000)
		{
			LCD_DisplayStringLine(Line5,(uint8_t *)"Warehouse Management");
		}
		if(uwTick - delayTick > 1500)
		{
			LCD_DisplayStringLine(Line7,(uint8_t *)"       System       ");

		}
		if(uwTick - delayTick > 2000)
		{
			jiemian = 1;
			LCD_ClearPrimary();
		}
		
	}
	
	/*菜单界面*/
	else if(jiemian == 1)
	{
		LCD_DisplayStringLine(Line0,(uint8_t *)"        MENU        ");
		LCD_DrawLine(Line1,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		
		if(lcd_once == 1)
        {
            LCD_DisplayStringLine(Line2,str_page1[0]);
            LCD_DisplayStringLine(Line4,str_page1[1]);
            LCD_DisplayStringLine(Line6,str_page1[2]);
			line_now = Line2;
            lcd_once = 0;
        }
		line_step = 48;
		line_beg = Line2;
		line_end = Line6;
		
		Line_change_page1();
	}
	
	/*入库管理界面_01录入货物信息*/
	if(jiemian == 2)
	{
		LCD_DisplayStringLine(Line0,(uint8_t *)"   Inbound (Wares)   ");
		LCD_DrawLine(Line1,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		
		//录入货物信息
		LCD_DisplayStringLine(Line2,(uint8_t *)"Name:");
		LCD_DisplayStringLine(Line3,(uint8_t *)"Type:");
		LCD_DisplayStringLine(Line4,(uint8_t *)"Number:");
		LCD_DisplayStringLine(Line5,(uint8_t *)"Date:");
		LCD_DisplayStringLine(Line6,(uint8_t *)"From:");
		LCD_DisplayStringLine(Line7,(uint8_t *)"To:");
	}
	
	/*入库管理界面_02设定库房参数*/
	if(jiemian == 3)
	{
		LCD_DisplayStringLine(Line0,(uint8_t *)"  Inbound (House)  ");
		LCD_DrawLine(Line1,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		LCD_DisplayStringLine(Line2,(uint8_t *)"Place:");
		LCD_DisplayStringLine(Line4,(uint8_t *)"Temperature:");
		LCD_DisplayStringLine(Line6,(uint8_t *)"Humidity:");
	}
	
	/*出库管理界面*/
	else if(jiemian == 4)
	{
		LCD_DisplayStringLine(Line0,(uint8_t *)"      Outbound      ");
		LCD_DrawLine(Line1,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		
		LCD_DisplayStringLine(Line4,(uint8_t *)"  Enter the Name.");
	}
	
	/*在库管理界面*/
	else if(jiemian == 5)
	{
		LCD_DisplayStringLine(Line0,(uint8_t *)"      Warehouse      ");
		LCD_DrawLine(Line1,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		line_step = 24;
		line_beg = Line2;
		line_end = Line7;

		if(lcd_once == 1)
		{
			line_now = Line2;
			LCD_DisplayStringLine(Line2,str_page5[0]);
			LCD_DisplayStringLine(Line3,str_page5[1]);
			LCD_DisplayStringLine(Line4,str_page5[2]);
			LCD_DisplayStringLine(Line5,str_page5[3]);
			LCD_DisplayStringLine(Line6,str_page5[4]);
			LCD_DisplayStringLine(Line7,str_page5[5]);
			lcd_once = 0;
		}
		Line_change_page5();
	}
	else if(jiemian == 6)
	{
		LCD_DisplayStringLine(Line0,(uint8_t *)"   Inquiry Of Wares   ");
		LCD_DrawLine(Line1,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		
		if(lcd_once == 1)
		{
			LCD_DisplayStringLine(Line4,(uint8_t *)"  Enter the Name.");
			lcd_once = 0;
		}
	}
	
	/*货物盘点界面*/
	else if (jiemian == 7)
	{
		LCD_DisplayStringLine(Line0,(uint8_t *)" Inventory Of Wares  ");
		LCD_DrawLine(Line1,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		
		LCD_DrawRect(40,300,49,50);
		LCD_DrawRect(40,220,49,50);
		LCD_DrawRect(40,140,49,50);
		LCD_DrawRect(40,60,49,50);
		LCD_DrawRect(130,300,49,50);
		LCD_DrawRect(130,220,49,50);
		LCD_DrawRect(130,140,49,50);
		LCD_DrawRect(130,60,49,50);
		
		if(lcd_once == 1)
		{
			LCD_DisplayChar(65,267 - 80 * 0,'1');//1
			LCD_DisplayChar(65,267 - 80 * 1,'2');//2
			LCD_DisplayChar(65,267 - 80 * 2,'3');//3
			LCD_DisplayChar(65,267 - 80 * 3,'4');//4
			LCD_DisplayChar(65 + 90,267 - 80 * 0,'5');//5
			LCD_DisplayChar(65 + 90,267 - 80 * 1,'6');//6
			LCD_DisplayChar(65 + 90,267 - 80 * 2,'7');//7
			LCD_DisplayChar(65 + 90,267 - 80 * 3,'8');//18
			
			LCD_SetBackColor(Green);
			for(int i = 0; i < 8;i++)
			{
				for(int j = 0; j < 8; j++)
				{
					if(Wares[i].place == j + 1)
					{
						if ( 0 <= j && j <=3)//第一行
						{
							LCD_DisplayChar(41,299 - 80 * j,' ');
							LCD_DisplayChar(41,283 - 80 * j,' ');
							LCD_DisplayChar(41,267 - 80 * j,' ');
							LCD_DisplayChar(65,299 - 80 * j,' ');
							LCD_DisplayChar(65,283 - 80 * j,' ');
							LCD_DisplayChar(65,267 - 80 * j,' ');
							sprintf((char *)lcd_buff,"%d",j + 1);
							LCD_DisplayString(65,267 - 80 * j,lcd_buff);
						}
						else if (4 <= j && j <= 7)//第二行
						{
							LCD_DisplayChar(41 + 90,299 - 80 * (j-4),' ');
							LCD_DisplayChar(41 + 90,283 - 80 * (j-4),' ');
							LCD_DisplayChar(41 + 90,267 - 80 * (j-4),' ');
							LCD_DisplayChar(65 + 90,299 - 80 * (j-4),' ');
							LCD_DisplayChar(65 + 90,283 - 80 * (j-4),' ');
							LCD_DisplayChar(65 + 90,267 - 80 * (j-4),' ');
							sprintf((char *)lcd_buff,"%d",j + 1);
							LCD_DisplayString(65 + 90,267 - 80 * (j-4),lcd_buff);
						}
					}
				}
			}
			LCD_SetBackColor(Black);
			lcd_once = 0;
		}
		for(int i = 0; i < 7; i++)
		{
			if(Wares[i].place == 1) LCD_DisplayString(90,310,Wares[i].name);
			if(Wares[i].place == 2) LCD_DisplayString(90,310 - 80*1,Wares[i].name);
			if(Wares[i].place == 3) LCD_DisplayString(90,310 - 80*2,Wares[i].name);
			if(Wares[i].place == 4) LCD_DisplayString(90,310 - 80*3,Wares[i].name);
			if(Wares[i].place == 5) LCD_DisplayString(90 + 90,310,Wares[i].name);
			if(Wares[i].place == 6) LCD_DisplayString(90 + 90,310 - 80*1,Wares[i].name);
			if(Wares[i].place == 7) LCD_DisplayString(90 + 90,310 - 80*2,Wares[i].name);
			if(Wares[i].place == 8) LCD_DisplayString(90 + 90,310 - 80*3,Wares[i].name);
		}
	}
	
	/*告警模拟界面*/
	else if(jiemian == 8)
	{
		LCD_DisplayStringLine(Line0,(uint8_t *)"   Alarm Simulation     ");
		LCD_DrawLine(Line1,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		
		if(lcd_once == 1)
		{
			for(int i = 0; i < 8; i++)
			{
				sprintf((char *)lcd_buff,"%d:    0'C    0%%",i + 1);
				LCD_DisplayStringLine((i + 1)*24,lcd_buff);
			}
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 8 ;j++)
				{
					if(Wares[i].place == j + 1) 
					{
						sprintf((char *)lcd_buff,"%d:  %3d'C   %2d%%",j + 1,Wares[i].temperature,Wares[i].humidity);
						LCD_DisplayStringLine(24*(j + 1),lcd_buff);
					}
				}
			}
			lcd_once = 0;
		}
	}
		
	/*告警设置界面*/
	else if (jiemian == 9)
	{
		LCD_DisplayStringLine(Line0,(uint8_t *)"      Alarm Setting     ");
		LCD_DrawLine(Line1,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		
		if(lcd_once == 1)
		{
			for(int i = 0; i < 8; i++)
			{
				sprintf((char *)lcd_buff,"%d:  0~  0'C  0~ 0%%",i + 1);
				LCD_DisplayStringLine((i + 1)*24,lcd_buff);
			}
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 8 ;j++)
				{
					if(Wares[i].place == j + 1) 
					{
						sprintf((char *)lcd_buff,"%d:%3d~%3d'C %2d~%2d%%",j + 1,Wares[i].temperature_down,Wares[i].temperature_up,Wares[i].humidity_down,Wares[i].humidity_up);
						LCD_DisplayStringLine(24*(j + 1),lcd_buff);
					}
				}
			}
			LCD_DrawLine(25,320 - 16*11 - 1,240 - 24*2,Vertical);
			lcd_once = 0;
		}
		
	}
	
	/*系统日期设置界面*/
	else if (jiemian == 10)
	{
		LCD_DisplayStringLine(Line0,(uint8_t *)"     System Date     ");
		LCD_DrawLine(Line1,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		
		sprintf((char *)lcd_buff,"     20%02d-%02d-%02d",date.Year,date.Month,date.Date);
		
		
		/*日期时间设置*/

		if((uwTick - uwSetingTime_Tick) > 500)
		{
			uwSetingTime_Tick = uwTick;
			Seting_Time_Ctrl ^= 1;
		}
		if(Seting_Time_Ctrl ==  1)
		{
			if(Interface_Num == 0)//设置年
			{
				lcd_buff[7] = ' ';
				lcd_buff[8] = ' ';
			}
			else if(Interface_Num == 1)//设置月
			{
				lcd_buff[10] = ' ';
				lcd_buff[11] = ' ';
			}
			else if(Interface_Num == 2)//设置日
			{
				lcd_buff[13] = ' ';
				lcd_buff[14] = ' ';
			}
		} 
		LCD_DisplayStringLine(Line4,lcd_buff);

	}
	
	/*系统时间设置界面*/
	else if(jiemian == 11)
	{
		LCD_DisplayStringLine(Line0,(uint8_t *)"     System Time     ");
		LCD_DrawLine(Line1,0,320,Horizontal);
		LCD_DrawLine(214,0,320,Horizontal);
		
		sprintf((char *)lcd_buff,"       %02d:%02d:%02d",time.Hours,time.Minutes,time.Seconds);
				/*日期时间设置*/

		if((uwTick - uwSetingTime_Tick) > 500)
		{
			uwSetingTime_Tick = uwTick;
			Seting_Time_Ctrl ^= 1;
		}
		if(Seting_Time_Ctrl ==  1)
		{
			if(Interface_Num == 0)//设置时
			{
				lcd_buff[7] = ' ';
				lcd_buff[8] = ' ';
			}
			else if(Interface_Num == 1)//设置分
			{
				lcd_buff[10] = ' ';
				lcd_buff[11] = ' ';
			}
			else if(Interface_Num == 2)//设置秒
			{
				lcd_buff[13] = ' ';
				lcd_buff[14] = ' ';
			}
		} 
		LCD_DisplayStringLine(Line4,lcd_buff);
	}
}

void KEY_Proc(void)
{
    /*返回上一级界面*/
    if(key[0].single_flag == 1)
    {
        if(jiemian == 2) jiemian = 1;
		if(jiemian == 3) jiemian = 1;
		if(jiemian == 4) jiemian = 1;
		if(jiemian == 5) jiemian = 1;
		if(jiemian == 6) jiemian = 5;
		if(jiemian == 7) jiemian = 5;
		if(jiemian == 8) jiemian = 5;
		if(jiemian == 9) jiemian = 5;
		if(jiemian == 10) jiemian = 5;
		if(jiemian == 11) jiemian = 5;
        lcd_once = 1;
		
		/*清屏*/
		LCD_ClearPrimary();
		
		key[0].single_flag = 0;
    }
 
    /*菜单界面*/
    if(jiemian == 1)
    {
        if(key[1].single_flag == 1)
        {
            line_now -= line_step;
            if(line_now < line_beg)
            {
                line_now = line_end;
            }
            Selcct_Up = 1;
            Selcct_Down = 0;
            
            key[1].single_flag = 0;
        }
		
        if(key[2].single_flag == 1)
        {
            line_now += line_step;
            if(line_now > line_end)
            {
                line_now = line_beg;
            }
            Selcct_Up = 0;
            Selcct_Down = 1;
			
            key[2].single_flag = 0;
        } 
		
		if(key[3].single_flag == 1)
		{
			if(line_now == Line2)//跳转入库管理界面
			{
				jiemian = 2;
				Usart_Once = 1;
				usart_state = 1;
				
			}
			else if(line_now == Line4)//跳转出库管理界面
			{
				jiemian = 4;
				Usart_Once = 1;
			}
			else if(line_now == Line6)//跳转在库管理界面
			{
				jiemian = 5;
			}
			lcd_once = 1;
			LCD_ClearPrimary();
			key[3].single_flag = 0;
		}
	}
	
	/*入库确定按键*/	
	if(jiemian == 3)//确定库房温度
	{
		if(key[3].single_flag == 1)
		{
			printf("    库房温度设定为%d摄氏度,湿度设定为%d%%\r\n",
				Wares[Ware_Select].temperature,Wares[Ware_Select].humidity);
			printf("★★★货物【入库成功】！★★★\r\n");
			printf("_______________________________\r\n");
			Set_House = 0;
			led_once = 1;
			if(++Ware_Select == 9)
			{
				Ware_Select = 0;
				printf("已入库过8批货物！\r\n");
			}
			key[3].single_flag = 0;
		}
	}
	if(jiemian == 5)
	{
		if(key[1].single_flag == 1)
        {
            line_now -= line_step;
            if(line_now < line_beg)
            {
                line_now = line_end;
            }
            Selcct_Up = 1;
            Selcct_Down = 0;
            
            key[1].single_flag = 0;
        }
		
        if(key[2].single_flag == 1)
        {
            line_now += line_step;
            if(line_now > line_end)
            {
                line_now = line_beg;
            }
            Selcct_Up = 0;
            Selcct_Down = 1;
			
            key[2].single_flag = 0;
        } 
		if(key[3].single_flag == 1)
		{
			if(line_now == Line2)
			{
				jiemian = 6;
			}
			if(line_now == Line3)
			{
				jiemian = 7;
			}
			if(line_now == Line4)
			{
				jiemian = 8;
			}
			if(line_now == Line5)
			{
				jiemian = 9;	
			}
			if(line_now == Line6)
			{
				jiemian = 10;
			}
			if(line_now == Line7)
			{
				jiemian = 11;
			}
			lcd_once = 1;
			Usart_Once = 1;
			LCD_ClearPrimary();
			key[3].single_flag = 0;
		}
	}
	if(jiemian == 10)
	{
		if(key[1].single_flag == 1)
        {
			if(Interface_Num == 0)//设置年
			{
				if(++date.Year > 99)
				{
					date.Year = 0;
				}
			}
			if(Interface_Num == 1)//设置月
			{
				if(++date.Month > 12)
				{
					date.Month = 0;
				}
			}
			if(Interface_Num == 2)
			{
				if(++date.Date > 31)
				{
					date.Date = 0;
				}
			}
			HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
            key[1].single_flag = 0;
        }
        if(key[2].single_flag == 1)
        {
           if(Interface_Num == 0)
			{
				if(--date.Year == 0)
				{
					date.Year = 99;
				}
			}
			if(Interface_Num == 1)//设置月
			{
				if(--date.Month == 0)
				{
					date.Month = 12;
				}
			}
			if(Interface_Num == 2)
			{
				if(--date.Date ==0)
				{
					date.Date = 31;
				}
			}
			HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
            key[2].single_flag = 0;
        }
		if(key[3].single_flag == 1)
        {
            if(++Interface_Num > 2)
            {
                Interface_Num = 0;
            }
            key[3].single_flag = 0;
        }
		if(key[3].long_flag == 1)
        {
            jiemian = 5;
			LCD_ClearPrimary();
			if(lcd_once == 1)
			{
				line_now = Line2;
				LCD_DisplayStringLine(Line2,str_page5[0]);
				LCD_DisplayStringLine(Line3,str_page5[1]);
				LCD_DisplayStringLine(Line4,str_page5[2]);
				LCD_DisplayStringLine(Line5,str_page5[3]);
				LCD_DisplayStringLine(Line6,str_page5[4]);
				lcd_once = 0;
			}
            key[3].long_flag = 0;
		}
	}
	/*时间设置*/
	if(jiemian == 11)
	{
		if(key[1].single_flag == 1)
        {
			if(Interface_Num == 0)//设置年
			{
				if(++time.Hours > 24)
				{
					time.Hours = 0;
				}
			}
			if(Interface_Num == 1)//设置月
			{
				if(++time.Minutes > 60)
				{
					date.Month = 0;
				}
			}
			if(Interface_Num == 2)
			{
				if(++time.Seconds > 60)
				{
					date.Date = 0;
				}
			}
			HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
            key[1].single_flag = 0;
        }
        if(key[2].single_flag == 1)
        {
           if(Interface_Num == 0)
			{
				if(--time.Hours == 0)
				{
					time.Hours = 23;
				}
			}
			if(Interface_Num == 1)//设置月
			{
				if(--time.Minutes == 0)
				{
					time.Minutes = 59;
				}
			}
			if(Interface_Num == 2)
			{
				if(--time.Seconds ==0)
				{
					time.Seconds = 59;
				}
			}
			HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
            key[2].single_flag = 0;
        }
       
        if(key[3].single_flag == 1)
        {
            if(++Interface_Num > 2)
            {
                Interface_Num = 0;
            }
            key[3].single_flag = 0;
        }
		if(key[3].long_flag == 1)
        {
            jiemian = 5;
			LCD_ClearPrimary();
			if(lcd_once == 1)
			{
				line_now = Line2;
				LCD_DisplayStringLine(Line2,str_page5[0]);
				LCD_DisplayStringLine(Line3,str_page5[1]);
				LCD_DisplayStringLine(Line4,str_page5[2]);
				LCD_DisplayStringLine(Line5,str_page5[3]);
				LCD_DisplayStringLine(Line6,str_page5[4]);
				lcd_once = 0;
			}
            key[3].long_flag = 0;
        }
		
	}

    /*清除按键标志位*/
	for(int i;i<4;i++)
	{
		key[i].single_flag = 0;
		key[i].long_flag = 0;
	}
}

void USART_Proc(void)
{
    if((uwTick- usartTick) < 200) return;
    usartTick = uwTick;
	
	/*开机界面*/
    if(USATR_CtrlTransmitTimes == 0)
    {
        USATR_CtrlTransmitTimes = 1;
		printf("__________________________________________________\r\n");
        printf("★★★★欢迎使用【智能物流仓储管理系统】！★★★★\r\n");
		printf("__________________________________________________\r\n");
		printf("★★系统已自动登录并完成初始化，祝您使用愉快！★★\r\n");
		printf("__________________________________________________\r\n");
    }
	
	/*入库货物界面*/
	if(jiemian == 2)
	{
		
		if(usart_state == 1)
		{
			if(Usart_Once == 1)
			{
				printf("☆☆|【入库管理模式】|☆☆\r\n");
				printf("____录入货物信息____\r\n");
				printf("请输入【货物名称】：\r\n");
				HAL_UART_Receive_IT(&huart1,&rx_dat,1);
				Usart_Once = 0;
			}
			
			if(rx_data[0] != 0)//接收到数据后的处理
			{
				sprintf((char *)Wares[Ware_Select].name,"%s",(uint8_t *)rx_data);
				sprintf((char *)lcd_buff,"Name:%s          ",(uint8_t *)Wares[Ware_Select].name);
				LCD_DisplayStringLine(Line2,lcd_buff);
				USART_ClearBuff();
				printf("    %s\r\n",Wares[Ware_Select].name);
				
				usart_state = 2;
			}
		}
		
		if(usart_state == 2)
		{
			if(rx_data[0] == 0)
			{
				if(Usart_Once == 0)
				{
					printf("请输入【货物类型】：\r\n");
					HAL_UART_Receive_IT(&huart1,&rx_dat,1);
					Usart_Once = 1;
				}
			}
			if(rx_data[0] != 0)
			{
				sprintf((char *)Wares[Ware_Select].type,"%s",(uint8_t *)rx_data);
				sprintf((char *)lcd_buff,"Type:%s          ",(uint8_t *)Wares[Ware_Select].type);
				LCD_DisplayStringLine(Line3,lcd_buff);	
				USART_ClearBuff();
				printf("    %s\r\n",Wares[Ware_Select].type);
				
				usart_state = 3;
			}
		}
		if(usart_state == 3)
		{
			if(rx_data[0] == 0)
			{
				if(Usart_Once == 1)
				{
					printf("请输入【货物数量】：\r\n");
					HAL_UART_Receive_IT(&huart1,&rx_dat,1);
					Usart_Once = 0;
				}
			}
			if(rx_data[0] != 0)
			{
				Wares[Ware_Select].number = Myatoi(rx_data);
				sprintf((char *)lcd_buff,"Number:%d          ",Wares[Ware_Select].number);
				LCD_DisplayStringLine(Line4,lcd_buff);	
				USART_ClearBuff();
				printf("    %d\r\n",Wares[Ware_Select].number);
				
				usart_state = 4;
			}
		}
		if(usart_state == 4)
		{
			Wares[Ware_Select].date.Year = date.Year; 
			Wares[Ware_Select].date.Month = date.Month; 
			Wares[Ware_Select].date.Date = date.Date;
			
			sprintf((char *)lcd_buff,"Date:20%02d-%02d-%02d",Wares[Ware_Select].date.Year,Wares[Ware_Select].date.Month,Wares[Ware_Select].date.Date);
			LCD_DisplayStringLine(Line5,lcd_buff);
			printf("货物【入库日期】为20%02d年%02d月%02d日\r\n    已自动录入！\r\n",Wares[Ware_Select].date.Year,Wares[Ware_Select].date.Month,Wares[Ware_Select].date.Date);
			usart_state = 5;
		}
		
		if(usart_state == 5)
		{
			if(rx_data[0] == 0)
			{
				if(Usart_Once == 0)
				{
					printf("请输入【货物来源地】：\r\n");
					HAL_UART_Receive_IT(&huart1,&rx_dat,1);
					Usart_Once = 1;
				}
			}
			if(rx_data[0] != 0)
			{
				sprintf((char *)Wares[Ware_Select].from,"%s",(uint8_t *)rx_data);
				sprintf((char *)lcd_buff,"From:%s          ",(uint8_t *)Wares[Ware_Select].from);
				LCD_DisplayStringLine(Line6,lcd_buff);	
				USART_ClearBuff();
				printf("    %s\r\n",Wares[Ware_Select].from);
				
				usart_state = 6;
			}
		}
		if(usart_state == 6)
		{
			if(rx_data[0] == 0)
			{
				if(Usart_Once == 1)
				{
					printf("请输入【货物目的地】：\r\n");
					HAL_UART_Receive_IT(&huart1,&rx_dat,1);
					Usart_Once = 0;
				}
			}
			if(rx_data[0] != 0)
			{
				sprintf((char *)Wares[Ware_Select].to,"%s",(uint8_t *)rx_data);
				sprintf((char *)lcd_buff,"To:%s          ",(uint8_t *)Wares[Ware_Select].to);
				LCD_DisplayStringLine(Line7,lcd_buff);	
				USART_ClearBuff();
				printf("    %s\r\n",Wares[Ware_Select].to);
				HAL_Delay(500);
				usart_state = 7;
			}
		}
		if(usart_state ==7)
		{
			if(rx_data[0] == 0)
			{
				if(Usart_Once == 0)
				{
					jiemian = 3;
					LCD_ClearPrimary();
					
					Usart_Once = 1;
				}
			}
		}
	}
	
	/*库房参数设置界面*/
	if(jiemian == 3)
	{
		if(usart_state ==7)
		{
			if(rx_data[0] == 0)
			{
				if(Usart_Once == 1)
				{
					printf("____设定库房参数____\r\n");
					printf("请输入【库房编号】：\r\n");
					HAL_UART_Receive_IT(&huart1,&rx_dat,1);
					
					Usart_Once = 0;
				}
			}
			if(rx_data[0] != 0)
			{
				if((uint8_t )rx_data[0] - 48 < 0 || (uint8_t )rx_data[0] - 48 > 8)
				{
					printf("输入错误，请输入1~8的整数!\r\n");
				}
				else
				{
					Wares[Ware_Select].place = (uint8_t )rx_data[0] - 48;
					sprintf((char *)lcd_buff,"Place:%d          ",Wares[Ware_Select].place);
					LCD_DisplayStringLine(Line2,lcd_buff);	
					USART_ClearBuff();
					printf("    货物将存放在%d号库房！\r\n",Wares[Ware_Select].place);
					
					for(uint8_t i = 0 ;i < 8; i++)
					{
						if(Wares[i].place != 0)
						{
							if(Wares[i].place == 1) House_Used_1 = 0x01;
							if(Wares[i].place == 2) House_Used_2 = 0x02;
							if(Wares[i].place == 3) House_Used_3 = 0x04;
							if(Wares[i].place == 4) House_Used_4 = 0x08;
							if(Wares[i].place == 5) House_Used_5 = 0x10;
							if(Wares[i].place == 6) House_Used_6 = 0x20;
							if(Wares[i].place == 7) House_Used_7 = 0x40;
							if(Wares[i].place == 8) House_Used_8 = 0x80;
							House_Used = House_Used_1 | House_Used_2 | House_Used_3 | House_Used_4 | House_Used_5 | House_Used_6 | House_Used_7 | House_Used_8;
						usart_state = 8;
						}
					}
				}
			}
		}

		if(usart_state ==8)
		{
			if(Usart_Once == 0)
			{
				printf("请设定【库房温度和湿度】：\r\n");
				Usart_Once = 1;
			}
			if(Set_House == 1)
			{
				HAL_ADC_Start(&hadc1);
				HAL_ADC_Start(&hadc2);
				
				adc_r37 = HAL_ADC_GetValue(&hadc2);
				adc_r38 = HAL_ADC_GetValue(&hadc1);
				Wares[Ware_Select].temperature = adc_r37/4096.0f*60 - 20;
				if(Wares[Ware_Select].temperature < 0)//-10
				{
					Wares[Ware_Select].temperature_down = Wares[Ware_Select].temperature * 1.1;
					Wares[Ware_Select].temperature_up = Wares[Ware_Select].temperature * 0.9;
				}
				else
				{
					Wares[Ware_Select].temperature_down = Wares[Ware_Select].temperature * 0.9;
					Wares[Ware_Select].temperature_up = Wares[Ware_Select].temperature * 1.1;
				}
				
				
				
				Wares[Ware_Select].humidity = adc_r38/4096.0f*100;
				Wares[Ware_Select].humidity_down = Wares[Ware_Select].humidity * 0.9;
				Wares[Ware_Select].humidity_up = Wares[Ware_Select].humidity * 1.1;
				
				sprintf((char *)lcd_buff,"Temperature:%3d'C",Wares[Ware_Select].temperature);
				LCD_DisplayStringLine(Line4,lcd_buff);	
				sprintf((char *)lcd_buff,"Humidity:%3d%%",Wares[Ware_Select].humidity);
				LCD_DisplayStringLine(Line6,lcd_buff);	
				
			}
			if(Set_House == 0)
			{
				jiemian = 1;
				LCD_ClearPrimary();
				lcd_once = 1;
				Set_House = 1;
				usart_state = 1;
			}
		}		
	}
	
	/*出库管理界面*/
	if(jiemian == 4)
	{
		if(rx_data[0] == 0)
		{
			if(Usart_Once == 1)
			{
				for(int i = 0; i < 7; i++)
				{
					if(Wares[i].name[0] != 0)
					{
						printf("当前仓库中有%s，数量%d件。\r\n",
							Wares[i].name,Wares[i].number);
					}
				}
				printf("请选择要出库的【货物名称】:\r\n");
				HAL_UART_Receive_IT(&huart1,&rx_dat,1);
				Usart_Once = 2;	
			}
		}
		if(rx_data[0] != 0)
		{
			if(Usart_Once == 2)
			{
				printf("    %s\r\n",rx_data);
				for(int i = 0; i < 8; i++)
				{
					if(strcmp((char *)rx_data,(char *)Wares[i].name) == 0)
					{
						OutBound_Num = i;
						sprintf((char *)OutBound_Name,"%s",rx_data);
						printf("请输入要出库的【货物数量】：\r\n");
						HAL_UART_Receive_IT(&huart1,&rx_dat,1);
						USART_ClearBuff();
						Usart_Once = 3;
						break;
					}
				}
			}
			if(Usart_Once == 3)
			{
				printf("    %s\r\n",rx_data);
				OutBound_Number = Myatoi(rx_data);
				if(Wares[OutBound_Num].number == OutBound_Number)//刚好出货完
				{
					memset(Wares[OutBound_Num].name,0,sizeof(Wares[OutBound_Num].name));
					memset(Wares[OutBound_Num].type,0,sizeof(Wares[OutBound_Num].type));
					Wares[OutBound_Num].number = 0;
					memset(Wares[OutBound_Num].from,0,sizeof(Wares[OutBound_Num].from));
					memset(Wares[OutBound_Num].to,0,sizeof(Wares[OutBound_Num].to));
					memset(Wares[OutBound_Num].time,0,sizeof(Wares[OutBound_Num].time));
					Wares[OutBound_Num].temperature = 0;
					Wares[OutBound_Num].humidity = 0;
					printf("目标货物已出库完成，%d号库房清空!\r\n",Wares[OutBound_Num].place);
					
					if(Wares[OutBound_Num].place == 1) House_Used_1 = 0;
					if(Wares[OutBound_Num].place == 2) House_Used_2 = 0;
					if(Wares[OutBound_Num].place == 3) House_Used_3 = 0;
					if(Wares[OutBound_Num].place == 4) House_Used_4 = 0;
					if(Wares[OutBound_Num].place == 5) House_Used_5 = 0;
					if(Wares[OutBound_Num].place == 6) House_Used_6 = 0;
					if(Wares[OutBound_Num].place == 7) House_Used_7 = 0;
					if(Wares[OutBound_Num].place == 8) House_Used_8 = 0;
					House_Used = House_Used_1 | House_Used_2 | House_Used_3 | House_Used_4 | House_Used_5 | House_Used_6 | House_Used_7 | House_Used_8;
					led_once = 1;
					Wares[OutBound_Num].place = 0;
					OutBound_Number = 0;
					Usart_Once = 0;
					USART_ClearBuff();
					jiemian = 1;
				}
				else if(Wares[OutBound_Num].number < OutBound_Number)
				{
					printf("库房中该货物没有足够的余量，请重新输入出货数量.\r\n");
					OutBound_Number = 0;
					USART_ClearBuff();
					Usart_Once = 1;
				}
				else if(Wares[OutBound_Num].number > OutBound_Number && OutBound_Number != 0)
				{
					Wares[OutBound_Num].number -= OutBound_Number;
					printf("目标货物已出库%d件！目前剩余%d件.\r\n",OutBound_Number,Wares[OutBound_Num].number);
					OutBound_Number = 0;
					USART_ClearBuff();
					Usart_Once = 0;
				}
			}
		}
	}
	
	/*货物查询界面*/
	if(jiemian == 6)
	{
		if(rx_data[0] == 0)
		{
			if(Usart_Once == 1)
			{
				printf("请输入要查询的【货物名称】：\r\n");
				HAL_UART_Receive_IT(&huart1,&rx_dat,1);
				
				Usart_Once = 0;
			}
		}
		if(rx_data[0] != 0)
		{
			sprintf((char *)search_wares,"%s",rx_data);
			USART_ClearBuff();
			for(int i= 0;i < 8; i++)
			{
				if(strcmp((char *)search_wares,(char *)Wares[i].name) == 0)
				{
					printf("    查询的【货物名称】为%s\r\n",Wares[i].name);
					printf("    查询的【货物类型】为%s\r\n",Wares[i].type);
					printf("    查询的【货物数量】为%d\r\n",Wares[i].number);
					printf("    查询的【入库日期】为20%d-%d-%d\r\n",Wares[i].date.Year,Wares[i].date.Month,Wares[i].date.Date);
					printf("    查询的【货物来源地】为%s\r\n",Wares[i].from);
					printf("    查询的【货物目的地】为%s\r\n",Wares[i].to);
					printf("    查询的【货物存放位置】为%d号库房\r\n",Wares[i].place);
					printf("    库房当前温度为%d摄氏度，湿度为%d%%\r\n",Wares[i].temperature,Wares[i].humidity);
					
					sprintf((char *)lcd_buff,"Name:%s          ",Wares[i].name);
					LCD_DisplayStringLine(Line1,lcd_buff);	
					sprintf((char *)lcd_buff,"Type:%s          ",Wares[i].type);
					LCD_DisplayStringLine(Line2,lcd_buff);	
					sprintf((char *)lcd_buff,"Number:%d          ",Wares[i].number);
					LCD_DisplayStringLine(Line3,lcd_buff);	
					sprintf((char *)lcd_buff,"Date:20%02d-%02d-%02d    ",Wares[i].date.Year,Wares[i].date.Month,Wares[i].date.Date);
					LCD_DisplayStringLine(Line4,lcd_buff);	
					sprintf((char *)lcd_buff,"From:%s     ",Wares[i].from);
					LCD_DisplayStringLine(Line5,lcd_buff);	
					sprintf((char *)lcd_buff,"To:%s    ",Wares[i].to);
					LCD_DisplayStringLine(Line6,lcd_buff);	
					sprintf((char *)lcd_buff,"Place:%d ",Wares[i].place);
					LCD_DisplayStringLine(Line7,lcd_buff);	
					sprintf((char *)lcd_buff,"Temp:%3d'C humi:%d%%    ",Wares[i].temperature,Wares[i].humidity);
					LCD_DisplayStringLine(Line8,lcd_buff);	
					Find_OK = 1;
				}
			}
			if(Find_OK == 0)
			{
				printf("未查找到目标货物。\r\n");
			}
		}
	}
	
	/*告警设置界面*/
	if(jiemian == 9)
	{
		if(rx_data[0] == 0)
		{
			if(Usart_Once == 1)
			{
				printf("请输入要修改阈值的【库房编号】：\r\n");
				HAL_UART_Receive_IT(&huart1,&rx_dat,1);
				
				Usart_Once = 2;
			}
			if(Usart_Once == 3)
			{
				printf("请按格式“温度下限~温度上限:湿度下限~湿度上限”输入要修改阈值的【目标阈值】：\r\n");
				HAL_UART_Receive_IT(&huart1,&rx_dat,1);
				Usart_Once = 4;
			}
			
		}
		if(rx_data[0] != 0)
		{
			if(Usart_Once == 2)
			{
				printf("    %s\r\n",rx_data);
				alarm_setting = Myatoi(rx_data) - 1;
				USART_ClearBuff();
				Usart_Once = 3;
			}
			if(Usart_Once == 4)
			{
				printf("    %s\r\n",rx_data);
				for(int i = 0; i < 8; i++)
				{
					if(Wares[i].place == alarm_setting + 1)
					{
						sscanf((char *)rx_data,"%d~%d:%d~%d",&Wares[i].temperature_down,&Wares[i].temperature_up,&Wares[i].humidity_down,&Wares[i].humidity_up);
					}
				}
				USART_ClearBuff();
				Usart_Once = 0;
				lcd_once = 1;
			}
		}
	}
	
		/*告警模拟界面*/
	if(jiemian == 8)
	{
		if(rx_data[0] == 0)
		{
			if(Usart_Once == 1)
			{
				printf("请输入要模拟的【异常类型】序号：\r\n");
				printf("支持以下四种类型：\r\n");
				printf("1.炎热高温\r\n");
				printf("2.寒冷低温\r\n");
				printf("3.雨水潮湿\r\n");
				printf("4.气候干旱\r\n");
				HAL_UART_Receive_IT(&huart1,&rx_dat,1);
				
				Usart_Once = 2;
			}
		}
		if(rx_data[0] != 0)
		{
			if(Usart_Once == 2)
			{
				printf("    %s\r\n",rx_data);
				abnormal_type = Myatoi(rx_data);
				if(abnormal_type == 1)
				{
					for(int i = 0; i < 8; i++)
					{
						Wares[i].temperature *= 1.2; 
					}
				}
				else if (abnormal_type == 2)
				{
					for(int i = 0; i < 8; i++)
					{
						Wares[i].temperature *= 0.8; 
					}
				}
				else if (abnormal_type == 3)
				{
					for(int i = 0; i < 8; i++)
					{
						Wares[i].humidity *= 1.2; 
					}
				}
				else if (abnormal_type == 4)
				{
					for(int i = 0; i < 8; i++)
					{
						Wares[i].humidity *= 0.8; 
					}
				}
				for(int i = 0; i < 8; i++)
				{
					for(int j = 0; j < 8; j++)
					{
						if(Wares[i].place == j + 1)
						{
							if(Wares[i].temperature < Wares[i].temperature_down) printf("异常【警告】：%d号库房温度过低！\r\n",j + 1);
							if(Wares[i].temperature > Wares[i].temperature_up) printf("异常【警告】：%d号库房温度过高！\r\n",j + 1);
							if(Wares[i].humidity < Wares[i].humidity_down) printf("异常【警告】：%d号库房湿度过低！\r\n",j + 1);
							if(Wares[i].humidity > Wares[i].humidity_up) printf("异常【警告】：%d号库房湿度过高！\r\n",j + 1);
						}
					}
				}
				USART_ClearBuff();
				lcd_once = 1;
				Usart_Once = 0;
			}
		}
	}
}

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,1000);
	return ch;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
