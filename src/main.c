/**
TODO
Back code up put on github
Get A2D calib at start up - Done
Scale midi CC better - Done
Legarto - change note use breath level for new velocity?
Start up and need for reset. Code options or hardware?
Better configuarble key pattern
MOve decode into none interupt routine? Have the interrupt only note changed and stability? Poll for new 
	combination main loop
Thumb rest and stability
Set midi port interrupt above touch (maybe) 
Eventually add watchdog timmer
Buttons for touch recal and breath recal
 */
#include <asf.h>
//#include <delay.h>
#include "ProjDefs.h"
#include "ProjADC.h"
#include "MidiDrv.h"
#include "ProjTouch.h"
#include "touch_api_ptc.h"

////////////////////////////////////
// Global static data
////////////////////////////////////
volatile uint8_t gDoEval = 0;
volatile uint gSysTick = 0;
volatile uint16_t gAdc_val;
extern volatile uint8_t gTouchBusy;
extern volatile uint8_t gKeysNote;
extern volatile uint8_t gLastNote;
extern volatile uint8_t gLastBreath;
extern uint16_t gThreshold;

extern uint uKeyState;
#ifdef TOUCH_DEBUG_USART
extern uint8_t gSendTouchDebugData;
#endif
////////////////////////////////////
// Local functions
////////////////////////////////////
MIDI_STATE EvaluateIdle(void);
MIDI_STATE EvaluateOnPendingKey(void);
MIDI_STATE EvaluateNoteOn(void);
MIDI_STATE EvaluatePendingKeyChange(void); 

static	uint8_t testVel = 101;

int main (void)
{
	system_init();
	struct system_clock_source_dfll_config config_dfll;
	system_clock_source_dfll_get_config_defaults(&config_dfll);

	board_init();
	InitADC();
//	InitDAC();
	InitMidi();
	InitTouch();
	
	MIDI_STATE eState = IDLE;
	MIDI_STATE eNewState = IDLE;
	// Test data
	uint8_t testChannel = 0;
	//////////////////////////////////////
	 
	system_interrupt_enable_global();
	system_interrupt_enable(SYSTEM_INTERRUPT_SYSTICK);
	system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_ADC);
	system_interrupt_enable((const enum system_interrupt_vector)SERCOM2_IRQn);
#ifdef TOUCH_DEBUG_USART
	system_interrupt_enable((const enum system_interrupt_vector)SERCOM4_IRQn);
#endif
//	system_interrupt_set_priority(SYSTEM_INTERRUPT_MODULE_ADC,SYSTEM_INTERRUPT_PRIORITY_LEVEL_1);
	
	SetMidiChannel(testChannel);
//	SendMidiMessage(MIDI_NOTE_ON,60,100);
	GetBaseBreathLevel();
	port_pin_set_output_level(LED_PIN,false);
	p_selfcap_measure_data->measurement_done_touch = 1u;
	while(true)
	{
//		if(gDoTouch<=0){
			if(	p_selfcap_measure_data->measurement_done_touch == 1u){
				touch_sensors_measure();
				#ifdef TOGGLE_DEBUG_TOUCH
				port_pin_set_output_level(SENSE_DEBUG_PIN,true);
				#endif
			}
//		}
		if(gDoEval){
			switch(eState)
			{
				case IDLE:
				{
					eNewState = EvaluateIdle();		
					break;
				}
				case NOTE_ON:
				{
					eNewState = EvaluateNoteOn();
					break;
				}
				case PENDING_KEY_NOTE_ON:
				{
					eNewState = EvaluatePendingKeyChange();
					break;
				}
				case PENDING_KEY_NOTE_OFF:
				{
					eNewState = EvaluateOnPendingKey();
					break;
				}	
			}
			if(eNewState == eState)				
				gDoEval = 0;//Else go straight back round
			eState = eNewState;
		}
		
	};
}

MIDI_STATE EvaluateIdle(void)
{
	MIDI_STATE eRet = IDLE;
	if(gAdc_val > gThreshold){
		//TODO factor this Note code with that below into a method
		//TODO macro for testing note valid bit
		if((gKeysNote & 0x80)	!= 0){
			SendMidiMessage(MIDI_NOTE_ON,gKeysNote & 0x7f,testVel);
			eRet = NOTE_ON;
			gLastNote = gKeysNote & 0x7f;
			port_pin_set_output_level(LED_PIN,true);
		}
		else{
			eRet = PENDING_KEY_NOTE_OFF;		
		}
	}
#ifdef TOUCH_DEBUG_USART
	else if(gSendTouchDebugData==1){
		SendTouchRaw();
	}
#endif
	return eRet;	
}

