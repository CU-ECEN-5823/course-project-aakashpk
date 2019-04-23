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
