#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"


int Incremental_PI_A(int Encoder,int Target);
int Incremental_PI_B(int Encoder,int Target);
void Xianfu_Pwm(void);
void Set_Pwm(int moto1,int moto2);
int myabs(int a);
int CCD_PID(void);

#endif
