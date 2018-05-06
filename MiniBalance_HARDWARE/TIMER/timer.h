#ifndef __TIMER_H
#define __TIMER_H
#include <sys.h>	 


#define N 7  //冒泡排序-中值滤波，超声波采集N个数据进行滤波,N为奇数

int selectSort(int arr[],int n);  //n为奇数
void TIM5_Int_Init(u16 arr,u16 psc);

void TIM3_Int_Init(u16 arr,u16 psc);
#endif
