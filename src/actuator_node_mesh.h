/*
 * actuator_node_mesh.h
 *
 *  Created on: Apr 23, 2019
 *      Author: aakash
 */

#ifndef SRC_ACTUATOR_NODE_MESH_H_
#define SRC_ACTUATOR_NODE_MESH_H_

#include "gecko_helper.h"

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

#endif /* SRC_ACTUATOR_NODE_MESH_H_ */
