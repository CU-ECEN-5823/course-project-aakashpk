/*
 * actuator_node_mesh.h
 *
 *  Created on: Apr 23, 2019
 *      Author: aakash
 */

#ifndef SRC_ACTUATOR_NODE_MESH_H_
#define SRC_ACTUATOR_NODE_MESH_H_

#include "gecko_helper.h"

typedef enum {
	full,
	half,
	error,
	empty
}water_level_t;


typedef struct {
	uint16_t light_level;
	uint16_t light_setpoint;
	uint16_t deadband;
	uint8_t light_output;
	uint8_t light_state;
} light_data_t;


void actuator_node_init(void);




#endif /* SRC_ACTUATOR_NODE_MESH_H_ */
