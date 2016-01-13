/*
 * ProjADC.h
 *
 * Created: 31/12/2015 17:04:50
 *  Author: Alan
 */ 


#ifndef PROJADC_H_
#define PROJADC_H_

void InitADC(void);
void GetBaseBreathLevel(void);

void InitDAC(void);
void DAC_Out(uint16_t val);


#endif /* PROJADC_H_ */