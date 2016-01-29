/*
 * MidiDrv.c
 *
 * Created: 02/01/2016 14:11:34
 *  Author: Alan
 *  N.B. Uses Sercom2
 */ 
#include <asf.h>
#include <usart.h>

#include "MidiDrv.h"
#include "ProjDefs.h"

// Data
struct usart_module usart_midi;
static uint8_t gChannel = 0;
extern uint uKeyState;
volatile uint8_t gKeysNote;
volatile uint8_t gLastNote;
volatile uint8_t gLastBreath;
extern uint16_t gAdc_Breath;
extern uint16_t gThreshold;
extern uint16_t gAdc_Bite;
extern float	gFilterBite;

//Circular buffer to place messages into
#define BUFFMax 0x3f
uint8_t MidiCircBuff[BUFFMax+1];	
//volatile uint8_t gMidiReadPtr = 0;
volatile uint8_t gMidiWritePtr = 0;

//Tx buffer for for ASF 
#define BUFF_SIZE 32
uint8_t TxBuff1[BUFF_SIZE];
uint8_t TxBuff2[BUFF_SIZE];
uint8_t* pWriteBuffPtr[2];
volatile uint8_t writeBuff =0;

//Local functions and data
uint8_t WriteMidiBuffer(uint8_t* pData, uint8_t size);
usart_callback_t midi_usart_write_callback(const struct usart_module *const usart_module);

struct usart_config config_usart;

void DecodeNote(uint16_t uState)
{
	// Do key debounce etc in this function
	// Assign any new valid combination to global gNextNote;
	uint16_t uKeys = (uState >> 11);
	int8_t nOctave = 0;
	switch(uKeys){
		case 1:
			nOctave = MIDDLE_THUMB_NOTE-24;
			break;
		case 0x2:
			nOctave = MIDDLE_THUMB_NOTE-12;
			break;
		case 0x4:
			nOctave =MIDDLE_THUMB_NOTE;
			break;
		case 0x8:
			nOctave =MIDDLE_THUMB_NOTE+12;
			break;
		case 0x10:
			nOctave = MIDDLE_THUMB_NOTE+24;
			break;
		default:
			break;
	}
	uKeys = uState & 0x7ff;
	int8_t nNote = 0;
	int8_t nMod = 0;

	//Get modifiers and remove them from what remains
	//LH
	if((uKeys & (1<<EWI_K5)) != 0)
		nMod += -1;
	if((uKeys & (1<<EWI_K4)) != 0)
		nMod += 1;
	//RH
	if((uKeys & (1<<EWI_K9)) != 0)
		nMod -= 1;
//	else if((uKeys & (1<<EWI_K11)) != 0)
//		nMod = -1;
	uint16_t nRKeys = uKeys & ((1<<EWI_K6) | (1<<EWI_K7) | (1<<EWI_K8) | ( 1<<EWI_K10) | (1<<EWI_K11));
//	uKeys &= ~((1<<EWI_K4) | (1<<EWI_K5) | (1<<EWI_K9) | (1<<EWI_K11));
	uint16_t nLKeys = uKeys &((1<<EWI_K1) | (EWI_K2<<1) | (EWI_K3<<1));
	if(nLKeys==0 && nRKeys==0)
		nNote+= 1; //C# no keys
	else{
		switch(nLKeys)//Get 3 LH fingers
		{
			case (1<<EWI_K2):
				nNote -= 0;	//C
				break;
			case (1<<EWI_K1):
			{
				nNote -= 1;	//B
				if(nRKeys == (1<<EWI_K6)) //Bb
					nNote -= 1;
				break;
			}
			case (1<<EWI_K1) | (1<<EWI_K2):
				nNote -= 3;	//A
				break;
			case (1<<EWI_K1) | (1<<EWI_K2)| (1<<EWI_K3):
				nNote -= 5;	//G
				break;
		}
		switch(nRKeys)
		{
			case (1<<EWI_K7):
				nNote -= 1;//F#:
				break;
			case(1<<EWI_K6):
				nNote -= 2; //F
				if(nLKeys == (1<<EWI_K1))
					nNote = -2;
				break;
			case ((1<<EWI_K6) | (1<<EWI_K7)):
				nNote -= 3; //E
				break;
			case ((1<<EWI_K6) | (1<<EWI_K7) | (1<<EWI_K8)):
				nNote -= 5; //D
				break;
			case ((1<<EWI_K6) | (1<<EWI_K7) | (1<<EWI_K8)| (1<<EWI_K10)):
				nNote -= 7; //C
				break;
			case((1<<EWI_K6) | (1<<EWI_K7) | (1<<EWI_K8)| (1<<EWI_K11)):
				nNote -= 8; //B 
				break;
		}
		if(nLKeys == 0)
			nNote +=7;
	}
	nNote += nMod;
		
	if(nOctave != 0){
		nOctave += nNote;
		gKeysNote = nOctave | 0x80;
	}
	else
		gKeysNote = 0;
}

