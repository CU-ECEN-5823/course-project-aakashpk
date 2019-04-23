/*
 * gecko_helper.c
 *
 *  Created on: Feb 20, 2019
 *      Author: aakash
 */

#include "gecko_helper.h"

static uint8_t trid; // transaction id
/// Flag for indicating that lpn feature is active
static uint8 lpn_active = 0;

/**
 * Initialize LPN functionality with configuration and friendship establishment.
 */
void lpn_init(void)
{
  uint16 res;
  // Initialize LPN functionality.
  res = gecko_cmd_mesh_lpn_init()->result;
  if (res) {
    LOG_INFO("LPN init failed (0x%x)", res);
    return;
  }

  /*
   * param 1 Minimum queue length the friend must support.
   * This value should be chosen based on the expected message
   * frequency and LPN sleep period, as messages that do not fit
   * into the friend queue are dropped.
   * Note that the given value is rounded up to the nearest
   * power of 2.             Range: 2..128
   *
   * Poll timeout in milliseconds. Poll timeout is the
   * longest time LPN will sleep in between querying its friend for
   * queued messages. Long poll timeout allows the LPN to sleep for
   * longer periods, at the expense of increased latency for receiving
   * messages.Note that the given value is rounded up to the nearest 100ms
   */
  res = gecko_cmd_mesh_lpn_configure(2, 5 * 1000)->result;
  if (res) {
    LOG_INFO("LPN conf failed (0x%x)", res);
    return;
  }

  LOG_INFO("trying to find friend...");
  res = gecko_cmd_mesh_lpn_establish_friendship(0)->result;

  if (res != 0) {
    LOG_INFO("Friend failed with code %x\r\n", res);
  }
  else lpn_active = 1;
}

/***************************************************************************//**
 * Deinitialize LPN functionality.
 ******************************************************************************/
void lpn_deinit(void)
{
  uint16 result;

  if (!lpn_active) {
    return; // lpn feature is currently inactive
  }

  result = gecko_cmd_hardware_set_soft_timer(0, // cancel friend finding timer
                                             TIMER_ID_FRIEND_FIND,
                                             1)->result;

  // Terminate friendship if exist
  result = gecko_cmd_mesh_lpn_terminate_friendship()->result;
  if (result) {
    LOG_INFO("Friendship termination failed (0x%x)", result);
  }
  // turn off lpn feature
  result = gecko_cmd_mesh_lpn_deinit()->result;
  if (result) {
    LOG_INFO("LPN deinit failed (0x%x)", result);
  }
  lpn_active = 0;
  LOG_INFO("LPN deinitialized");
}

/**
 * Sensor node initialization. This is called at each boot if provisioning is already done.
 * Otherwise this function is called after provisioning is completed.
 */
void sensor_node_init(void)
{
  mesh_lib_init(malloc, free, 8);

  lpn_init();
}

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


/**
 * This function publishes one on/off request to change the state of light(s)
 */
void send_button_state(uint8_t state)
{
	uint16 resp;
	struct mesh_generic_request req;

	req.kind = mesh_generic_request_on_off;
	req.on_off = state;

	// increment transaction ID for each request
		trid++;
		//mesh_lib_generic_client_publish
	resp = gecko_cmd_mesh_generic_client_publish(
			MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID,
			0, // element index of primary element is 0
			trid,
			0,   /* using zero transition time by default */
			0,
			0,     // flags
			mesh_generic_request_on_off,     // type
			1,     // param len
			&req.on_off     /// parameters data
	)->result;

	if (resp) {
		LOG_INFO("gecko_cmd_mesh_generic_client_publish failed,code %x\r\n", resp);
	} else {
		LOG_INFO("On/off request sent, trx id = %u", trid);
	}
}

/***************************************************************************//**
 * This function publishes one light CTL request to change the temperature level
 * of light(s) in the group. Global variable temperature_level holds the latest
 * desired light temperature level.
 * The CTL request also send lightness_level which holds the latest desired light
 * lightness level and Delta UV which is hardcoded to 0 for this application.
 *
 * param[in] retrans  Indicates if this is the first request or a retransmission,
 *                    possible values are 0 = first request, 1 = retransmission.
 ******************************************************************************/
void send_sensor_data(uint16_t lightness_level,
		uint16_t temperature_level,int16_t DELTA_UV,int retrans)
{
  uint16 resp1 = 1;
  uint16 delay;
  struct mesh_generic_request req;

  req.kind = mesh_lighting_request_lightness_actual;
//  req.kind = mesh_lighting_request_ctl;
  req.ctl.lightness = lightness_level;
//  req.ctl.temperature = temperature_level;
//  req.ctl.deltauv = DELTA_UV; //hardcoded delta uv

  // increment transaction ID for each request, unless it's a retransmission
  if (retrans == 0)
  {
    trid++;
  }

  delay = 0;

  resp1 = mesh_lib_generic_client_publish(
		  MESH_LIGHTING_LIGHTNESS_CLIENT_MODEL_ID,
    0,
    trid,
    &req,
    0,     // transition
    delay,
    0     // flags
    );

  if (resp1) {
    LOG_ERROR("gecko_cmd_mesh_generic_client_publish failed,code %x", resp1);
  } else {
    LOG_INFO("Sensor Data sent, trid = %u, delay = %d", trid, delay);
  }
}

void send_sensor_data_ctl(uint16_t lightness_level,
		uint16_t temperature_level,int16_t DELTA_UV,int retrans)
{
  uint16 resp1 = 1;
  uint16 delay;
  struct mesh_generic_request req;

//  req.kind = mesh_lighting_request_lightness_actual;
  req.kind = mesh_lighting_request_ctl;
  req.ctl.lightness = lightness_level;
  req.ctl.temperature = temperature_level;
  req.ctl.deltauv = DELTA_UV; //hardcoded delta uv

  // increment transaction ID for each request, unless it's a retransmission
  if (retrans == 0)
  {
    trid++;
  }

  delay = 0;

  resp1 = mesh_lib_generic_client_publish(
		  MESH_LIGHTING_CTL_CLIENT_MODEL_ID,
    0,
    trid,
    &req,
    0,     // transition
    delay,
    0     // flags
    );

  if (resp1) {
    LOG_ERROR("gecko_cmd_mesh_generic_client_publish failed,code %x", resp1);
  } else {
    LOG_INFO("Sensor Data sent, trid = %u, delay = %d", trid, delay);
  }
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

	LOG_INFO("register model %d",mesh_lib_generic_server_register_handler(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
			                                           0,
													   ctl_request,
													   ctl_change));

//	update_and_publish_on_off(0,1);
//	LOG_INFO("Update and publish 0x%x",ctl_update_and_publish(0,0)); // This errors now, no clue why

}

