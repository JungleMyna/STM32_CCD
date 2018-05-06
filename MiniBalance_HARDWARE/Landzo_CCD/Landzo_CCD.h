#ifndef _CCD_H_
#define _CCD_H_

#include "sys.h"

#define SI_SetVal()   GPIO_SetBits(GPIOA,GPIO_Pin_4);//PTE4_OUT = 1;
#define SI_ClrVal()   GPIO_ResetBits(GPIOA,GPIO_Pin_4);//PTE4_OUT = 0;
#define CLK_ClrVal()  GPIO_ResetBits(GPIOA,GPIO_Pin_3);//PTE5_OUT = 0;
#define CLK_SetVal()  GPIO_SetBits(GPIOA,GPIO_Pin_3);//PTE5_OUT = 1;


void StartIntegration(void);   
void ImageCapture(unsigned char * ImageData);
void SendHex(unsigned char hex);

void SamplingDelay(void);
void LandzoCCD_init(void);
void CalculateIntegrationTime(u8 *data);
uint8_t PixelAverage(uint8_t len, uint8_t *data) ;
void SendImageData(unsigned char * ImageData) ;


#endif 
