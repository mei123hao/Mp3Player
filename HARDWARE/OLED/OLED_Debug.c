/**********************************************
 *程序来自五邑大学邱忠明同学，那小子的OLED调的不错
*********************************************/

#include "QZM_OLED_Debug.h"
#include "bsp_spi_flash.h"
#include "ZY_led.h"

u8 mode=0;
u8 rec_mode=0;
/***********************************************************************************************************************************
描  述：在OLED上显示当前操作的位置
参  数：uint8_t* line，代表了行数
返回值：none
***********************************************************************************************************************************/
static void ShowLocation(uint8_t line)
{
    switch (line)
    {
        case 1:
        {
            OLED_ShowStr(location,2,"<=",1);OLED_ShowStr(location,3,"  ",1);
            OLED_ShowStr(location,4,"  ",1);OLED_ShowStr(location,5,"  ",1);
            OLED_ShowStr(location,6,"  ",1);OLED_ShowStr(location,7,"  ",1);
            break;
        }
        case 2:
        {
            OLED_ShowStr(location,2,"  ",1);OLED_ShowStr(location,3,"<=",1);
            OLED_ShowStr(location,4,"  ",1);OLED_ShowStr(location,5,"  ",1);
            OLED_ShowStr(location,6,"  ",1);OLED_ShowStr(location,7,"  ",1);
            break;
        }
        case 3:
        {
            OLED_ShowStr(location,2,"  ",1);OLED_ShowStr(location,3,"  ",1);
            OLED_ShowStr(location,4,"<=",1);OLED_ShowStr(location,5,"  ",1);
            OLED_ShowStr(location,6,"  ",1);OLED_ShowStr(location,7,"  ",1);
            break;
        }
        case 4:
        {
            OLED_ShowStr(location,2,"  ",1);OLED_ShowStr(location,3,"  ",1);
            OLED_ShowStr(location,4,"  ",1);OLED_ShowStr(location,5,"<=",1);
            OLED_ShowStr(location,6,"  ",1);OLED_ShowStr(location,7,"  ",1);
            break;
        }
        case 5:
        {
            OLED_ShowStr(location,2,"  ",1);OLED_ShowStr(location,3,"  ",1);
            OLED_ShowStr(location,4,"  ",1);OLED_ShowStr(location,5,"  ",1);
            OLED_ShowStr(location,6,"<=",1);OLED_ShowStr(location,7,"  ",1);
            break;
        }
        case 6:
        {
            OLED_ShowStr(location,2,"  ",1);OLED_ShowStr(location,3,"  ",1);
            OLED_ShowStr(location,4,"  ",1);OLED_ShowStr(location,5,"  ",1);
            OLED_ShowStr(location,6,"  ",1);OLED_ShowStr(location,7,"<=",1);
            break;
        }
    }
}

/***********************************************************************************************************************************
描  述：行数，加，含保护机制
参  数：uint8_t line，当前行数
        uint8_t tota，总行数
返回值：none
***********************************************************************************************************************************/
static void LineAdd(uint8_t* line, uint8_t total)
{
    (*line)++;
    if(*line == total+1)//最后一行再按下“下”
        *line = 1;//调到第一行
    ShowLocation(*line);
}

/***********************************************************************************************************************************
描  述：行数，减，含保护机制
参  数：uint8_t line，当前行数
        uint8_t tota，总行数
返回值：none
***********************************************************************************************************************************/
static void LineMinus(uint8_t* line, uint8_t total)
{
    (*line)--;
    if(*line == 0)//第一行再按下“上”
        *line = total;//调到最后一行
    ShowLocation(*line);
}

/***********************************************************************************************************************************
描  述：根据OLED行数改变对应的参数，加，含保护机制
参  数：uint8_t line，行数
        uint16_t* num，实际上为数组首地址
返回值：none
***********************************************************************************************************************************/
static void NumAdd(uint8_t line, float* num)
{
    (*(num+line-1))+=0.1;
//    if(*(num+line-1) == 0)
//    {
//        *(num+line-1) = 65535;
//    }
    OLED_float(NumLocation, line+1, *(num+line-1));
}

/***********************************************************************************************************************************
描  述：根据OLED行数改变对应的参数，减，含保护机制
参  数：uint8_t line，行数
        uint16_t* num，实际上为数组首地址
返回值：none
***********************************************************************************************************************************/
static void NumMinus(uint8_t line, float* num)
{
    (*(num+line-1))-=0.1;
//    if(*(num+line-1) == 65535)
//    {
//        *(num+line-1) = 0;
//    }
    OLED_float(NumLocation, line+1, *(num+line-1));
}

