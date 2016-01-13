/*
 * ProjTouch.c
 *
 * Created: 28/12/2015 16:52:08
 *  Author: Alan
 */ 
#include <asf.h>
#include "ProjDefs.h"
#include "ProjTouch.h"
#include "touch_api_ptc.h"
#include "MidiDrv.h"

//extern touch_measure_data_t *p_selfcap_measure_data;
extern touch_config_t touch_config;
volatile uint uKeyState = 0;

#ifdef TOUCH_DEBUG_USART
volatile uint8_t gSendTouchDebugData = 0;
#endif

void touch_selfcap_measure_complete_callback( void );
touch_ret_t lib_touch_sensors_init(void);
void touch_configure_ptc_clock(void);
touch_ret_t ConfigYAWCTouch(void);

#ifdef TOUCH_DEBUG_USART
void configure_touch_usart(void);
void write_touch_usart(uint8_t* pBuff, size_t len);
#endif

void InitTouch(void )
{
	lib_touch_sensors_init();
#ifdef TOUCH_DEBUG_USART
	configure_touch_usart();
#endif	
}

void touch_selfcap_measure_complete_callback( void )
{
	//Here is where to evaluate keys and asses if valid
	//Set a global with key state and status 
	static uint Count;
	static uint16_t NewState;
	static uint16_t LastState;
	if (!(p_selfcap_measure_data->acq_status & TOUCH_BURST_AGAIN)) {
#ifdef TOGGLE_DEBUG_TOUCH
		port_pin_set_output_level(SENSE_DEBUG_PIN,false);
#endif
		if((uKeyState & 0xffff) != *((uint16_t*)p_selfcap_measure_data->p_sensor_states)){
			//Possibly don't decode. Just store state. 
			//Do decode and evaluation in loop
			NewState = *((uint16_t*)p_selfcap_measure_data->p_sensor_states);
			if(NewState == LastState)		
				Count++;
			else{
				Count = 0;
				LastState = NewState;
			}
			if(Count > 5U){	
//				Count++;
				DecodeNote(uKeyState=NewState);
			}
		}
		p_selfcap_measure_data->measurement_done_touch = 1u;
	}
}

touch_ret_t touch_sensors_measure(void)
{
	touch_ret_t touch_ret = TOUCH_SUCCESS;
	
	p_selfcap_measure_data->measurement_done_touch = 0u;
	if (!(p_selfcap_measure_data->acq_status & TOUCH_CC_CALIB_ERROR)){
		touch_ret = touch_selfcap_sensors_measure(
		touch_time.current_time_ms,
		NORMAL_ACQ_MODE,
		touch_selfcap_measure_complete_callback);

		if ((touch_ret != TOUCH_ACQ_INCOMPLETE) && (touch_ret == TOUCH_SUCCESS)){
			touch_time.time_to_measure_touch = 0u;
		}
		else if ((touch_ret != TOUCH_SUCCESS) && (touch_ret != TOUCH_ACQ_INCOMPLETE)) {
			while (1)
			{}
			// API config Error
		}
		
	}
	else
	{
		while(1)
		{
		}

	}

	return (touch_ret);
}

touch_ret_t ConfigYAWCTouch(void)
{
	touch_ret_t touch_ret = TOUCH_SUCCESS;
	sensor_id_t sensor_id;

	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_0,CHANNEL_0, NO_AKS_GROUP, 33u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}
	
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_1,CHANNEL_1, NO_AKS_GROUP, 29u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}

	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_2,CHANNEL_2, NO_AKS_GROUP, 28u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}

	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_3,CHANNEL_3, AKS_GROUP_1, 24u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}

	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_4,CHANNEL_4, AKS_GROUP_1, 30u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}

	//Right hand
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_5,CHANNEL_5, NO_AKS_GROUP, 26u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_6,CHANNEL_6, NO_AKS_GROUP, 25u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_7,CHANNEL_7, NO_AKS_GROUP, 29u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_8,CHANNEL_8, AKS_GROUP_3, 28u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_9,CHANNEL_9, AKS_GROUP_3, 24u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_10,CHANNEL_10, AKS_GROUP_3, 26u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}
	//Thumb
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_11,CHANNEL_11, AKS_GROUP_2, 18u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_12,CHANNEL_12, AKS_GROUP_2, 19u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_13,CHANNEL_13, AKS_GROUP_2, 15u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_14,CHANNEL_14, AKS_GROUP_2, 32u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}
	touch_ret = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, CHANNEL_15,CHANNEL_15, AKS_GROUP_2, 33u, HYST_25,RES_8_BIT, &sensor_id);
	if (touch_ret != TOUCH_SUCCESS){
		while (1) ;
	}



	return (touch_ret);
}

