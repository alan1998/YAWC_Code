/*
 * ProjDefs.h
 *
 * Created: 28/12/2015 16:12:51
 *  Author: Alan
 */ 


#ifndef PROJDEFS_H_
#define PROJDEFS_H_

typedef enum MIDI_STATE{
	IDLE,
	NOTE_ON,	//Sending breath (other CCs?)
	PENDING_KEY_NOTE_OFF, // Over breath threshold but no valid key
	PENDING_KEY_NOTE_ON //Key change while over threshold but not valid keys yet
}MIDI_STATE;

#define ON_THRESHOLD 1500
#define BREATH_ADC_MULT 12L
#define BREATH_ADC_DIV 10L
//#define CHANGE_NOTES_AT_BREATH_LEVEL
//#define TOUCH_CYCLE_GAP 6
#define BITE_CYCLE_GAP	3

//Exclusive defines for PB11 toggle debug
#define TOGGLE_DEBUG_SYSTICK 
//#define TOGGLE_DEBUG_TOUCH
//#define TOGGLE_A2D

// Define this to enable touch debug comms out of Sercom4
#define TOUCH_DEBUG_USART

#define MIDDLE_THUMB_NOTE	72
#define LED_PIN PIN_PB30
#define SENSE_DEBUG_PIN PIN_PB11

#define BREATH_A2D_PIN	PIN_PA11

#endif /* PROJDEFS_H_ */