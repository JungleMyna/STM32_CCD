#include "Landzo_CCD.h"
#include "adc.h"
#include "delay.h"
#include "stm32f10x_adc.h"
/*************************************************************************
*
*  函数名称：CCD_init
*  功能说明：CCD初始化
*  参数说明：
*  函数返回：无
*  备    注：
*************************************************************************/
void LandzoCCD_init(void)
{
//  gpio_init (PTE4, GPO,HIGH);
//  gpio_init (PTE5, GPO,HIGH);
     GPIO_InitTypeDef  GPIO_InitStructure;
        
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB,PE端口时钟
        
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_3;	 // 端口配置
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
     GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
     GPIO_SetBits(GPIOA,GPIO_Pin_3|GPIO_Pin_4);			     //PE.4 PE.6输出高
    //  adc_init(ADC1, AD6b) ;
     Adc_Init();		//ADC初始化 
}

/*************************************************************************
*
*  函数名称：StartIntegration
*  功能说明：CCD启动程序
*  参数说明：
*  函数返回：无
*  备    注：
*************************************************************************/
void StartIntegration(void) {

    unsigned char i;

    SI_SetVal();            /* SI  = 1 */
    SamplingDelay();
    CLK_SetVal();           /* CLK = 1 */
    SamplingDelay();
    SI_ClrVal();            /* SI  = 0 */
    SamplingDelay();
    CLK_ClrVal();           /* CLK = 0 */

    for(i=0; i<127; i++) {
        SamplingDelay();
        SamplingDelay();
        CLK_SetVal();       /* CLK = 1 */
        SamplingDelay();
        SamplingDelay();
        CLK_ClrVal();       /* CLK = 0 */
    }
    SamplingDelay();
    SamplingDelay();
    CLK_SetVal();           /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    CLK_ClrVal();           /* CLK = 0 */
}

/*************************************************************************
*
*  函数名称：ImageCapture
*  功能说明：CCD采样程序
*  参数说明：* ImageData   采样数组
*  函数返回：无
*  备    注：
*ImageData =  ad_once(ADC1, AD6a, ADC_8bit);
*************************************************************************/

void ImageCapture(uint8_t * ImageData) {
    uint8_t i;
    //extern uint8_t AtemP ;

    SI_SetVal();            /* SI  = 1 */
    delay_us(20);
    CLK_SetVal();           /* CLK = 1 */
	delay_us(20);
    SI_ClrVal();            /* SI  = 0 */
    delay_us(30);
//Delay 10us for sample the first pixel
    for(i = 0; i < 250; i++) {                    //更改250，让CCD的图像看上去比较平滑，
      SamplingDelay();  //200ns                  //把该值改大或者改小达到自己满意的结果。
    }
//Sampling Pixel 1

    *ImageData =  Get_Adc(ADC_Channel_1)>>4;
    ImageData ++ ;
    CLK_ClrVal();           /* CLK = 0 */

    for(i=0; i<127; i++) {
        delay_us(30);
        CLK_SetVal();       /* CLK = 1 */
        delay_us(30);
        //Sampling Pixel 2~128
       *ImageData =  Get_Adc(ADC_Channel_1)>>4;	 
        ImageData ++ ;
        CLK_ClrVal();       /* CLK = 0 */
    }
    delay_us(30);
    CLK_SetVal();           /* CLK = 1 */
	delay_us(30);
    CLK_ClrVal();           /* CLK = 0 */
}


/*************************************************************************
*
*  函数名称：CalculateIntegrationTime
*  功能说明：计算曝光时间
*  参数说明：
*  函数返回：无
*  备    注：
*************************************************************************/

