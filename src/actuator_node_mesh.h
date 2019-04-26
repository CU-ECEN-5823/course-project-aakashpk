/*
 * actuator_node_mesh.h
 *
 *  Created on: Apr 23, 2019
 *      Author: aakash
 */

#ifndef SRC_ACTUATOR_NODE_MESH_H_
#define SRC_ACTUATOR_NODE_MESH_H_

#include "gecko_helper.h"
#include "scheduler.h"
#include "output_ctrl.h"

#define LIGHT_TASK Task0
#define WATER_TASK Task1
#define SETPOINT_CHANGE_TASK Task2
#define DEADBAND_CHANGE_TASK Task3
#define CONFIG_CHANGE_TASK Task4

void actuator_node_init(void);


#endif /* SRC_ACTUATOR_NODE_MESH_H_ */
