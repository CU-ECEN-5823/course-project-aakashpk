/*
 * sensor_node_mesh.h
 *
 *  Created on: Apr 23, 2019
 *      Author: aakash
 */

#ifndef SRC_SENSOR_NODE_MESH_H_
#define SRC_SENSOR_NODE_MESH_H_

#include "gecko_helper.h"



void send_button_state(uint8_t state);
void sensor_node_init(void);

void send_lighting_lightness(uint16_t level,
		uint8_t kind,
		uint8_t flag,
		uint16_t transition,
		uint16_t delay,
		int retrans);

#define SEND_LIGHT_DATA(level,flag) send_lighting_lightness(level,\
									mesh_lighting_request_lightness_actual,\
									flag,0,0,0)

#define SEND_WATER_DATA(level,flag) send_lighting_lightness(level,\
									mesh_lighting_request_lightness_linear,\
									flag,0,0,0)

void send_sensor_data_ctl(uint16_t lightness_level,
		uint16_t temperature_level,int16_t DELTA_UV,int retrans);

void lpn_init(void);
void lpn_deinit(void);

#endif /* SRC_SENSOR_NODE_MESH_H_ */
