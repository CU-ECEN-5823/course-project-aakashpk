/*
 * actuator_node_mesh.c
 *
 *  Created on: Apr 23, 2019
 *      Author: aakash
 */

#include "actuator_node_mesh.h"

errorcode_t update_and_publish_on_off(uint8_t old_state, uint8_t new_state)
{
	struct mesh_generic_state current, target;
	errorcode_t e;

	current.kind = mesh_generic_state_on_off;
	current.on_off.on = old_state; //MESH_GENERIC_ON_OFF_STATE_ON;

	target.kind = mesh_generic_state_on_off;
	target.on_off.on = new_state; //MESH_GENERIC_ON_OFF_STATE_OFF;


	e = mesh_lib_generic_server_update(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
	                                        0, // element index for primary is 0
	                                        &current,
	                                        &target,
	                                        0);

	  if (e == bg_err_success)
	  {
	    e = mesh_lib_generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
	                                        0, // element index for primary is 0
	                                        mesh_generic_state_on_off);
	    if(e != bg_err_success)
	    {
	    	LOG_ERROR("Server publish failed --, reason %x",e);
	    }
	  }
	  else
		  LOG_ERROR("Server update failed --, reason %x",e);

	  return e;

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

	displayPrintf(DISPLAY_ROW_TEMPVALUE,"LED %s",(button_state?"ON":"OFF"));
	button_state?gpioLed0SetOn():gpioLed0SetOff();
	//update_and_publish_on_off(button_state^0x01,button_state);
	button_state = button_state^0x01;
}

void onoff_change(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
                         uint32_t remaining_ms)
{
	LOG_INFO("Model id 0x%4x element index 0%d, remaining_ms %d",
			model_id,element_index,remaining_ms);

	LOG_INFO("onoff_change called with current %x target %x",
			current->on_off,
			target->on_off);
}




/***************************************************************************//**
 * This function process the requests for the light CTL model.
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
void ctl_request(uint16_t model_id,
                        uint16_t element_index,
                        uint16_t client_addr,
                        uint16_t server_addr,
                        uint16_t appkey_index,
                        const struct mesh_generic_request *request,
                        uint32_t transition_ms,
                        uint16_t delay_ms,
                        uint8_t request_flags)
{
  LOG_INFO("ctl_request: lightness=%u, temperature=%u, delta_uv=%d, transition=%lu, delay=%u",
         request->ctl.lightness, request->ctl.temperature, request->ctl.deltauv, transition_ms, delay_ms);

  //ctl_update_and_publish(element_index, remaining_ms);
}

/***************************************************************************//**
 * This function is a handler for light CTL change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
void ctl_change(uint16_t model_id,
                       uint16_t element_index,
                       const struct mesh_generic_state *current,
                       const struct mesh_generic_state *target,
                       uint32_t remaining_ms)
{
	LOG_INFO("CTL change called");
  if (current->kind != mesh_lighting_state_ctl) {
    // if kind is not 'ctl' then just report the change here
    printf("ctl change, kind %u\r\n", current->kind);
    return;
  }

}


/***************************************************************************//**
 * Update light CTL state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns bg_err_success (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static errorcode_t ctl_update(uint16_t element_index, uint32_t remaining_ms)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_ctl;
  current.ctl.lightness = 1;
  current.ctl.temperature = 2;
  current.ctl.deltauv =3;

  target.kind = mesh_lighting_state_ctl;
  target.ctl.lightness = 4;
  target.ctl.temperature = 5;
  target.ctl.deltauv = 6;

  return mesh_lib_generic_server_update(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                                        element_index,
                                        &current,
                                        &target,
                                        remaining_ms);
}




/***************************************************************************//**
 * Update light CTL state and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns bg_err_success (0) if succeed, non-zero otherwise.
 ******************************************************************************/
errorcode_t ctl_update_and_publish(uint16_t element_index,
                                          uint32_t remaining_ms)
{
  errorcode_t e;

  e = ctl_update(element_index, remaining_ms);

  if (e == bg_err_success) {

    e = mesh_lib_generic_server_publish(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                                        element_index,
                                        mesh_lighting_state_ctl);
  }

  return e;
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

	switch(request->kind)
	{
	case mesh_lighting_request_lightness_actual:
		LOG_INFO("lightness_request actual: level=%u, transition=%lu, delay=%u",
				         request->lightness, transition_ms, delay_ms);
		LOG_INFO("Sensor Value = %f",request->lightness/1000.0);
		break;
	case mesh_lighting_request_lightness_linear:
		LOG_INFO("lightness_request linear: level=%u, transition=%lu, delay=%u\r\n",
						         request->lightness, transition_ms, delay_ms);
		break;
	default:
		LOG_INFO("Unhandled lightness type %x",request->kind);
		break;
	}

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
	  LOG_INFO("Friend mode initialization ");

	  res = gecko_cmd_mesh_friend_init()->result;
	  if (res) {
		LOG_INFO("Friend init failed 0x%x", res);
	  }

	LOG_INFO("register model %d",mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
	                                           0,
	                                           onoff_request,
	                                           onoff_change));

	LOG_INFO("register model %d",mesh_lib_generic_server_register_handler(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
			                                           0,
													   lightness_request,
													   lightness_change));

}

