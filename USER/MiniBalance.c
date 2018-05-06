#include "stm32f10x.h"
#include "sys.h"
#include "control.h"
#include "calculation.h"
#include "Landzo_CCD.h"

u8 Pixel[128];
u8 Image_Data1[128]={0};
u8 Image_Data2[128]={0};
u8 Image_Data3[128]={0};
u8 Black_Left=0,Black_Right=127,Black_Center=IMAGE_DATA/2;

int Moto1,Moto2;                      //电机PWM变量	

extern u8 TIME1flag_1ms;
extern u8 TIME1flag_20ms;

int main(void)
{
    u8 count=0;  
    
    MY_NVIC_PriorityGroupConfig(2);	//=====中断分组
	delay_init();	    	        //=====延时函数初始化	
	uart_init(115200);	            //=====串口初始化为  

	LED_Init();						//=====LED初始化
	
    MiniBalance_PWM_Init(7199,0);   //=====初始化PWM 10KHZ，用于驱动电机 如需初始化电调接口  A8 A11   B12-B15
	Encoder_Init_TIM2();            //=====初始化编码器1    A0 A1
	Encoder_Init_TIM4();            //=====初始化编码器2    B6 B7
	TIM5_Int_Init(49,7199);         //72M/7200=10Khz的计数频率，计数到50为5ms,用于执行机构电机PID的运算
    
    TIM3_Int_Init(999,72);//1ms定时，用于CCD曝光时间计算
    LandzoCCD_init();
    
    //OLED_Init();                    //=====OLED初始化
    
    while(1)
    {     	
		//**********数据采集**********//
		if(TIME1flag_20ms==1)
		{
			TIME1flag_20ms=0;
			if(count==0)
			{			
				ImageCapture(Image_Data1);
				CalculateIntegrationTime(Image_Data1);
			}
			else if(count==1)
			{
				ImageCapture(Image_Data2);
				CalculateIntegrationTime(Image_Data2);
			}
			else if(count==2)
			{
				ImageCapture(Image_Data3);
				CalculateIntegrationTime(Image_Data3);
			}
			count++;
		}
		if(count==3)
		{
            LED0=~LED0;   //LED闪烁表示程序正在运行
			count=0;
			//**********3点平均值滤波**********//
			for(int j=0;j<IMAGE_DATA;j++)
			{
				Pixel[j]=(Image_Data1[j]+Image_Data2[j]+Image_Data3[j])/3;
			}
//			//**********串口发送图像数据**********//
//			SendImageData(Pixel);   //串口发送三次采集滤波后的效果图
			//**********图像数据处理**********//
			Image_Data_Search_Line(IMAGE_DATA,Pixel);
        }
//        SendImageData(Pixel);   //串口发送图像阈值处理过后的图
//	    oled_show();              //===显示屏打开 	  	 
    }	 
}

