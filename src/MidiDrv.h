/*
 * MidiDrv.h
 *
 * Created: 02/01/2016 14:11:51
 *  Author: Alan
 */ 


#ifndef MIDIDRV_H_
#define MIDIDRV_H_

#define MIDI_NOTE_ON	0x90
#define MIDI_NOTE_OFF	0x80
#define MIDI_CC			0xB0
#define MIDI_BREATH_CC	0x02
#define MIDI_EFFECT1_CC		12
#define MIDI_PITCHBEND	0xE0 

typedef struct MSG{
	uint8_t b0;
	uint8_t b1;
	uint8_t b2;
}MSG;
 
//Map of keys to bit positions
#define EWI_K1	0	// LH 1st finger
#define EWI_K2	1
#define EWI_K3	2
#define EWI_K4	3	// LH Sharp  key
#define EWI_K5	4	// LH Flat key
//Board 2 Right Hand
#define EWI_K6	5
#define EWI_K7	6
#define EWI_K8	7
#define EWI_K9	8
#define EWI_K10	9
#define EWI_K11	10

void InitMidi(void);
uint8_t SendMidiMessage(uint8_t Msg, uint8_t d0, uint8_t d1);
void SetMidiChannel(uint8_t nChannel);
void DecodeNote(uint16_t uState);
uint8_t CalcBreathCC(void);
uint16_t CalcBite(void);
void SendModulation(uint16_t Mod);

#endif /* MIDIDRV_H_ */