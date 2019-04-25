/*
 * actuator_node_mesh.c
 *
 *  Created on: Apr 23, 2019
 *      Author: aakash
 */

#include "actuator_node_mesh.h"


void update_and_publish_on_off(uint8_t old_state, uint8_t new_state)
{
	printf("sending \n\r");
	struct mesh_generic_state current, target;
	errorcode_t e;

	current.kind = mesh_generic_state_on_off;
	current.on_off.on = old_state; //MESH_GENERIC_ON_OFF_STATE_ON;

	target.kind = mesh_generic_state_on_off;
	target.on_off.on = new_state; //MESH_GENERIC_ON_OFF_STATE_OFF;

	printf("inside send 1\n\r");
	//This goes into default intr handler, not sure why TODO: check and figure this out
//	MESH_CHECK_RESPONSE(
	LOG_INFO("On off update %x",		mesh_lib_generic_server_update(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
	                                        0, // element index for primary is 0
	                                        &current,
	                                        &target,
	                                        0));
	printf("inside send 2\n\r");
//	MESH_CHECK_RESPONSE(
	LOG_INFO("On off server publish %x",		mesh_lib_generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
	                                        0, // element index for primary is 0
	                                        mesh_generic_state_on_off));
						// can this use mesh_generic_state_on_power_up instead
						//to show that the device has powered up
	printf("leaving send \n\r");
}

void onoff_request(uint16_t model_id,
                          uint16_t element_index,
                          uint16_t client_addr,
                          uint16_t server_addr,
                          uint16_t appkey_index,
                          const struct mesh_generic_request *request,
                          uint32_t transition_ms,
                          uint16_t delay_ms,
                          uint8_t request_flags)
{
	static uint8_t button_state = 0x01;

	LOG_DEBUG("Model id 0x%4x element index 0%d client addr 0x%04x server addr 0x%04x "
			"appkey index 0%d transition_ms 0%d delay_ms 0%d request flags 0x%4x",
			model_id,element_index,client_addr,server_addr,appkey_index,
			transition_ms,delay_ms,request_flags);

	//Change the LED state based
	button_state?gpioLed0SetOn():gpioLed0SetOff();

//	update_and_publish_on_off(button_state^0x01,button_state);
	button_state = button_state^0x01;
}

void onoff_change(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
                         uint32_t remaining_ms)
{
//	LOG_INFO("Model id 0x%4x element index 0%d, remaining_ms %d",
//			model_id,element_index,remaining_ms);

	LOG_INFO("onoff_change called with current %x target %x",
			current->on_off,
			target->on_off);
}







/***************************************************************************//**
 * This function process the requests for the light lightness model.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] server_addr    Address the message was sent to.
 * @param[in] appkey_index   The application key index used in encrypting the request.
 * @param[in] request        Pointer to the request structure.
 * @param[in] transition_ms  Requested transition time (in milliseconds).
 * @param[in] delay_ms       Delay time (in milliseconds).
 * @param[in] request_flags  Message flags. Bitmask of the following:
 *                           - Bit 0: Nonrelayed. If nonzero indicates
 *                                    a response to a nonrelayed request.
 *                           - Bit 1: Response required. If nonzero client
 *                                    expects a response from the server.
 ******************************************************************************/
static void lightness_request(uint16_t model_id,
                              uint16_t element_index,
                              uint16_t client_addr,
                              uint16_t server_addr,
                              uint16_t appkey_index,
                              const struct mesh_generic_request *request,
                              uint32_t transition_ms,
                              uint16_t delay_ms,
                              uint8_t request_flags)
{

	LOG_DEBUG("client addr 0x%04x server_addr 0x%04x",client_addr,server_addr);
	LOG_DEBUG("[actual]: level=%u, transition=%lu, delay=%u flags 0x%x",
			         request->lightness, transition_ms, delay_ms, request_flags);

	LOG_DEBUG("%d c[0x%04x] s[0x%04x] value %u flag 0x%x",
			request->kind,
			client_addr,server_addr,request->lightness,request_flags);

	/*
	 * TODO: to handle the case of multiple sensors here,
	 * maybe out of scope for this project
	 */

	switch(request->kind)
	{
	case mesh_lighting_request_lightness_actual:
		//if request flag is zero, sensor error
		set_light_val(request->lightness,request_flags>0);
		//schedule light handling task
		schedule_event(LIGHT_TASK);
		break;

	case mesh_lighting_request_lightness_linear:

		set_water_level(request->lightness);
		//schedule water level handling task
		schedule_event(WATER_TASK);
		break;
	default:
		LOG_INFO("Unhandled lightness type %x",request->kind);
		break;
	}

	//raise external signal to run scheduler
	gecko_external_signal(SET_GECKO_EVENT);
}

/***************************************************************************//**
 * This function is a handler for light lightness change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void lightness_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
	LOG_INFO("Lightness change");
}


void actuator_node_init(void)
{
	mesh_lib_init(malloc, free,10);

	uint16_t res;
	//Initialize Friend functionality
	BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_friend_init());

	pump_control_init();
	light_control_init();

	// Initialize required models
	MESH_CHECK_RESPONSE(mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
	                                           0,
	                                           onoff_request,
	                                           onoff_change));

//	update_and_publish_on_off(0x00,0x01);

	MESH_CHECK_RESPONSE(mesh_lib_generic_server_register_handler(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
			                                           0,
													   lightness_request,
													   lightness_change));

}