void touch_configure_ptc_clock(void)
{
	struct system_gclk_chan_config gclk_chan_conf;

	system_gclk_chan_get_config_defaults(&gclk_chan_conf);

	gclk_chan_conf.source_generator = GCLK_GENERATOR_TOUCH;

	system_gclk_chan_set_config(PTC_GCLK_ID, &gclk_chan_conf);

	system_gclk_chan_enable(PTC_GCLK_ID);

	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, PM_APBCMASK_PTC);
	PM->APBCMASK.reg |= (1u << 11u);
}

touch_ret_t lib_touch_sensors_init(void)
{
	touch_ret_t touch_ret = TOUCH_SUCCESS;

	/* Setup and enable generic clock source for PTC module. */
	touch_configure_ptc_clock();

	touch_time.measurement_period_ms = DEF_TOUCH_MEASUREMENT_PERIOD_MS;

	/* Initialize touch library for Self Cap operation. */
	touch_ret = touch_selfcap_sensors_init(&touch_config);
	if (touch_ret != TOUCH_SUCCESS) {
		while (1u) {    /* Check API Error return code. */
		}
	}

	/* configure the touch library sensors. */
	touch_ret = ConfigYAWCTouch();
	if (touch_ret != TOUCH_SUCCESS) {
		while (1u) {    /* Check API Error return code. */
		}
	}

	/* Auto Tuning setting for calibration.
	 *
	 * AUTO_TUNE_PRSC: When Auto tuning of pre-scaler is selected
	 * the PTC uses the user defined internal series resistor setting
	 * (DEF_MUTLCAP_SENSE_RESISTOR) and the pre-scaler is adjusted
	 * to slow down the PTC operation to ensure full charge transfer.
	 *
	 * AUTO_TUNE_RSEL: When Auto tuning of the series resistor is
	 * selected the PTC runs at user defined pre-scaler setting speed
	 * (DEF_MUTLCAP_CLK_PRESCALE) and the internal series resistor is
	 * tuned automatically to the optimum value to allow for full
	 * charge transfer.
	 *
	 * AUTO_TUNE_NONE: When manual tuning option is selected
	 *(AUTO_TUNE_NONE),
	 * the user defined values of PTC pre-scaler and series resistor is used
	 * for PTC operation as given in DEF_MUTLCAP_CLK_PRESCALE and
	 * DEF_MUTLCAP_SENSE_RESISTOR
	 *
	 */
	touch_ret = touch_selfcap_sensors_calibrate(DEF_SELF_AUTO_TUNE_VALUE);
	if (touch_ret != TOUCH_SUCCESS) {
		while (1u) {    /* Check API Error return code. */
		}
	}

	return (touch_ret);
}

#ifdef TOUCH_DEBUG_USART

struct usart_module touch_usart_instance;

extern volatile int gSendData;

void SendTouchRaw(void)
{
	uint8_t dataByte = DEF_SELFCAP_NUM_SENSORS;
	write_touch_usart(&dataByte,sizeof(uint8_t));
	if(DEF_SELFCAP_NUM_SENSORS <= 8){
		dataByte = *(p_selfcap_measure_data->p_sensor_states);
		write_touch_usart(&dataByte,sizeof(uint8_t));
	}
	else{
		write_touch_usart((uint8_t *)p_selfcap_measure_data->p_sensor_states,sizeof(uint16_t));
	}
	for(uint8_t n=0; n < DEF_SELFCAP_NUM_SENSORS; n++){
		write_touch_usart((uint8_t *)&(p_selfcap_measure_data->p_channel_signals[n]),sizeof(uint16_t));
	}
	for(uint8_t n=0; n < DEF_SELFCAP_NUM_SENSORS; n++){
		write_touch_usart((uint8_t *)&(p_selfcap_measure_data->p_channel_references[n]),sizeof(uint16_t));
	}
	gSendTouchDebugData = 0;
}

