/***************************************************************************//**
 * @file
 * @brief Silicon Labs BT Mesh Empty Example Project
 * This example demonstrates the bare minimum needed for a Blue Gecko BT Mesh C application.
 * The application starts unprovisioned Beaconing after boot
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include <gecko_configuration.h>
#include <mesh_sizes.h>

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include <em_gpio.h>

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif
#include "ble_mesh_device_type.h"

#include "log.h"
#include "display.h"
#include "scheduler.h"
#include "gecko_ble_errors.h"
#include "gecko_helper.h"
#include "sensor_node_mesh.h"
#include "actuator_node_mesh.h"

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

// bluetooth stack heap
#define MAX_CONNECTIONS 2

uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

// Bluetooth advertisement set configuration
//
// At minimum the following is required:
// * One advertisement set for Bluetooth LE stack (handle number 0)
// * One advertisement set for Mesh data (handle number 1)
// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
// * One advertisement set for Mesh unprovisioned URI (handle number 3)
// * N advertisement sets for Mesh GATT service advertisements
// (one for each network key, handle numbers 4 .. N+3)
//
#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)

static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;

// bluetooth stack configuration
extern const struct bg_gattdb_def bg_gattdb_data;

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

const gecko_configuration_t config =
{
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.max_advertisers = MAX_ADVERTISERS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
  .bluetooth.sleep_clock_accuracy = 100,
  .bluetooth.linklayer_priorities = &linklayer_priorities,
  .gattdb = &bg_gattdb_data,
  .btmesh_heap_size = BTMESH_HEAP_SIZE,
#if (HAL_PA_ENABLE)
  .pa.config_enable = 1, // Set this to be a valid PA config
#if defined(FEATURE_PA_INPUT_FROM_VBAT)
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#else
  .pa.input = GECKO_RADIO_PA_INPUT_DCDC,
#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
#endif // (HAL_PA_ENABLE)
  .max_timers = 16,
};
//
//void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);
//void mesh_native_bgapi_init(void);
//bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);

// definition
void gatt_char_change_server(struct gecko_cmd_packet *evt);


/**
 * See light switch app.c file definition
 */
void gecko_bgapi_classes_init_server_friend(void)
{
	  gecko_bgapi_class_dfu_init();
	  gecko_bgapi_class_system_init();
	  gecko_bgapi_class_le_gap_init();
	  gecko_bgapi_class_le_connection_init();
	  //gecko_bgapi_class_gatt_init();
	  gecko_bgapi_class_gatt_server_init();
	  gecko_bgapi_class_hardware_init();
	  gecko_bgapi_class_flash_init();
	  gecko_bgapi_class_test_init();
	  //gecko_bgapi_class_sm_init();
	  //mesh_native_bgapi_init();
	  gecko_bgapi_class_mesh_node_init();
	  //gecko_bgapi_class_mesh_prov_init();
	  gecko_bgapi_class_mesh_proxy_init();
	  gecko_bgapi_class_mesh_proxy_server_init();
	  //gecko_bgapi_class_mesh_proxy_client_init();
	  //gecko_bgapi_class_mesh_generic_client_init();
	  gecko_bgapi_class_mesh_generic_server_init();
	  //gecko_bgapi_class_mesh_vendor_model_init();
	  //gecko_bgapi_class_mesh_health_client_init();
	  //gecko_bgapi_class_mesh_health_server_init();
	  //gecko_bgapi_class_mesh_test_init();
	  //gecko_bgapi_class_mesh_lpn_init();
	  gecko_bgapi_class_mesh_friend_init();
}


/**
 * See main function list in soc-btmesh-switch project file
 */
