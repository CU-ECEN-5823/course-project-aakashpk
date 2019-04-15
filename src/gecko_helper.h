/*
 * gecko_helper.h
 *
 *  Created on: Feb 20, 2019
 *      Author: aakash
 */

#ifndef SRC_GECKO_HELPER_H_
#define SRC_GECKO_HELPER_H_

/* Bluetooth stack headers */
#include "bg_types.h"
#include "gatt_db.h"
#include "native_gecko.h"
#include "mesh_generic_model_capi_types.h"
#include "mesh_lighting_model_capi_types.h"
#include "mesh_lib.h"

#include "gecko_ble_errors.h"
#include "log.h"
#include "display.h"
#include "gpio.h"

#define SET_GECKO_EVENT (1U)

#define SCHEDULER_SUPPORTS_DISPLAY_UPDATE_EVENT 1
#define TIMER_SUPPORTS_1HZ_TIMER_EVENT	1


#define BT_ADDRESS(server_addr)		server_addr.addr[5],server_addr.addr[4],\
									server_addr.addr[3],server_addr.addr[2],\
									server_addr.addr[1],server_addr.addr[0]


void send_button_state(uint8_t state);
void sensor_node_init(void);
void actuator_node_init(void);

void onoff_request(uint16_t model_id,
                          uint16_t element_index,
                          uint16_t client_addr,
                          uint16_t server_addr,
                          uint16_t appkey_index,
                          const struct mesh_generic_request *request,
                          uint32_t transition_ms,
                          uint16_t delay_ms,
                          uint8_t request_flags);

void onoff_change(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
                         uint32_t remaining_ms);

#endif /* SRC_GECKO_HELPER_H_ */
