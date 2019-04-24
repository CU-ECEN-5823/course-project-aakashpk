/*
 * sensor_node_mesh.c
 *
 *  Created on: Apr 23, 2019
 *      Author: aakash
 */

#include "sensor_node_mesh.h"

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
 BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_lpn_init());

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
  BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_lpn_configure(2, 5 * 1000));


  LOG_INFO("trying to find friend...");
  BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_lpn_establish_friendship(0));

  lpn_active = 1;
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

  BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(0, // cancel friend finding timer
                                             TIMER_ID_FRIEND_FIND,
                                             1));

  // Terminate friendship if exist
  BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_lpn_terminate_friendship());

  LOG_INFO("Deinitialize LPN");
  // turn off lpn feature
  BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_lpn_deinit());

  lpn_active = 0;

}

client_on_off_response(uint16_t model_id,
					  uint16_t element_index,
					  uint16_t client_addr,
					  uint16_t server_addr,
					  const struct mesh_generic_state *current,
					  const struct mesh_generic_state *target,
					  uint32_t remaining_ms,
					  uint8_t response_flags)
{
	LOG_INFO("response flags 0x%x",response_flags);
}

/**
 * Sensor node initialization. This is called at each boot if provisioning is already done.
 * Otherwise this function is called after provisioning is completed.
 */
void sensor_node_init(void)
{
  MESH_CHECK_RESPONSE(mesh_lib_init(malloc, free, 8));

  MESH_CHECK_RESPONSE(mesh_lib_generic_client_register_handler(MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID,
                                           0,
                                           client_on_off_response));
  lpn_init();
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

	BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_generic_client_publish(
			MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID,
			0, // element index of primary element is 0
			trid,
			0,   /* using zero transition time by default */
			0,
			0,     // flags
			mesh_generic_request_on_off,     // type
			1,     // param len
			&req.on_off     /// parameters data
	));

}


void send_lighting_lightness(uint16_t level,
		uint8_t kind,
		uint8_t flag,
		uint16_t transition,
		uint16_t delay,
		int retrans)
{
  struct mesh_generic_request req;

//  mesh_lighting_request_lightness_actual;
  req.kind = kind;
  req.ctl.lightness = level;

  // increment transaction ID for each request, unless it's a retransmission
  if (retrans == 0)
  {
    trid++;
  }

  delay = 0;

  MESH_CHECK_RESPONSE(mesh_lib_generic_client_publish(
		  MESH_LIGHTING_LIGHTNESS_CLIENT_MODEL_ID,
    0,
    trid,
    &req,
	transition,     // transition
    delay,
	flag     // flags
    ));

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
