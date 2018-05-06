#include "show.h"

int filter_dist_F=111,filter_dist_L=122,filter_dist_R=133;
/**************************************************************************
函数功能：OLED显示
入口参数：无
返回  值：无
**************************************************************************/
void oled_show(void)
{
//	OLED_ShowString(00,0,"Dist_F");
//	OLED_ShowNumber(80,0,Distance_F,3,12);  
	
	
	
	OLED_ShowString(00,0,"Dist_F:");
	OLED_ShowNumber(62,0,filter_dist_F/10,3,12);
	OLED_ShowString(85,0,".");
	OLED_ShowNumber(87,0,filter_dist_F%10,2,12);
    OLED_ShowString(104,0,"cm");
      
	OLED_ShowString(00,10,"Dist_L:");
	OLED_ShowNumber(62,10,filter_dist_L/10,3,12);
	OLED_ShowString(85,10,".");
	OLED_ShowNumber(87,10,filter_dist_L%10,2,12);
    OLED_ShowString(104,10,"cm");
	
	OLED_ShowString(00,20,"Dist_R:");
	OLED_ShowNumber(62,20,filter_dist_R/10,3,12);
	OLED_ShowString(85,20,".");
	OLED_ShowNumber(87,20,filter_dist_R%10,2,12);
    OLED_ShowString(104,20,"cm");
	
//	OLED_ShowString(00,40,"Mode:");
//    OLED_ShowNumber(40,40,mode_flag,2,12);
//	
//	OLED_ShowString(00,50,"L:");
//    OLED_ShowNumber(40,50,Left_Lose_Flag,2,12);
//	
//	OLED_ShowString(60,50,"R:");
//    OLED_ShowNumber(100,50,Right_Lose_Flag,2,12);
	
//=============刷新=======================//
	OLED_Refresh_Gram();	
}

