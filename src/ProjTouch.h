/*
 * ProjTouch.h
 *
 * Created: 28/12/2015 16:52:25
 *  Author: Alan
 */ 


#ifndef PROJTOUCH_H_
#define PROJTOUCH_H_
#include "ProjDefs.h"

void InitTouch(void );

#ifdef TOUCH_DEBUG_USART
void SendTouchRaw(void);
#endif

//  N.B. GenClk needs to be 4MHz
#define GCLK_GENERATOR_TOUCH GCLK_GENERATOR_3

#define DEF_TOUCH_SELFCAP               (1)
#define DEF_TOUCH_MUTLCAP               (0)

// ISR
#define   DEF_TOUCH_PTC_ISR_LVL       (3u)

// Thumb #define DEF_SELFCAP_LINES  Y(10),Y(11),Y(12),Y(13),Y(14)
//Left hand
#define DEF_SELFCAP_LINES  Y(5),Y(4),Y(3),Y(2),Y(15),  Y(6),Y(7),Y(8),Y(9),Y(0),Y(1),  Y(10),Y(11),Y(12),Y(13),Y(14)
#define DEF_SELFCAP_NUM_CHANNELS   (16) //Total number of channels 
#define DEF_SELFCAP_NUM_SENSORS  (16)  //Total number of sensors  
#define DEF_SELFCAP_NUM_ROTORS_SLIDERS  (0)  //Number of rotor sliders

#define DEF_SELFCAP_GAIN_PER_NODE  GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1,GAIN_1

#define DEF_SELFCAP_CLK_PRESCALE_PER_NODE PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1,PRSC_DIV_SEL_1

#define DEF_SELFCAP_SENSE_RESISTOR_PER_NODE RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0,RSEL_VAL_0

#define DEF_SELFCAP_HOP_FREQS  FREQ_HOP_SEL_1,FREQ_HOP_SEL_2,FREQ_HOP_SEL_3

/*----------------------------------------------------------------------------
 *                   Self Cap method aquisition parameters.
 *
 *
 *----------------------------------------------------------------------------*/
#define  DEF_SELFCAP_FILTER_LEVEL FILTER_LEVEL_4            // Filter level

#define  DEF_SELFCAP_AUTO_OS 2                 // Automatic OverSampling

#define DEF_SELFCAP_AUTO_OS_SIGNAL_STABILITY_LIMIT 20

#define DEF_SELFCAP_FREQ_MODE FREQ_MODE_NONE


#define DEF_SELF_AUTO_TUNE_VALUE		0

#define DEF_TOUCH_MEASUREMENT_PERIOD_MS 20u

#define   DEF_SELFCAP_DI         4u

#define  DEF_SELFCAP_TCH_DRIFT_RATE        120u

#define  DEF_SELFCAP_ATCH_DRIFT_RATE         120u

#define   DEF_SELFCAP_MAX_ON_DURATION       0u

#define   DEF_SELFCAP_DRIFT_HOLD_TIME        255u

#define   DEF_SELFCAP_ATCH_RECAL_DELAY       10u

#define   DEF_SELFCAP_ATCH_RECAL_THRESHOLD      0u

#define   DEF_SELFCAP_TOUCH_POSTPROCESS_MODE       TOUCH_LIBRARY_DRIVEN

#define   DEF_SELFCAP_MOIS_TOLERANCE_ENABLE       (0u)

#define   DEF_SELFCAP_MOIS_QUICK_REBURST_ENABLE       (1u)

#define   DEF_SELFCAP_NUM_MOIS_GROUPS       (0u)

#define   DEF_SELFCAP_AKS_ENABLE       (1u)

#define DEF_SELF_CAP_CSD_VALUE (0u)

#define   DEF_SELFCAP_NOISE_MEAS_ENABLE       (0u)

#define   DEF_SELFCAP_NOISE_MEAS_SIGNAL_STABILITY_LIMIT       10u

#define   DEF_SELFCAP_NOISE_LIMIT       12u

#define   DEF_SELFCAP_NOISE_MEAS_BUFFER_CNT       (5u)

#define   DEF_SELFCAP_LOCKOUT_SEL       0

#define   DEF_SELFCAP_LOCKOUT_CNTDOWN       10

#define   DEF_SELFCAP_FREQ_AUTO_TUNE_ENABLE       (0u)

#define   DEF_SELFCAP_FREQ_AUTO_TUNE_SIGNAL_STABILITY_LIMIT       10u

#define   DEF_SELFCAP_FREQ_AUTO_TUNE_IN_CNT       12

#define DEF_SELFCAP_FILTER_CALLBACK              (NULL)


#define DEF_TOUCH_QDEBUG_ENABLE_SELFCAP

#define DEF_TOUCH_QDEBUG_ENABLE 0u


#endif /* PROJTOUCH_H_ */