/***********************************************************************************************************************************
描  述: 显示初始值
参  数：uint16_t* num，此界面操作参数
        uint8_t NUM，此界面的参数个数
返回值：none
***********************************************************************************************************************************/
static void ShowInitialValue(float* num, uint8_t NUM)
{
    for(uint8_t i = 0; i <= NUM - 1; i++)
    {
        OLED_float(NumLocation, i + 2, *(num + i));
    }
}

/***********************************************************************************************************************************
描  述: 界面的按键操作
参  数：uint16_t* num，此界面操作参数
        uint8_t NUM，此界面的参数个数
返回值：none
***********************************************************************************************************************************/
static void Interface_KEY(float* num, uint8_t NUM)
{
    uint8_t temp = RESET;//此界面完成标志位
    uint8_t line = 1;//第几行
    
    ShowLocation(line);
    
    while(temp == RESET)//界面循环
    {
        if(KeyScan(KEY_E_GPIO,KEY_E_PIN) == KEY_DOWN)//检测板子上的按键（带延时消抖）
        {
            temp = SET;//此界面完成
            OLED_Fill(0xff);//满屏
            OLED_Fill(0x00);//清屏
        }

        if(temp == RESET)//确保还在这个界面
        {
            if(KeyScan(KEY_U_GPIO,KEY_U_PIN) == KEY_DOWN)//上
            {
                 LineMinus(&line, NUM);
            }

            if(KeyScan(KEY_D_GPIO,KEY_D_PIN) == KEY_DOWN)//下
            {
                 LineAdd(&line, NUM);
            }

            if(KeyScan(KEY_L_GPIO,KEY_L_PIN) == KEY_DOWN)//左
            {
                 NumAdd(line, num);
            }

            if(KeyScan(KEY_R_GPIO,KEY_R_PIN) == KEY_DOWN)//右
            {
                 NumMinus(line, num);
            }
        }
    }
}


/***********************************************************************************************************************************
描  述: 开机选择界面
参  数：
返回值：none
***********************************************************************************************************************************/
void interface(void)
{
    uint8_t temp = RESET;//此界面完成标志位
    uint8_t line = 1;//第几行

    //显示固定的信息
	  OLED_ShowStr(0, 1, "GPS MODE CONFIGURE",1);     
    OLED_ShowStr(0, 2, "Debug Mode",1);      //调试模式
    OLED_ShowStr(0, 3, "Location Mode",1);   //定位模式
	  OLED_ShowStr(0, 4, "Erase Flash",1);     //擦除外部FLASH

    ShowLocation(line);

    while(temp == RESET)//界面循环
    {
        if(KeyScan(KEY_E_GPIO,KEY_E_PIN) == KEY_DOWN)//检测板子上的按键（带延时消抖）
        {
					  if(mode == 3)   
						{
							SPI_FLASH_Init();
							LED3(ON);
							SPI_FLASH_BulkErase();  //放在这里确保FLASH擦除只执行一次
							LED3(OFF);
							OLED_Fill(0x00);//清屏
						}
            temp = SET;//此界面完成
            OLED_Fill(0x00);//清屏
        }

        if(KeyScan(KEY_U_GPIO,KEY_U_PIN) == KEY_DOWN)//上
        {
             LineMinus(&line, 3);
        }

        if(KeyScan(KEY_D_GPIO,KEY_D_PIN) == KEY_DOWN)//下
        {
             LineAdd(&line, 3);
        }
				mode = line;
    }
}

/***********************************************************************************************************************************
描  述: 录音调试界面
参  数：
返回值：none
***********************************************************************************************************************************/
void interface_2(void)
{
	uint8_t temp = RESET;//此界面完成标志位
	uint8_t line = 1;//第几行

	//显示固定的信息
	OLED_ShowStr(0, 1, "Debug Mode",1);     
	OLED_ShowStr(0, 2, "RECORDERADDR_a",1);      
	OLED_ShowStr(0, 3, "RECORDERADDR_b",1);   
	OLED_ShowStr(0, 4, "RECORDERADDR_c",1);     
	OLED_ShowStr(0, 5, "RECORDERADDR_d",1); 
	OLED_ShowStr(0, 6, "RECORDERADDR_e",1);

	ShowLocation(line);
	
	while(temp == RESET)//界面循环
	{
		if(KeyScan(KEY_E_GPIO,KEY_E_PIN) == KEY_DOWN)//检测板子上的按键（带延时消抖）
		{
				temp = SET;//此界面完成
				OLED_Fill(0x00);//清屏
		}

		if(KeyScan(KEY_U_GPIO,KEY_U_PIN) == KEY_DOWN)//上
		{
				 LineMinus(&line, 5);
		}

		if(KeyScan(KEY_D_GPIO,KEY_D_PIN) == KEY_DOWN)//下
		{
				 LineAdd(&line, 5);
		}
		rec_mode = line;
	}  
}