//State: Over breath threshold but sending note since key unknown
MIDI_STATE EvaluateOnPendingKey(void)
{
	MIDI_STATE eRet = PENDING_KEY_NOTE_OFF;//ASSUME not changing
	if(gAdc_val > gThreshold){
		if((gKeysNote & 0x80)	!= 0){
			SendMidiMessage(MIDI_NOTE_ON,gKeysNote & 0x7f,testVel);
			gLastNote = gKeysNote & 0x7f;
			eRet = NOTE_ON;
			port_pin_set_output_level(LED_PIN,true);
		}
	}
	else{
		eRet = IDLE;
	}
	return eRet;
}

MIDI_STATE EvaluateNoteOn(void)
{
	MIDI_STATE eRet = NOTE_ON;
	if(gAdc_val < gThreshold){
		SendMidiMessage(MIDI_CC,MIDI_BREATH_CC,0);
		if(SendMidiMessage(MIDI_NOTE_OFF,gLastNote,testVel)){
			port_pin_set_output_level(LED_PIN,false);
			eRet = IDLE;
		}
	}
	else if((gKeysNote & 0x7f) != gLastNote){
		if((gKeysNote & 0x80) != 0){
			//Already on new valid key combination
			#ifdef CHANGE_NOTES_AT_BREATH_LEVEL
			SendMidiMessage(MIDI_NOTE_ON,gKeysNote & 0x7f,gLastBreath);
			#else
			SendMidiMessage(MIDI_NOTE_ON,gKeysNote & 0x7f,testVel);
			#endif
			SendMidiMessage(MIDI_NOTE_OFF,gLastNote,gLastBreath);
			gLastNote = gKeysNote & 0x7f;	
		}
		else{
			eRet = PENDING_KEY_NOTE_ON;
		}
	}
	else{
		//Send breath
		uint8_t val = CalcBreathCC();
		SendMidiMessage(MIDI_CC,MIDI_BREATH_CC,val);
	}
	return eRet;
}

//Already in note on state but keys changing but not yet valid
MIDI_STATE EvaluatePendingKeyChange(void)
{
	MIDI_STATE eRet = PENDING_KEY_NOTE_ON;
	if(gAdc_val < gThreshold){
		SendMidiMessage(MIDI_CC,MIDI_BREATH_CC,0);
		if(SendMidiMessage(MIDI_NOTE_OFF,gLastNote,testVel)){
			port_pin_set_output_level(LED_PIN,false);
			eRet = IDLE;
		}
	}
	else{
		if((gKeysNote & 0x80) != 0){
			//Already on new valid key combination
			SendMidiMessage(MIDI_NOTE_ON,gKeysNote & 0x7f,testVel);
			SendMidiMessage(MIDI_NOTE_OFF,gLastNote,testVel);
			gLastNote = gKeysNote & 0x7f;
			eRet = NOTE_ON;
		}
		else{
			uint8_t val = CalcBreathCC();
			SendMidiMessage(MIDI_CC,MIDI_BREATH_CC,val);			
		}
	}
	return eRet;
}

void board_init()
{
	// Set pins for A2D (Breath)
	struct system_pinmux_config config_pinmux;
	system_pinmux_get_config_defaults(&config_pinmux);
	config_pinmux.mux_position = 1;
	config_pinmux.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
	config_pinmux.input_pull = SYSTEM_PINMUX_PIN_PULL_NONE;
	system_pinmux_pin_set_config(BREATH_A2D_PIN, &config_pinmux);

	// Pin out for debug
	system_pinmux_get_config_defaults(&config_pinmux);
	config_pinmux.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	system_pinmux_pin_set_config(SENSE_DEBUG_PIN, &config_pinmux);
	port_pin_set_output_level(SENSE_DEBUG_PIN,false);

	system_pinmux_pin_set_config(LED_PIN, &config_pinmux);
	port_pin_set_output_level(LED_PIN,true);
		
	uint32_t cycles_per_ms = system_gclk_gen_get_hz(0);
	cycles_per_ms /= 1000;
	SysTick_Config(cycles_per_ms*5);
}

void SysTick_Handler()
{
	#ifdef TOGGLE_DEBUG_SYSTICK
	port_pin_toggle_output_level(SENSE_DEBUG_PIN);
	#endif
	gDoEval = 1;
	gSysTick++;	
}
