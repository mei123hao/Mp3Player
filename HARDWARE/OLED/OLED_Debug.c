/**********************************************
 *�����������ش�ѧ������ͬѧ����С�ӵ�OLED���Ĳ���
*********************************************/

#include "QZM_OLED_Debug.h"
#include "bsp_spi_flash.h"
#include "ZY_led.h"

u8 mode=0;
u8 rec_mode=0;
/***********************************************************************************************************************************
��  ������OLED����ʾ��ǰ������λ��
��  ����uint8_t* line������������
����ֵ��none
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
��  �����������ӣ�����������
��  ����uint8_t line����ǰ����
        uint8_t tota��������
����ֵ��none
***********************************************************************************************************************************/
static void LineAdd(uint8_t* line, uint8_t total)
{
    (*line)++;
    if(*line == total+1)//���һ���ٰ��¡��¡�
        *line = 1;//������һ��
    ShowLocation(*line);
}

/***********************************************************************************************************************************
��  ������������������������
��  ����uint8_t line����ǰ����
        uint8_t tota��������
����ֵ��none
***********************************************************************************************************************************/
static void LineMinus(uint8_t* line, uint8_t total)
{
    (*line)--;
    if(*line == 0)//��һ���ٰ��¡��ϡ�
        *line = total;//�������һ��
    ShowLocation(*line);
}

/***********************************************************************************************************************************
��  ��������OLED�����ı��Ӧ�Ĳ������ӣ�����������
��  ����uint8_t line������
        uint16_t* num��ʵ����Ϊ�����׵�ַ
����ֵ��none
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
��  ��������OLED�����ı��Ӧ�Ĳ�������������������
��  ����uint8_t line������
        uint16_t* num��ʵ����Ϊ�����׵�ַ
����ֵ��none
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
��  ��: ��ʾ��ʼֵ
��  ����uint16_t* num���˽����������
        uint8_t NUM���˽���Ĳ�������
����ֵ��none
***********************************************************************************************************************************/
static void ShowInitialValue(float* num, uint8_t NUM)
{
    for(uint8_t i = 0; i <= NUM - 1; i++)
    {
        OLED_float(NumLocation, i + 2, *(num + i));
    }
}

/***********************************************************************************************************************************
��  ��: ����İ�������
��  ����uint16_t* num���˽����������
        uint8_t NUM���˽���Ĳ�������
����ֵ��none
***********************************************************************************************************************************/
static void Interface_KEY(float* num, uint8_t NUM)
{
    uint8_t temp = RESET;//�˽�����ɱ�־λ
    uint8_t line = 1;//�ڼ���
    
    ShowLocation(line);
    
    while(temp == RESET)//����ѭ��
    {
        if(KeyScan(KEY_E_GPIO,KEY_E_PIN) == KEY_DOWN)//�������ϵİ���������ʱ������
        {
            temp = SET;//�˽������
            OLED_Fill(0xff);//����
            OLED_Fill(0x00);//����
        }

        if(temp == RESET)//ȷ�������������
        {
            if(KeyScan(KEY_U_GPIO,KEY_U_PIN) == KEY_DOWN)//��
            {
                 LineMinus(&line, NUM);
            }

            if(KeyScan(KEY_D_GPIO,KEY_D_PIN) == KEY_DOWN)//��
            {
                 LineAdd(&line, NUM);
            }

            if(KeyScan(KEY_L_GPIO,KEY_L_PIN) == KEY_DOWN)//��
            {
                 NumAdd(line, num);
            }

            if(KeyScan(KEY_R_GPIO,KEY_R_PIN) == KEY_DOWN)//��
            {
                 NumMinus(line, num);
            }
        }
    }
}


/***********************************************************************************************************************************
��  ��: ����ѡ�����
��  ����
����ֵ��none
***********************************************************************************************************************************/
void interface(void)
{
    uint8_t temp = RESET;//�˽�����ɱ�־λ
    uint8_t line = 1;//�ڼ���

    //��ʾ�̶�����Ϣ
	  OLED_ShowStr(0, 1, "GPS MODE CONFIGURE",1);     
    OLED_ShowStr(0, 2, "Debug Mode",1);      //����ģʽ
    OLED_ShowStr(0, 3, "Location Mode",1);   //��λģʽ
	  OLED_ShowStr(0, 4, "Erase Flash",1);     //�����ⲿFLASH

    ShowLocation(line);

    while(temp == RESET)//����ѭ��
    {
        if(KeyScan(KEY_E_GPIO,KEY_E_PIN) == KEY_DOWN)//�������ϵİ���������ʱ������
        {
					  if(mode == 3)   
						{
							SPI_FLASH_Init();
							LED3(ON);
							SPI_FLASH_BulkErase();  //��������ȷ��FLASH����ִֻ��һ��
							LED3(OFF);
							OLED_Fill(0x00);//����
						}
            temp = SET;//�˽������
            OLED_Fill(0x00);//����
        }

        if(KeyScan(KEY_U_GPIO,KEY_U_PIN) == KEY_DOWN)//��
        {
             LineMinus(&line, 3);
        }

        if(KeyScan(KEY_D_GPIO,KEY_D_PIN) == KEY_DOWN)//��
        {
             LineAdd(&line, 3);
        }
				mode = line;
    }
}

/***********************************************************************************************************************************
��  ��: ¼�����Խ���
��  ����
����ֵ��none
***********************************************************************************************************************************/
void interface_2(void)
{
	uint8_t temp = RESET;//�˽�����ɱ�־λ
	uint8_t line = 1;//�ڼ���

	//��ʾ�̶�����Ϣ
	OLED_ShowStr(0, 1, "Debug Mode",1);     
	OLED_ShowStr(0, 2, "RECORDERADDR_a",1);      
	OLED_ShowStr(0, 3, "RECORDERADDR_b",1);   
	OLED_ShowStr(0, 4, "RECORDERADDR_c",1);     
	OLED_ShowStr(0, 5, "RECORDERADDR_d",1); 
	OLED_ShowStr(0, 6, "RECORDERADDR_e",1);

	ShowLocation(line);
	
	while(temp == RESET)//����ѭ��
	{
		if(KeyScan(KEY_E_GPIO,KEY_E_PIN) == KEY_DOWN)//�������ϵİ���������ʱ������
		{
				temp = SET;//�˽������
				OLED_Fill(0x00);//����
		}

		if(KeyScan(KEY_U_GPIO,KEY_U_PIN) == KEY_DOWN)//��
		{
				 LineMinus(&line, 5);
		}

		if(KeyScan(KEY_D_GPIO,KEY_D_PIN) == KEY_DOWN)//��
		{
				 LineAdd(&line, 5);
		}
		rec_mode = line;
	}  
}