/* 曝光时间，单位ms */
uint8_t IntegrationTime = 10;
void CalculateIntegrationTime(u8 *data) {
//extern uint8_t Pixel[128];
/* 128个像素点的平均AD值 */
uint8_t PixelAverageValue;
/* 128个像素点的平均电压值的10倍 */
uint8_t PixelAverageVoltage;
/* 设定目标平均电压值，实际电压的10倍 */
int16_t TargetPixelAverageVoltage = 20;
/* 设定目标平均电压值与实际值的偏差，实际电压的10倍 */
int16_t PixelAverageVoltageError = 0;
/* 设定目标平均电压值允许的偏差，实际电压的10倍 */
int16_t TargetPixelAverageVoltageAllowError = 2;

    /* 计算128个像素点的平均AD值 */
    PixelAverageValue = PixelAverage(128,data);
    /* 计算128个像素点的平均电压值,实际值的10倍 */
    PixelAverageVoltage = (uint8_t)((int16_t)PixelAverageValue * 25 / 194);

    PixelAverageVoltageError = TargetPixelAverageVoltage - PixelAverageVoltage;
    if(PixelAverageVoltageError < -TargetPixelAverageVoltageAllowError)
    {
      PixelAverageVoltageError = 0- PixelAverageVoltageError ;
      PixelAverageVoltageError /= 5;
      if(PixelAverageVoltageError > 10 )
         PixelAverageVoltageError = 10 ;
       IntegrationTime -= PixelAverageVoltageError;
    }
    if(PixelAverageVoltageError > TargetPixelAverageVoltageAllowError)
    { 
        PixelAverageVoltageError /= 5;
        if(PixelAverageVoltageError > 10 )
           PixelAverageVoltageError = 10 ;
        IntegrationTime += PixelAverageVoltageError;}
 
/*    
    uart_putchar(UART0,0XAA) ;
    uart_putchar(UART0,PixelAverageVoltage) ;
    uart_putchar(UART0,PixelAverageVoltageError) ;
    uart_putchar(UART0,IntegrationTime) ;
 */   
    if(IntegrationTime <= 1)
        IntegrationTime = 1;
    if(IntegrationTime >= 100)
        IntegrationTime = 100;
}

/*************************************************************************
*
*  函数名称：PixelAverage
*  功能说明：求数组的均值程序
*  参数说明：
*  函数返回：无
*  备    注：
*************************************************************************/
uint8_t PixelAverage(uint8_t len, uint8_t *data) {
  unsigned char i;
  unsigned int sum = 0;
  for(i = 0; i<len; i++) {
    sum = sum + *data++;
  }
  return ((unsigned char)(sum/len));
}
/*************************************************************************
*
*  函数名称：SendHex
*  功能说明：采集发数程序
*  参数说明：
*  函数返回：无
*  备    注：
*************************************************************************/
void SendHex(unsigned char hex) {
  unsigned char temp;
  temp = hex >> 4;
  if(temp < 10) {
    USART_SendData(USART1,temp + '0');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
  } else {
    USART_SendData(USART1,temp - 10 + 'A');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
  }
  temp = hex & 0x0F;
  if(temp < 10) {
    USART_SendData(USART1,temp + '0');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
  } else {
   USART_SendData(USART1,temp - 10 + 'A');
   while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
  }
}
/*************************************************************************
*
*  函数名称：SendImageData
*  功能说明：
*  参数说明：
*  函数返回：无
*  备    注：
*************************************************************************/
void SendImageData(unsigned char * ImageData) {

    unsigned char i;
    unsigned char crc = 0;

    /* Send Data */
    USART_SendData(USART1,'*');//向串口1发送数据//uart_putchar(UART0,'*');
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
    USART_SendData(USART1,'L');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
    USART_SendData(USART1,'D');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
    SendHex(0);
    SendHex(0);
    SendHex(0);
    SendHex(0);

    for(i=0; i<128; i++) {
      SendHex(*ImageData++);
    }

    SendHex(crc);
    USART_SendData(USART1,'#');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
}
/*************************************************************************
*
*  函数名称：SamplingDelay
*  功能说明：CCD延时程序 
*  参数说明：
*  函数返回：无
*  备    注：
*************************************************************************/
void SamplingDelay(void){
   volatile uint8_t i ;
   for(i=0;i<1;i++) {
    __NOP();
    __NOP();}
   
}
