/*
 * gecko_helper.c
 *
 *  Created on: Feb 20, 2019
 *      Author: aakash
 */

#include "gecko_helper.h"

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

  res = gecko_cmd_mesh_lpn_configure(2, 5 * 1000)->result;
  if (res) {
    LOG_INFO("LPN conf failed (0x%x)", res);
    return;
  }

  LOG_INFO("trying to find friend...");
  res = gecko_cmd_mesh_lpn_establish_friendship(0)->result;

  if (res != 0) {
    LOG_INFO("ret.code %x\r\n", res);
  }
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

	LOG_INFO("State %x",request->on_off);

	if(request->on_off == 0x01 ) displayPrintf(DISPLAY_ROW_TEMPVALUE,"Button Pressed");
	else displayPrintf(DISPLAY_ROW_TEMPVALUE,"Button Released");
}

void onoff_change(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
                         uint32_t remaining_ms)
{
	LOG_INFO("onoff_change called");
}

void actuator_node_init(void)
{
	mesh_lib_init(malloc, free,8);

	uint16_t res;
	//Initialize Friend functionality
	  LOG_INFO("Friend mode initialization ");

	  res = gecko_cmd_mesh_friend_init()->result;
	  if (res) {
	    LOG_INFO("Friend init failed 0x%x", res);
	  }

	LOG_INFO("register handler 1 %d",mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
	                                           0,
	                                           onoff_request,
	                                           onoff_change));

	struct mesh_generic_state current, target;
	errorcode_t e;

	current.kind = mesh_generic_state_on_off;
	current.on_off.on = MESH_GENERIC_ON_OFF_STATE_ON;

	target.kind = mesh_generic_state_on_off;
	target.on_off.on = MESH_GENERIC_ON_OFF_STATE_OFF;


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

}

/**
 * This function publishes one on/off request to change the state of light(s)
 */
void send_button_state(uint8_t state)
{
	static uint8_t trid;
	uint16 resp;
	struct mesh_generic_request req;

	req.kind = mesh_generic_request_on_off;
	req.on_off = state;

	// increment transaction ID for each request
		trid++;

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
		LOG_INFO("request sent, trx id = %u", trid);
	}
}
