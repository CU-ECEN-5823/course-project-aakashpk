/*
 * gecko_helper.h
 *
 *  Created on: Feb 20, 2019
 *      Author: aakash
 */

#ifndef SRC_GECKO_HELPER_H_
#define SRC_GECKO_HELPER_H_

/* Bluetooth stack headers */
#include <stdint.h>
#include <stdlib.h>
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


#define TIMER_ID_FACTORY_RESET  77
#define TIMER_ID_RESTART    78
#define TIMER_ID_PROVISIONING   66
#define TIMER_ID_FRIEND_FIND 20


#define BT_ADDRESS(server_addr)		server_addr.addr[5],server_addr.addr[4],\
									server_addr.addr[3],server_addr.addr[2],\
									server_addr.addr[1],server_addr.addr[0]



#endif /* SRC_GECKO_HELPER_H_ */