void Sercom4Handler(uint8_t inst)
{
	SercomUsart *const usart_hw = &SERCOM4->USART;// &(SERCOM4->hw->USART);
	//
	///* Wait for the synchronization to complete */
	while(usart_hw->STATUS.reg & SERCOM_USART_STATUS_SYNCBUSY){};
	//
	///* Read and mask interrupt flag register */
	uint16_t interrupt_status = usart_hw->INTFLAG.reg;
	if (interrupt_status & SERCOM_USART_INTFLAG_RXC) {
		/* Read out the status code and mask away all but the 4 LSBs*/
		uint8_t  error_code = (uint8_t)(usart_hw->STATUS.reg & SERCOM_USART_STATUS_MASK);
		/* Check if an error has occurred during the receiving */
		if (error_code) {
		}
		else {
			///* Read current packet from DATA register,
				//* increment buffer pointer and decrement buffer length */
			uint16_t received_data = (usart_hw->DATA.reg & SERCOM_USART_DATA_MASK);
			if(received_data == 's'){
				gSendTouchDebugData = 1;
			}			
		}
	}
}

void configure_touch_usart(void)
{
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	config_usart.baudrate    = 56000;
	config_usart.generator_source = GCLK_GENERATOR_3;
	config_usart.mux_setting = USART_RX_1_TX_0_XCK_1;
	config_usart.pinmux_pad0 = PINMUX_PB12C_SERCOM4_PAD0;
	config_usart.pinmux_pad1 = PINMUX_PB13C_SERCOM4_PAD1;
	config_usart.pinmux_pad2 = PINMUX_UNUSED;
	config_usart.pinmux_pad3 = PINMUX_UNUSED;

	while (usart_init(&touch_usart_instance,
	SERCOM4, &config_usart) != STATUS_OK) {
	}

	_sercom_set_handler(4,Sercom4Handler);
	usart_enable(&touch_usart_instance);
	SercomUsart *const usart_hw = &(touch_usart_instance.hw->USART);
	usart_hw->INTENSET.reg = SERCOM_USART_INTFLAG_RXC;

}

void write_touch_usart(uint8_t* pBuff, size_t len)
{
	usart_write_buffer_wait(&touch_usart_instance, pBuff, len);
}




//void _usart_interrupt_handler(
		//uint8_t instance)
//{
	//uint16_t interrupt_status;
	//uint16_t callback_status;
	//uint8_t error_code;
//
	//struct usart_module *module
		//= (struct usart_module *)_sercom_instances[instance];
//
	///* Pointer to the hardware module instance */
	//SercomUsart *const usart_hw
		//= &(module->hw->USART);
//
	///* Wait for the synchronization to complete */
	//_usart_wait_for_sync(module);
//
	///* Read and mask interrupt flag register */
	//interrupt_status = usart_hw->INTFLAG.reg;
	//interrupt_status &= usart_hw->INTENSET.reg;
//
//
	//if (interrupt_status & SERCOM_USART_INTFLAG_RXC) {
		///* Read out the status code and mask away all but the 4 LSBs*/
		//error_code = (uint8_t)(usart_hw->STATUS.reg & SERCOM_USART_STATUS_MASK);
		///* Check if an error has occurred during the receiving */
		//if (error_code) {
			///* Check which error occurred */
			//if (error_code & SERCOM_USART_STATUS_FERR) {
				///* Store the error code and clear flag by writing 1 to it */
				//module->rx_status = STATUS_ERR_BAD_FORMAT;
				//usart_hw->STATUS.reg |= SERCOM_USART_STATUS_FERR;
			//} else if (error_code & SERCOM_USART_STATUS_BUFOVF) {
				///* Store the error code and clear flag by writing 1 to it */
				//module->rx_status = STATUS_ERR_OVERFLOW;
				//usart_hw->STATUS.reg |= SERCOM_USART_STATUS_BUFOVF;
			//} else if (error_code & SERCOM_USART_STATUS_PERR) {
				///* Store the error code and clear flag by writing 1 to it */
				//module->rx_status = STATUS_ERR_BAD_DATA;
				//usart_hw->STATUS.reg |= SERCOM_USART_STATUS_PERR;
			//}
//
//
		//} else {
//
			///* Read current packet from DATA register,
				//* increment buffer pointer and decrement buffer length */
			//uint16_t received_data = (usart_hw->DATA.reg & SERCOM_USART_DATA_MASK);
			//if(received_data == 's'){
				//gSendTouchDebugData = 1;
			//}			
		//}
	//}
//}
#endif