void gecko_bgapi_classes_init_client_lpn(void)
{
	gecko_bgapi_class_dfu_init();
	gecko_bgapi_class_system_init();
	gecko_bgapi_class_le_gap_init();
	gecko_bgapi_class_le_connection_init();
	//gecko_bgapi_class_gatt_init();
	gecko_bgapi_class_gatt_server_init();
	gecko_bgapi_class_hardware_init();
	gecko_bgapi_class_flash_init();
	gecko_bgapi_class_test_init();
	//gecko_bgapi_class_sm_init();
	//mesh_native_bgapi_init();
	gecko_bgapi_class_mesh_node_init();
	//gecko_bgapi_class_mesh_prov_init();
	gecko_bgapi_class_mesh_proxy_init();
	gecko_bgapi_class_mesh_proxy_server_init();
	//gecko_bgapi_class_mesh_proxy_client_init();
	gecko_bgapi_class_mesh_generic_client_init();
	//gecko_bgapi_class_mesh_generic_server_init();
	//gecko_bgapi_class_mesh_vendor_model_init();
	//gecko_bgapi_class_mesh_health_client_init();
	//gecko_bgapi_class_mesh_health_server_init();
	//gecko_bgapi_class_mesh_test_init();
	gecko_bgapi_class_mesh_lpn_init();
	//gecko_bgapi_class_mesh_friend_init();

}
void gecko_main_init()
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  // Minimize advertisement latency by allowing the advertiser to always
  // interrupt the scanner.
  linklayer_priorities.scan_max = linklayer_priorities.adv_min + 1;

  gecko_stack_init(&config);

  if( DeviceUsesClientModel() ) {
	  gecko_bgapi_classes_init_client_lpn();
  } else {
	  gecko_bgapi_classes_init_server_friend();
  }

  // Initialize coexistence interface. Parameters are taken from HAL config.
  gecko_initCoexHAL();

}

/*
 * All code below heavily borrows from
 * Si Labs BT mesh light and switch implementation
 */


/** Timer Frequency used. */
#define TIMER_CLK_FREQ ((uint32)32768)
/** Convert msec to timer ticks. */
#define TIMER_MS_2_TIMERTICK(ms) ((TIMER_CLK_FREQ * ms) / 1000)

/** global variables */
static uint16 _primary_elem_index = 0xffff; /* For indexing elements of the node */
static uint16 _secondary_elem_index = 0xffff; /* For indexing elements of the node */
static uint8 conn_handle = 0xFF;      /* handle of the last opened LE connection */

void initiate_factory_reset(void)
{
  LOG_INFO("factory reset");
  displayPrintf(DISPLAY_ROW_CONNECTION,"FACTORY RESET");

  /* if connection is open then close it before rebooting */
  if (conn_handle != 0xFF) {
    gecko_cmd_le_connection_close(conn_handle);
  }


  /* perform a factory reset by erasing PS storage. This removes all the keys and other settings
     that have been configured for this node */
  BTSTACK_CHECK_RESPONSE(gecko_cmd_flash_ps_erase_all());

  // reboot after a small delay
  gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_FACTORY_RESET, 1);
}

void set_device_name(bd_addr *pAddr)
{
  char name[20];

  // create unique device name using the last two bytes of the Bluetooth address
  snprintf(name,20,"5823%s%x:%x",device_name, pAddr->addr[1], pAddr->addr[0]);

  LOG_INFO("Device name: %s", name);

  BTSTACK_CHECK_RESPONSE(
		  gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name,
		  0, strlen(name), (uint8 *)name));

  // show device name on the LCD
  displayPrintf(DISPLAY_ROW_BTADDR,"%s",name);
	if(DeviceUsesServerModel())
		displayPrintf(DISPLAY_ROW_NAME,"Actuator Node");
	else
		displayPrintf(DISPLAY_ROW_NAME,"Sensor Node");
}

/**
 * Handling of stack events. Both BLuetooth LE and Bluetooth mesh events are handled here.
 */
