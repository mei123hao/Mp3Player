#include "OLED_I2C.h"
#include "delay.h"
#include "codetab.h"
 /**
  * @brief  I2C_Configuration����ʼ��Ӳ��IIC����
  * @param  ��
  * @retval ��
  */
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  //GPIOB10,B11��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
	IIC_SCL=1;
	IIC_SDA=1;
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(2);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(2);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ��������
}	  

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(2);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(2);							   					   	
}



//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�

u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA=1;delay_us(1);   
	IIC_SCL=1;delay_us(1);	
  
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 


//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(4);
	IIC_SCL=1;
	delay_us(4);
	IIC_SCL=0;
}


//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(4);
	IIC_SCL=1;
	delay_us(4);
	IIC_SCL=0;
}	


//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			
u8 test=0;
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	  SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	 
    for(t=0;t<8;t++)
    {             
			if((txd&0x80)>>7)
        IIC_SDA=1;
			else 
				IIC_SDA=0;
			
        txd<<=1; 	 
			  delay_us(5);
				IIC_SCL=1;
				delay_us(5); 
				IIC_SCL=0;	
				delay_us(5);
    }	 
} 	




//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   

u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	 SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(5);
		    IIC_SCL=1;
        receive<<=1;
		     
        if(READ_SDA)receive++;   
		   delay_us(5); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

/****END OF IIC ALL OPERATION*****/


/*********************OLEDд����************************************/ 
void WriteDat(unsigned char IIC_Data)
{
	IIC_Start();
   IIC_Send_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
   IIC_Send_Byte(0x40);			//write data
	IIC_Wait_Ack();	
   IIC_Send_Byte(IIC_Data);
	IIC_Wait_Ack();
	IIC_Stop();
}
/*********************OLEDд����************************************/
void WriteCmd(unsigned char IIC_Command)
{
	IIC_Start();
   IIC_Send_Byte(0x78);            //Slave address,SA0=0
	while(1)
	{
		if(!IIC_Wait_Ack())
			break;
	}
   IIC_Send_Byte(0x00);			//write command
	IIC_Wait_Ack();	
   IIC_Send_Byte(IIC_Command); 
	IIC_Wait_Ack();
	IIC_Stop();
}



 /**
  * @brief  OLED_Init����ʼ��OLED
  * @param  ��
  * @retval ��
  */
void OLED_Init(void)
{
	IIC_Init();
	delay_ms(500); //�������ʱ����Ҫ
	
	WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //���ȵ��� 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
	OLED_Fill(0x00); //��ʼ����
	OLED_SetPos(0,0);
}


 /**
  * @brief  OLED_SetPos�����ù��
  * @param  x,���xλ��
	*					y�����yλ��
  * @retval ��
  */
void OLED_SetPos(unsigned char x, unsigned char y) //������ʼ������
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

 /**
  * @brief  OLED_Fill�����������Ļ
  * @param  fill_Data:Ҫ��������
	* @retval ��
  */
void OLED_Fill(unsigned char fill_Data)//ȫ�����
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x01);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				WriteDat(fill_Data);
			}
	}
}

 /**
  * @brief  OLED_CLS������
  * @param  ��
	* @retval ��
  */
void OLED_CLS(void)//����
{
	OLED_Fill(0x00);
}


 /**
  * @brief  OLED_ON����OLED�������л���
  * @param  ��
	* @retval ��
  */
void OLED_ON(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X14);  //������ɱ�
	WriteCmd(0XAF);  //OLED����
}


 /**
  * @brief  OLED_OFF����OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
  * @param  ��
	* @retval ��
  */
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X10);  //�رյ�ɱ�
	WriteCmd(0XAE);  //OLED����
}


 /**
  * @brief  OLED_ShowStr����ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
  * @param  x,y : ��ʼ������(x:0~127, y:0~7);
	*					ch[] :- Ҫ��ʾ���ַ���; 
	*					TextSize : �ַ���С(1:6*8 ; 2:8*16)
	* @retval ��
  */
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				
				if(x > 126)  //�Զ�����
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}






 /**
  * @brief  OLED_ShowCN����ʾcodetab.h�еĺ���,16*16����
  * @param  x,y: ��ʼ������(x:0~127, y:0~7); 
	*					N:������codetab.h�е�����
	* @retval ��
  */
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}



 /**
  * @brief  OLED_DrawBMP����ʾBMPλͼ
  * @param  x0,y0 :��ʼ������(x0:0~127, y0:0~7);
	*					x1,y1 : ���Խ���(������)������(x1:1~128,y1:1~8)
	* @retval ��
  */
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}



