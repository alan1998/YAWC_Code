/*
 * ProjADC.h
 *
 * Created: 31/12/2015 17:04:50
 *  Author: Alan
 */ 


#ifndef PROJADC_H_
#define PROJADC_H_
#define BREATH_CHAN_OFF 0
#define MOUTH_CHAN_OFF	1

void InitADC(void);
void GetBaseBreathLevel(void);

void InitDAC(void);
void DAC_Out(uint16_t val);


#endif /* PROJADC_H_ */