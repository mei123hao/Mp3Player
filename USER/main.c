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

//�û�����������
//����ֵ:0,����
//       1,������3
u8 USH_User_App(void)
{ 
	u32 total,free;
	u8 res=0;
	printf("�豸���ӳɹ�...\n");
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
 
	while(HCD_IsDeviceConnected(&USB_OTG_Core))//�豸���ӳɹ�
	{	
		LED0=!LED0;
		OLED_ShowStr(0,0,"Device Connected",2);
		mp3_play();
	}   
	printf("�豸������...\n");
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�
	OLED_Init();

	OLED_Fill(0xFF);//ȫ������
	delay_ms(1000);
	OLED_Fill(0x00);//ȫ����
	delay_ms(1000);
	
	
	my_mem_init(SRAMIN);	//��ʼ���ڲ��ڴ��	
 	exfuns_init();			//Ϊfatfs��ر��������ڴ�
//	TIM3_Int_Init(500-1,8400-1);
  VS_Init();	  				//��ʼ��VS1053	
	printf("Ram Test:0X%04X\r\n",VS_Ram_Test());//��ӡRAM���Խ��
//	printf("���Ҳ�����...\n");
  VS_Sine_Test();	
//  printf("���Ҳ����Գɹ���\n");
  f_mount(fs[2],"2:",1); 	//����U��
	KEY_Init();				//����

	//��ʼ��USB����
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