/*******************************************************************************
��  ����д��һ�����֣���6��8
��  ����L����ֵ
       H����ֵ��ȡֵ��Χ0��7
       num��Ҫ��ʾ�����֣���Χ0-255
����ֵ��none
*******************************************************************************/
void OLED_num8(uint8_t L,uint8_t H,uint8_t num)
{
  uint8_t num8[4]={"000\0"};

  num8[0]=num/100+'0';
  num8[1]=num/10%10+'0';
  num8[2]=num%10+'0';
  OLED_ShowStr(L,H,num8,1);
}

/*******************************************************************************
��  ����д��һ�����֣���6��8
��  ����L����ֵ
       H����ֵ��ȡֵ��Χ0��7
       num��Ҫ��ʾ�����֣���Χ0-255
����ֵ��none
*******************************************************************************/
void OLED_num8_2(uint8_t L,uint8_t H,uint8_t num)
{
  uint8_t num8[3]={"00\0"};

//  num8[0]=num/100+'0';
//  num8[1]=num/10%10+'0';
//  num8[2]=num%10+'0';
	
	num8[0]= num/10%10+'0';
  num8[1]= num%10+'0';
  OLED_ShowStr(L,H,num8,1);
}

/*******************************************************************************
��  ����д��һ�����֣���6��8
��  ����L����ֵ
       H����ֵ��ȡֵ��Χ0��7
       num��Ҫ��ʾ�����֣���Χ0-65535
����ֵ��none
*******************************************************************************/
void OLED_num16(uint8_t L,uint8_t H,uint16_t num)
{
  uint8_t num16[7]={"00000\0"};

  num16[0]=num/10000+'0';
  num16[1]=num/1000%10+'0';
  num16[2]=num/100%10+'0';
  num16[3]=num/10%10+'0';
  num16[4]=num%10+'0';
  OLED_ShowStr(L,H,num16,1);
}

/*******************************************************************************
��  ����д��һ�����֣���6��8
��  ����L����ֵ
       H����ֵ��ȡֵ��Χ0��7
       num��Ҫ��ʾ�����֣���Χ0-4294967295
����ֵ��none
*******************************************************************************/
void OLED_num32(uint8_t L,uint8_t H,uint32_t num)
{
  uint8_t num32[11]={"0000000000\0"};

  num32[0]=num/1000000000+'0';
  num32[1]=num/100000000%10+'0';
  num32[2]=num/10000000%10+'0';
  num32[3]=num/1000000%10+'0';
  num32[4]=num/100000%10+'0';
  num32[5]=num/10000%10+'0';
  num32[6]=num/1000%10+'0';
  num32[7]=num/100%10+'0';
  num32[8]=num/10%10+'0';
  num32[9]=num%10+'0';
  OLED_ShowStr(L,H,num32,1);
}

/*******************************************************************************
��  ����д��һ�����֣���6��8
��  ����L����ֵ
       H����ֵ��ȡֵ��Χ0��7
       num��Ҫ��ʾ�����֣���Χ-3.4E38 - +3.40E38
����ֵ��none
*******************************************************************************/
void OLED_float(uint8_t L,uint8_t H,float num)
{
  uint8_t num_f[9]={"\0"};

  int32_t temp;
  temp=(int32_t)(num*100);
//  if(num>=0)   num_f[0]='+';
//  else        {num_f[0]='-';temp=-temp;}
	num_f[0]=' ';
  num_f[1]=temp/100000%10+'0';
  num_f[2]=temp/10000%10+'0';
  num_f[3]=temp/1000%10+'0';
  num_f[4]=temp/100%10+'0';
	num_f[5]='.';
  num_f[6]=temp/10%10+'0';
  num_f[7]=temp%10+'0';
  OLED_ShowStr(L,H,num_f,1);
}
