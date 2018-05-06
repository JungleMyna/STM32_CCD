#include "control.h"	


int Encoder_Left,Encoder_Right;    //左右编码器的脉冲计数
int Target_Left=10,Target_Right=10;      //目标速度，用于电机PID
extern int Moto1,Moto2;            //电机PWM变量 	
extern u8 Black_Center;

void TIM5_IRQHandler(void)   //TIM2中断
{
	if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
        
        Encoder_Left=Read_Encoder(2);                                      //===读取编码器的值
		Encoder_Right=Read_Encoder(4);                                      //===读取编码器的值 
        Moto1=Incremental_PI_A(Encoder_Left,Target_Left)-CCD_PID(); //===速度闭环控制计算电机A最终PWM
		Moto2=Incremental_PI_B(Encoder_Right,Target_Right)+CCD_PID();
        Xianfu_Pwm();                                                       //===PWM限幅
		Set_Pwm(Moto1,Moto2); //===赋值给PWM寄存器
	}
}


//ccd提取中线反馈，给电机进行PD差速
int kerror[2]={0,0};
float CCD_kp=16.8,CCD_kd=5.8;
int CCD_PID(void)
{
	float Sum=0;
	kerror[0]=(Black_Center-(IMAGE_DATA/2));
	Sum = CCD_kp*kerror[0]+CCD_kd*(kerror[0]-kerror[1]);
	kerror[1] = kerror[0];
	return (int)Sum;
}


/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：右轮PWM、左轮PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int moto1,int moto2)
{
	if(moto1>=0)	BIN2=0,			BIN1=1;
	else 	    BIN2=1,			BIN1=0;
		PWMB=myabs(moto1);   //右轮
	if(moto2>=0)	AIN1=0,			AIN2=1;
	else        AIN1=1,			AIN2=0;
		PWMA=myabs(moto2);	 //左轮
}


/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式 
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
float Velocity_KP=12,Velocity_KI=12;	       //速度控制PID参数
int Incremental_PI_A (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_PI_B (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}

/**************************************************************************
函数功能：限制PWM赋值 
入口参数：无
返回  值：无
**************************************************************************/
void Xianfu_Pwm(void)
{	
	int Amplitude=6900;    //===PWM满幅是7200 限制在6900
    
    if(Moto1<-Amplitude) 
        Moto1=-Amplitude;	
	if(Moto1>Amplitude)  
        Moto1=Amplitude;	
	if(Moto2<-Amplitude) 
        Moto2=-Amplitude;	
	if(Moto2>Amplitude)  
        Moto2=Amplitude;		
}

/**************************************************************************
函数功能：绝对值函数
入口参数：int
返回  值：unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	int temp;
	if(a<0)  
        temp=-a;  
	else 
        temp=a;
	return temp;
}


