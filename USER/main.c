#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "key.h"  
#include "sram.h"   
#include "malloc.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "usbh_usr.h"
#include "exfuns.h"
#include "vs10xx.h"	
#include "mp3player.h"
#include "timer.h"
#include "OLED_I2C.h"


USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

//用户测试主程序
//返回值:0,正常
//       1,有问题3
u8 USH_User_App(void)
{ 
	u32 total,free;
	u8 res=0;
	printf("设备连接成功...\n");
	res=exf_getfree("2:",&total,&free);
	if(res==0)
	{
		total=total>>10;
		free=free>>10;
		printf("Total is %d \n",total);
		printf("The left is %d \n",free);
		OLED_ShowStr(0,2,"The Total:",1);
		OLED_ShowStr(0,3,"The left:",1);
		OLED_num16(10*6,2,total);
		OLED_num16(10*6,3,free);
		OLED_ShowStr(15*6,2,"MBytes",1);
		OLED_ShowStr(15*6,3,"MBytes",1);
	} 
 
	while(HCD_IsDeviceConnected(&USB_OTG_Core))//设备连接成功
	{	
		LED0=!LED0;
		OLED_ShowStr(0,0,"Device Connected",2);
		mp3_play();
	}   
	printf("设备连接中...\n");
	OLED_ShowStr(0,0,"                ",2);
	OLED_ShowStr(0,0,"No Device!",2);
	OLED_ShowStr(0,2,"                     ",1);
	OLED_ShowStr(0,3,"                     ",1);
	OLED_ShowStr(0,4,"                     ",1);
	OLED_ShowStr(0,5,"                     ",1);
	return res;
} 

int main(void)
{        
	u8 t;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();				//初始化与LED连接的硬件接口
	OLED_Init();

	OLED_Fill(0xFF);//全屏点亮
	delay_ms(1000);
	OLED_Fill(0x00);//全屏灭
	delay_ms(1000);
	
	
	my_mem_init(SRAMIN);	//初始化内部内存池	
 	exfuns_init();			//为fatfs相关变量申请内存
//	TIM3_Int_Init(500-1,8400-1);
  VS_Init();	  				//初始化VS1053	
	printf("Ram Test:0X%04X\r\n",VS_Ram_Test());//打印RAM测试结果
//	printf("正弦波测试...\n");
  VS_Sine_Test();	
//  printf("正弦波测试成功！\n");
  f_mount(fs[2],"2:",1); 	//挂载U盘
	KEY_Init();				//按键

	//初始化USB主机
  USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  
	while(1)
	{
		USBH_Process(&USB_OTG_Core, &USB_Host);
		delay_ms(1);
		t++;
		if(t==200)
		{
			LED1=!LED1;
			t=0;
		}
	}	
}










