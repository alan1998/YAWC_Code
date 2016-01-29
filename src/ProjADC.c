/*
 * ProjADC.c
 *
 * Created: 31/12/2015 17:04:32
 *  Author: Alan
 */ 
#include "asf.h"
#include <dac.h>

#include "ProjDefs.h"
#include "ProjADC.h"

struct adc_module adc_instance;

extern uint16_t gAdc_Breath;
extern uint16_t gAdc_Bite;
extern uint16_t gThreshold;
extern uint gSysTick;
uint16_t gThreshold;
//void adc_complete_callback( const struct adc_module * const module)
//{
	////Flag something?
//}

void InitADC(void)
{
	struct adc_config config;
	adc_get_config_defaults(&config);
	
	config.clock_source = GCLK_GENERATOR_3;
	config.clock_prescaler = ADC_CLOCK_PRESCALER_DIV8;
	config.positive_input = ADC_POSITIVE_INPUT_PIN18;
	config.accumulate_samples = ADC_AVGCTRL_SAMPLENUM_16_Val;
	config.divide_result = ADC_DIVIDE_RESULT_DISABLE;
	config.resolution = ADC_RESOLUTION_CUSTOM;
	config.gain_factor = ADC_INPUTCTRL_GAIN_2X;
	config.freerunning = true;
	config.sample_length = 5;
	config.pin_scan.inputs_to_scan = 2;
	config.pin_scan.offset_start_scan = 0;
	
	
	adc_init(&adc_instance,ADC,&config);
	adc_enable(&adc_instance);
	
	ADC->INTENSET.reg = 1; //Result ready
}

void ADC_Handler(void)
{
	
	uint Pos = ADC->INPUTCTRL.reg;
	Pos = (Pos >> 20) & 0xf;
	switch(Pos)
	{
		case BREATH_CHAN_OFF:
			gAdc_Breath = ADC->RESULT.reg;
			break;
		case MOUTH_CHAN_OFF:
			gAdc_Bite = ADC->RESULT.reg;
			break;
		default:
		break;
	}
	
	ADC->INTFLAG.bit.RESRDY =1;

	#ifdef TOGGLE_A2D
	port_pin_toggle_output_level(SENSE_DEBUG_PIN);
	#endif
}

void GetBaseBreathLevel(void)
{
	uint nNow = gSysTick;
	uint nSum = 0;
	for(uint n=0; n < (1<< 6);n++)
	{
		while(nNow == gSysTick)
		{};
		nSum += gAdc_Breath;
		nNow == gSysTick;
	}
	gThreshold = (nSum >> 6);
	gThreshold += ON_THRESHOLD;  	
}

struct dac_module dac_inst;

void InitDAC(void)
{
	struct dac_config config_dac;
	dac_get_config_defaults(&config_dac);
	dac_init(&dac_inst, DAC, &config_dac);
	dac_enable(&dac_inst);

	struct dac_chan_config config_dac_chan;
	dac_chan_get_config_defaults(&config_dac_chan);
	dac_chan_set_config(&dac_inst, DAC_CHANNEL_0, &config_dac_chan);
	dac_chan_enable(&dac_inst, DAC_CHANNEL_0);
}

void DAC_Out(uint16_t val)
{
	dac_chan_write(&dac_inst,DAC_CHANNEL_0,val);
}