void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
	uint16_t result = 0;

	struct gecko_msg_mesh_node_provisioning_failed_evt_t  *prov_fail_evt;

	if (NULL == evt) {
		return;
	}

	switch (evt_id) {
	case gecko_evt_system_boot_id:
		// check pushbutton state at startup. If either PB0 or PB1 is held down then do factory reset
		if (GPIO_PinInGet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN) == 0 || GPIO_PinInGet(BSP_BUTTON1_PORT, BSP_BUTTON1_PIN) == 0) {
			initiate_factory_reset();
		} else {
			struct gecko_msg_system_get_bt_address_rsp_t *pAddr = gecko_cmd_system_get_bt_address();

			set_device_name(&pAddr->address);

			// Initialize Mesh stack in Node operation mode, wait for initialized event
			LOG_INFO("Node init %x",gecko_cmd_mesh_node_init()->result);

		}
		break;

	case gecko_evt_hardware_soft_timer_id:
		switch (evt->data.evt_hardware_soft_timer.handle) {
		case TIMER_ID_FACTORY_RESET:
			gecko_cmd_system_reset(0);
			break;

		case TIMER_ID_RESTART:
			gecko_cmd_system_reset(0);
			break;

        case TIMER_ID_FRIEND_FIND:
        {
          LOG_DEBUG("trying to find friend...");
          uint16_t result = gecko_cmd_mesh_lpn_establish_friendship(0)->result;
          displayPrintf(DISPLAY_ROW_ACTION,"FRIEND SRCH");

          if (result != 0) {
            LOG_INFO("ret.code %x", result);
          }
        }
        break;

		}

		break;

		case gecko_evt_mesh_node_initialized_id:
			LOG_INFO("node initialized");

			#if DEVICE_USES_BLE_MESH_SERVER_MODEL
						gecko_cmd_mesh_generic_server_init(); // server

			#else
						gecko_cmd_mesh_generic_client_init(); //client
			#endif

			struct gecko_msg_mesh_node_initialized_evt_t *pData = (struct gecko_msg_mesh_node_initialized_evt_t *)&(evt->data);

			if (pData->provisioned) {
				LOG_INFO("node is provisioned. address:0x%x, ivi:%ld", pData->address, pData->ivi);
				displayPrintf(DISPLAY_ROW_PASSKEY,"provisioned");

				#if DEVICE_USES_BLE_MESH_SERVER_MODEL
					actuator_node_init();
				#else
					sensor_node_init();
				#endif
			}
			else
			{
				LOG_INFO("node is unprovisioned");
				displayPrintf(DISPLAY_ROW_PASSKEY,"unprovisioned");

				LOG_INFO("starting unprovisioned beaconing...");

				#if DEVICE_USES_BLE_MESH_SERVER_MODEL
								gecko_cmd_mesh_node_start_unprov_beaconing(0x3);   //server enable ADV and GATT provisioning bearer
				#else
								gecko_cmd_mesh_node_start_unprov_beaconing(0x3);   // client enable GATT provisioning bearer
				#endif
			}
			break;

		case gecko_evt_mesh_node_provisioning_started_id:
			LOG_INFO("Started provisioning");
			displayPrintf(DISPLAY_ROW_PASSKEY,"provisioning ..");
			break;

		case gecko_evt_mesh_node_provisioned_id:
			_primary_elem_index = 0;   // index of primary element is zero.
			_secondary_elem_index = 1; // index of secondary element is one.

			#if DEVICE_USES_BLE_MESH_SERVER_MODEL
						actuator_node_init();
			#else
						sensor_node_init();
			#endif

			LOG_INFO("node provisioned, got address=0x%x", evt->data.evt_mesh_node_provisioned.address);
			displayPrintf(DISPLAY_ROW_PASSKEY,"provisioned");
			break;

		case gecko_evt_mesh_node_provisioning_failed_id:
			prov_fail_evt = (struct gecko_msg_mesh_node_provisioning_failed_evt_t  *)&(evt->data);
			LOG_INFO("provisioning failed, code 0x%x", prov_fail_evt->result);
			displayPrintf(DISPLAY_ROW_PASSKEY,"prov failed");
			/* start a one-shot timer that will trigger soft reset after small delay */
			gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_RESTART, 1);
			break;

		case gecko_evt_mesh_node_key_added_id:
			LOG_INFO("got new %s key with index 0x%x",
					evt->data.evt_mesh_node_key_added.type == 0 ? "network" : "application",
					evt->data.evt_mesh_node_key_added.index);
			break;

		case gecko_evt_mesh_node_model_config_changed_id:
			LOG_INFO("model config changed id %x state %x addr %x",
			    		  evt->data.evt_mesh_node_model_config_changed.model_id,
						  evt->data.evt_mesh_node_model_config_changed.mesh_node_config_state,
						  evt->data.evt_mesh_node_model_config_changed.element_address);
			break;

		case gecko_evt_mesh_generic_server_client_request_id:
			LOG_DEBUG("evt gecko_evt_mesh_generic_server_client_request_id");
			mesh_lib_generic_server_event_handler(evt);
			break;

		case gecko_evt_mesh_generic_client_server_status_id:
			LOG_INFO("gecko_evt_mesh_generic_client_server_status_id");
			mesh_lib_generic_client_event_handler(evt);
			break;

		case gecko_evt_mesh_generic_server_state_changed_id:
			LOG_INFO("evt gecko_evt_mesh_generic_server_client_request_id");

			LOG_INFO("State changed Elem index %d Model id: 0x%x",
					evt->data.evt_mesh_generic_server_state_changed.elem_index,
					evt->data.evt_mesh_generic_server_state_changed.model_id);

			// pass the server state changed event to mesh lib handler that will invoke
			// the callback functions registered by application
			mesh_lib_generic_server_event_handler(evt);
			break;

		case gecko_evt_mesh_node_reset_id:
			LOG_INFO("evt gecko_evt_mesh_node_reset_id");
			initiate_factory_reset();
			break;

		case gecko_evt_mesh_friend_friendship_established_id:
			LOG_DEBUG("evt gecko_evt_mesh_friend_friendship_established");
			LOG_INFO("Friend estd: lpn_address=%x",
					evt->data.evt_mesh_friend_friendship_established.lpn_address);
			displayPrintf(DISPLAY_ROW_ACTION,"FRIEND");
			break;

		case gecko_evt_mesh_friend_friendship_terminated_id:
			LOG_INFO("evt gecko_evt_mesh_friend_friendship_terminated, reason=%x\r\n", evt->data.evt_mesh_friend_friendship_terminated.reason);
			displayPrintf(DISPLAY_ROW_ACTION,"NO LPN");
			break;

	    case gecko_evt_mesh_lpn_friendship_established_id:
	      LOG_INFO("friendship established");
	      displayPrintf(DISPLAY_ROW_ACTION,"LPN with FRIEND");

	      break;

	    case gecko_evt_mesh_lpn_friendship_failed_id:
	      LOG_DEBUG("friendship failed 0x%x",evt->data.evt_mesh_lpn_friendship_failed.reason);
	      displayPrintf(DISPLAY_ROW_ACTION,"NO FRIEND");

	      // try again in 2 seconds
	      BTSTACK_CHECK_RESPONSE(
	    		  gecko_cmd_hardware_set_soft_timer(
	    				  TIMER_MS_2_TIMERTICK(5000),
	    				  TIMER_ID_FRIEND_FIND, 1));
	      break;

	    case gecko_evt_mesh_lpn_friendship_terminated_id:
	      LOG_INFO("friendship terminated 0x%x",evt->data.evt_mesh_friend_friendship_terminated.reason);
	      displayPrintf(DISPLAY_ROW_ACTION,"FRIEND LOST");

	      // try again in 2 seconds
	        BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer
	        		(TIMER_MS_2_TIMERTICK(2000),
	        				TIMER_ID_FRIEND_FIND, 1));
	        break;

		case gecko_evt_le_gap_adv_timeout_id:
			// adv timeout events silently discarded
			break;

		case gecko_evt_le_connection_opened_id:
			lpn_deinit();
			LOG_INFO("evt:gecko_evt_le_connection_opened_id");
			conn_handle = evt->data.evt_le_connection_opened.connection;
			displayPrintf(DISPLAY_ROW_CONNECTION,"connected");
			break;

		case gecko_evt_le_connection_parameters_id:
			LOG_INFO("evt:gecko_evt_le_connection_parameters_id");
			break;

		case gecko_evt_le_connection_closed_id:
			/* Check if need to boot to dfu mode */
			if (boot_to_dfu) {
				/* Enter to DFU OTA mode */
				gecko_cmd_system_reset(2);
			}

			LOG_INFO("evt:conn closed, reason 0x%x", evt->data.evt_le_connection_closed.reason);
			conn_handle = 0xFF;
			break;

		case gecko_evt_gatt_server_attribute_value_id:
			LOG_DEBUG("gecko_evt_gatt_server_attribute_value_id opcode 0x%x attribute 0x%4x length %d data 0x%02x%02x",
					evt->data.evt_gatt_server_attribute_value.att_opcode,
					evt->data.evt_gatt_server_attribute_value.attribute,
					evt->data.evt_gatt_server_attribute_value.value.len,
					evt->data.evt_gatt_server_attribute_value.value.data[0],
					evt->data.evt_gatt_server_attribute_value.value.data[1]);
			#if DEVICE_USES_BLE_MESH_SERVER_MODEL
					gatt_char_change_server(evt);
			#else
