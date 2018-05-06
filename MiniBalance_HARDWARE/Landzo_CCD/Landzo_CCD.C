#include "Landzo_CCD.h"
#include "adc.h"
#include "delay.h"
#include "stm32f10x_adc.h"
/*************************************************************************
*
*  �������ƣ�CCD_init
*  ����˵����CCD��ʼ��
*  ����˵����
*  �������أ���
*  ��    ע��
*************************************************************************/
void LandzoCCD_init(void)
{
//  gpio_init (PTE4, GPO,HIGH);
//  gpio_init (PTE5, GPO,HIGH);
     GPIO_InitTypeDef  GPIO_InitStructure;
        
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
        
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_3;	 // �˿�����
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
     GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
     GPIO_SetBits(GPIOA,GPIO_Pin_3|GPIO_Pin_4);			     //PE.4 PE.6�����
    //  adc_init(ADC1, AD6b) ;
     Adc_Init();		//ADC��ʼ�� 
}

/*************************************************************************
*
*  �������ƣ�StartIntegration
*  ����˵����CCD��������
*  ����˵����
*  �������أ���
*  ��    ע��
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
*  �������ƣ�ImageCapture
*  ����˵����CCD��������
*  ����˵����* ImageData   ��������
*  �������أ���
*  ��    ע��
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
    for(i = 0; i < 250; i++) {                    //����250����CCD��ͼ����ȥ�Ƚ�ƽ����
      SamplingDelay();  //200ns                  //�Ѹ�ֵ�Ĵ���߸�С�ﵽ�Լ�����Ľ����
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
*  �������ƣ�CalculateIntegrationTime
*  ����˵���������ع�ʱ��
*  ����˵����
*  �������أ���
*  ��    ע��
*************************************************************************/

/* �ع�ʱ�䣬��λms */
uint8_t IntegrationTime = 10;
void CalculateIntegrationTime(u8 *data) {
//extern uint8_t Pixel[128];
/* 128�����ص��ƽ��ADֵ */
uint8_t PixelAverageValue;
/* 128�����ص��ƽ����ѹֵ��10�� */
uint8_t PixelAverageVoltage;
/* �趨Ŀ��ƽ����ѹֵ��ʵ�ʵ�ѹ��10�� */
int16_t TargetPixelAverageVoltage = 20;
/* �趨Ŀ��ƽ����ѹֵ��ʵ��ֵ��ƫ�ʵ�ʵ�ѹ��10�� */
int16_t PixelAverageVoltageError = 0;
/* �趨Ŀ��ƽ����ѹֵ������ƫ�ʵ�ʵ�ѹ��10�� */
int16_t TargetPixelAverageVoltageAllowError = 2;

    /* ����128�����ص��ƽ��ADֵ */
    PixelAverageValue = PixelAverage(128,data);
    /* ����128�����ص��ƽ����ѹֵ,ʵ��ֵ��10�� */
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
*  �������ƣ�PixelAverage
*  ����˵����������ľ�ֵ����
*  ����˵����
*  �������أ���
*  ��    ע��
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
*  �������ƣ�SendHex
*  ����˵�����ɼ���������
*  ����˵����
*  �������أ���
*  ��    ע��
*************************************************************************/
void SendHex(unsigned char hex) {
  unsigned char temp;
  temp = hex >> 4;
  if(temp < 10) {
    USART_SendData(USART1,temp + '0');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
  } else {
    USART_SendData(USART1,temp - 10 + 'A');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
  }
  temp = hex & 0x0F;
  if(temp < 10) {
    USART_SendData(USART1,temp + '0');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
  } else {
   USART_SendData(USART1,temp - 10 + 'A');
   while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
  }
}
/*************************************************************************
*
*  �������ƣ�SendImageData
*  ����˵����
*  ����˵����
*  �������أ���
*  ��    ע��
*************************************************************************/
void SendImageData(unsigned char * ImageData) {

    unsigned char i;
    unsigned char crc = 0;

    /* Send Data */
    USART_SendData(USART1,'*');//�򴮿�1��������//uart_putchar(UART0,'*');
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
    USART_SendData(USART1,'L');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
    USART_SendData(USART1,'D');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
    SendHex(0);
    SendHex(0);
    SendHex(0);
    SendHex(0);

    for(i=0; i<128; i++) {
      SendHex(*ImageData++);
    }

    SendHex(crc);
    USART_SendData(USART1,'#');
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
}
/*************************************************************************
*
*  �������ƣ�SamplingDelay
*  ����˵����CCD��ʱ���� 
*  ����˵����
*  �������أ���
*  ��    ע��
*************************************************************************/
void SamplingDelay(void){
   volatile uint8_t i ;
   for(i=0;i<1;i++) {
    __NOP();
    __NOP();}
   
}