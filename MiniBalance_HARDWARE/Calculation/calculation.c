#include "calculation.h"
#include "Landzo_CCD.h"
#include "usart.h"



//extern uint8_t Pixel[128];
//数据二值化
void Image_Data_Two_Valued(u8 len,u8 *data)
{
//	u16 temp=130;
//	u8 i;
	//temp=PixelAverage(len,data)+20;
	for(int i=0;i<len;i++)
	{
		if(*data>THRESHOLD) *data=0xff;
		else *data=0x00;
		data++;
	}
}
//数据滤波
//二值化数据滤波
void Two_Valued_Data_Filter(u8 *data)
{
	u8 i;
//	u8 num;
	data[0]=data[1];
	data[127]=data[126];
	//滤突变
	for(i=1;i<127;i++)
	{
		if(data[i]==0xff&&data[i]!=data[i-1]&&data[i]!=data[i+1])
		{
			data[i]=0x00;
		}
		else if(data[i]==0x00&&data[i]!=data[i-1]&&data[i]!=data[i+1])
		{
			data[i]=0xff;
		}
	}

	for(i=1;i<126;i++)
	{
		if(data[i]==data[i+1]==0xff&&data[i]!=data[i-1]&&data[i]!=data[i+2])
		{
			data[i]=data[i+1]=0x00;
		}
		else if(data[i]==data[i+1]==0x00&&data[i]!=data[i-1]&&data[i]!=data[i+2])
		{
			data[i]=data[i+1]=0xff;
		}
	}
	for(i=1;i<125;i++)
	{
		if(data[i]==data[i+1]==data[i+2]==0xff&&data[i]!=data[i-1]&&data[i]!=data[i+3])
		{
			data[i]=data[i+1]=data[i+2]=0x00;
		}
		else if(data[i]==data[i+1]==data[i+2]==0x00&&data[i]!=data[i-1]&&data[i]!=data[i+3])
		{
			data[i]=data[i+1]=data[i+2]=0xff;
		}
	}
}
//原始数据滤波
void Image_Data_Filter(u8 *data)
{
	u8 i;
	u16 sum;
	for(i=0;i<12;i++)
	{
		sum+=data[i];
	}
	sum=sum/12;
	for(i=0;i<12;i++)
	{
		data[i]=sum;
	}
	sum=0;
	for(i=127;i>115;i--)
	{
		sum+=data[i];
	}
	sum=sum/12;
	for(i=127;i>115;i--)
	{
		data[i]=sum;
	}
}

u8 Last_Center=64;
extern u8 Black_Left,Black_Right,Black_Center;
//寻线
void Image_Data_Search_Line(u8 len,u8 *data)
{
	//u8 i;
	Image_Data_Filter(data);
	Image_Data_Two_Valued(len,data);
	Two_Valued_Data_Filter(data);
    for(int i=1;i<len-1;i++)
    {
        if(data[i-1]==0xff&&data[i]==0xff&&data[i+1]==0x00) //找到白-黑-黑跳变
        {
            Black_Left = i;
        }
        if(data[i-1]==0x00&&data[i]==0x00&&data[i+1]==0xff) //找到黑-黑-白跳变
        {
            Black_Right = i;
        }
        if(Black_Right!=127&&Black_Left!=0)  
        {
            Black_Center = (Black_Left+Black_Right)/2;
            Last_Center = Black_Center;  
            Black_Left = 0;
            Black_Right = 127;
        }
        else
        {
            Black_Center = Last_Center;  //如果没有找到有效黑线，沿用上次中线
            Black_Left = 0;
            Black_Right = 127;
        }
    }
}