//					gatt_char_change_client(evt);
			#endif

			break;

		case gecko_evt_gatt_server_user_write_request_id:
			LOG_INFO("gecko_evt_gatt_server_user_write_request_id");
			if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
				/* Set flag to enter to OTA mode */
				boot_to_dfu = 1;
				/* Send response to Write Request */
				gecko_cmd_gatt_server_send_user_write_response(
						evt->data.evt_gatt_server_user_write_request.connection,
						gattdb_ota_control,
						bg_err_success);

				/* Close connection to enter to DFU OTA mode */
				gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
			}
			break;

		case gecko_evt_system_external_signal_id:
			LOG_DEBUG("gecko_evt_system_external_signal_id");
			scheduler_run();
			logFlush();
			break;

		default:
			LOG_INFO("unhandled evt: %8.8x class %2.2x method %2.2x", evt_id, (evt_id >> 16) & 0xFF, (evt_id >> 24) & 0xFF);
			break;
	}

}


void gatt_char_change_server(struct gecko_cmd_packet *evt)
{
	switch(evt->data.evt_gatt_server_attribute_value.attribute)
	{
	case gattdb_light_setpoint:

		switch(evt->data.evt_gatt_server_attribute_value.value.len)
		{
		case 1:
			set_changed_light_setpoint(evt->data.evt_gatt_server_attribute_value.value.data[0]);
			break;
		case 2:
			set_changed_light_setpoint((uint16_t)evt->data.evt_gatt_server_attribute_value.value.data[0]<<8
					|evt->data.evt_gatt_server_attribute_value.value.data[1]);
			break;
		}
		schedule_event(SETPOINT_CHANGE_TASK);
		gecko_external_signal(SET_GECKO_EVENT);
		break;

	case gattdb_deadband:
		set_changed_light_deadband(evt->data.evt_gatt_server_attribute_value.value.data[0]);
		schedule_event(DEADBAND_CHANGE_TASK);
		gecko_external_signal(SET_GECKO_EVENT);
		break;

	case gattdb_conn_dev:
		set_changed_config(evt->data.evt_gatt_server_attribute_value.value.data[0]);
		schedule_event(CONFIG_CHANGE_TASK);
		gecko_external_signal(SET_GECKO_EVENT);
		break;
	}

}