uint8_t CalcBreathCC(void)
{
	int nTemp = gAdc_Breath-gThreshold;//  >> 9) & 0x7f;	
	if(nTemp < 0)
		nTemp = 0;
	else{
		nTemp *= BREATH_ADC_MULT;
		nTemp /= BREATH_ADC_DIV;
		nTemp = nTemp >> 9;
		if(nTemp > 127)
			nTemp = 127;
	}
	gLastBreath = nTemp;
	return gLastBreath;
}

#define AVE_CONST 0.08
#define BITE_SCALE 0.01

uint16_t CalcBite(void)
{
	float Val = (float)gAdc_Bite;
	float PrevVal = gFilterBite;
	gFilterBite = AVE_CONST * Val + (1-AVE_CONST) * gFilterBite;
	Val = PrevVal - gFilterBite;
	Val *= 0.7;
	int uVal = (int)Val + 0x2000;
//	uVal = uVal >> 9;
	if(uVal > 0x3fff)
		uVal = 0x2000;
	if(uVal < 10)
		uVal = 2000;
	return uVal & 0xffff;
}

void SendModulation(uint16_t Mod)
{
	//Turn the 14bit value into correct message
	uint8_t uLeast = Mod & 0x7f;
	uint8_t uHigh = (Mod >> 7) & 0x7f;
	SendMidiMessage(MIDI_PITCHBEND,uLeast, uHigh);
}

void InitMidi(void)
{
	pWriteBuffPtr[0] = TxBuff1;
	pWriteBuffPtr[1] = TxBuff2;
	
	usart_get_config_defaults(&config_usart);
	config_usart.baudrate    = 31250;//19200;//
	config_usart.generator_source = GCLK_GENERATOR_3;
	config_usart.mux_setting = USART_RX_1_TX_0_XCK_1;
	config_usart.pinmux_pad0 = PINMUX_PA12C_SERCOM2_PAD0;
	config_usart.pinmux_pad1 = PINMUX_UNUSED;//PINMUX_PA13C_SERCOM2_PAD1;
	config_usart.pinmux_pad2 = PINMUX_UNUSED;
	config_usart.pinmux_pad3 = PINMUX_UNUSED;
	while (usart_init(&usart_midi,
		SERCOM2, &config_usart) != STATUS_OK) {
	}
	usart_enable(&usart_midi);
	//SERCOM2->USART.INTENSET.bit.TXC = 1;
	usart_enable_transceiver(&usart_midi,USART_TRANSCEIVER_TX);
	
	usart_register_callback(&usart_midi,midi_usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_midi,USART_CALLBACK_BUFFER_TRANSMITTED);
}

uint8_t SendMidiMessage(uint8_t Msg, uint8_t d0, uint8_t d1)
{
	MSG msg;
	msg.b0 = Msg | gChannel;
	msg.b1 = d0;
	msg.b2 = d1;
	return WriteMidiBuffer((uint8_t*)&msg,sizeof(msg));
}

void SetMidiChannel(uint8_t nChannel)
{
	gChannel = nChannel & 0x0f;
}

usart_callback_t midi_usart_write_callback(const struct usart_module *const usart_module)
{
	//Flag write done?
	if(gMidiWritePtr != 0){
		usart_write_buffer_job(&usart_midi,pWriteBuffPtr[writeBuff],gMidiWritePtr);
		writeBuff ^= 1;
		gMidiWritePtr = 0;		
	}
	return 0;
}

uint8_t WriteMidiBuffer(uint8_t* pData, uint8_t size)
{
	if((gMidiWritePtr + size) >= BUFF_SIZE)
		return false;
	//Enough space in current buffer so carry on
	for(uint8_t n=0; n < size; n++){
		pWriteBuffPtr[writeBuff][gMidiWritePtr++] = *(pData++);
	}
	if(usart_get_job_status(&usart_midi, USART_TRANSCEIVER_TX) == STATUS_OK){
		usart_write_buffer_job(&usart_midi,pWriteBuffPtr[writeBuff],gMidiWritePtr);
		writeBuff ^= 1;
		gMidiWritePtr = 0;
	}
	return true;
}

