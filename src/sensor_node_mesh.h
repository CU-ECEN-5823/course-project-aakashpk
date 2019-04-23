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

void send_sensor_data(uint16_t lightness_level,
					  uint16_t temperature_level,
					  int16_t DELTA_UV,
					  int retrans);

void send_sensor_data_ctl(uint16_t lightness_level,
		uint16_t temperature_level,int16_t DELTA_UV,int retrans);

void lpn_init(void);
void lpn_deinit(void);

#endif /* SRC_SENSOR_NODE_MESH_H_ */
