#include <stdbool.h>
#include "native_gecko.h"
#include "log.h"
#include "ble_mesh_device_type.h"

#include "mesh_sizes.h"
#include "gatt_db.h"
#include "gecko_ble_errors.h"
#include "mesh_generic_model_capi_types.h"
#include "gpiointerrupt.h"

#include "gpio.h"
#include "display.h"
#include "button.h"
#include "scheduler.h"
#include "events.h"


extern void gecko_main_init();
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);
extern void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);


bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);

// Maximum number of simultaneous Bluetooth connections
#define MAX_CONNECTIONS 2

// heap for Bluetooth stack
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

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


int main(void)
{

  // Initialize stack
  gecko_main_init();


  gpioInit();
  buttonInit();
  displayInit();

  logInit();

  // Register ISRs
  GPIOINT_Init();
  GPIOINT_CallbackRegister(BUTTON0_PIN,button_press);

  //Register Tasks
  scheduler_init();
  scheduler_event_register(Button_Press_Task,Task0);
  scheduler_event_register(Button_Release_Task,Task1);

  /* Infinite loop */
  while (1) {
	struct gecko_cmd_packet *evt = gecko_wait_event();
	bool pass = mesh_bgapi_listener(evt);
	if (pass) {
		handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
	}
  };
}


/*
 * All code below heavily borrows from
 * Si Labs BT mesh light and switch implementation
 */

// TODO: Move to different file, kept here till working proven

#define TIMER_ID_FACTORY_RESET  77
#define TIMER_ID_RESTART    78
#define TIMER_ID_PROVISIONING   66

/** global variables */
static uint16 _primary_elem_index = 0xffff; /* For indexing elements of the node */
static uint16 _secondary_elem_index = 0xffff; /* For indexing elements of the node */
static uint8 conn_handle = 0xFF;      /* handle of the last opened LE connection */

void initiate_factory_reset(void)
{
  LOG_INFO("factory reset\r\n");
  displayPrintf(DISPLAY_ROW_CONNECTION,"FACTORY RESET");

  /* if connection is open then close it before rebooting */
  if (conn_handle != 0xFF) {
    gecko_cmd_le_connection_close(conn_handle);
  }


  /* perform a factory reset by erasing PS storage. This removes all the keys and other settings
     that have been configured for this node */
  gecko_cmd_flash_ps_erase_all();
  // reboot after a small delay
  gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_FACTORY_RESET, 1);
}

void set_device_name(bd_addr *pAddr)
{
  char name[20];
  uint16 res;

  // create unique device name using the last two bytes of the Bluetooth address
  sprintf(name, "5823%s%x:%x",device_name, pAddr->addr[1], pAddr->addr[0]);

  LOG_INFO("Device name: %s", name);

  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(name), (uint8 *)name)->result;
  if (res) {
    LOG_INFO("gecko_cmd_gatt_server_write_attribute_value() failed, code %x", res);
  }

  // show device name on the LCD
  displayPrintf(DISPLAY_ROW_BTADDR,"%s",name);
	if(DeviceUsesServerModel())
		displayPrintf(DISPLAY_ROW_NAME,"Subscriber");
	else
		displayPrintf(DISPLAY_ROW_NAME,"Publisher");
}

/**
 * Handling of stack events. Both BLuetooth LE and Bluetooth mesh events are handled here.
 */
void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{

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
			BTSTACK_CHECK_RESPONSE(
					gecko_cmd_mesh_node_init());
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

		}

		break;

		case gecko_evt_mesh_node_initialized_id:
			printf("node initialized\r\n");

#if DEVICE_USES_BLE_MESH_SERVER_MODEL
				gecko_cmd_mesh_generic_server_init(); // server
				subscriber_node_init();
#else
				gecko_cmd_mesh_generic_client_init(); //client
				button_node_init();
#endif

			struct gecko_msg_mesh_node_initialized_evt_t *pData = (struct gecko_msg_mesh_node_initialized_evt_t *)&(evt->data);

			if (pData->provisioned) {
				LOG_INFO("node is provisioned. address:%x, ivi:%ld", pData->address, pData->ivi);

				displayPrintf(DISPLAY_ROW_PASSKEY,"provisioned");

			}
			else
			{
				LOG_INFO("node is unprovisioned");
				displayPrintf(DISPLAY_ROW_PASSKEY,"unprovisioned");

				LOG_INFO("starting unprovisioned beaconing...");

#if DEVICE_USES_BLE_MESH_SERVER_MODEL
				gecko_cmd_mesh_node_start_unprov_beaconing(0x3);   //server enable ADV and GATT provisioning bearer
#else
				gecko_cmd_mesh_node_start_unprov_beaconing(0x2);   // client enable GATT provisioning bearer
#endif
			}
			break;

		case gecko_evt_mesh_node_provisioning_started_id:
			LOG_INFO("Started provisioning");
			displayPrintf(DISPLAY_ROW_PASSKEY,"provisioning ..");
			// start timer for blinking LEDs to indicate which node is being provisioned
			gecko_cmd_hardware_set_soft_timer(32768 / 4, TIMER_ID_PROVISIONING, 0);
			break;

		case gecko_evt_mesh_node_provisioned_id:
			_primary_elem_index = 0;   // index of primary element is zero.
			_secondary_elem_index = 1; // index of secondary element is one.

#if DEVICE_USES_BLE_MESH_SERVER_MODEL
			subscriber_node_init();
#else
			button_node_init();
#endif

			LOG_INFO("node provisioned, got address=%x\r\n", evt->data.evt_mesh_node_provisioned.address);
			// stop LED blinking when provisioning complete
			gecko_cmd_hardware_set_soft_timer(0, TIMER_ID_PROVISIONING, 0);

			displayPrintf(DISPLAY_ROW_PASSKEY,"provisioned");
			break;

		case gecko_evt_mesh_node_provisioning_failed_id:
			prov_fail_evt = (struct gecko_msg_mesh_node_provisioning_failed_evt_t  *)&(evt->data);
			LOG_INFO("provisioning failed, code %x", prov_fail_evt->result);
			displayPrintf(DISPLAY_ROW_PASSKEY,"prov failed");
			/* start a one-shot timer that will trigger soft reset after small delay */
			gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_RESTART, 1);
			break;

		case gecko_evt_mesh_node_key_added_id:
			LOG_INFO("got new %s key with index %x", evt->data.evt_mesh_node_key_added.type == 0 ? "network" : "application",
					evt->data.evt_mesh_node_key_added.index);
			break;

		case gecko_evt_mesh_node_model_config_changed_id:
			LOG_INFO("model config changed");
			break;

		case gecko_evt_mesh_generic_server_client_request_id:
			LOG_DEBUG("evt gecko_evt_mesh_generic_server_client_request_id");
			mesh_lib_generic_server_event_handler(evt);
			break;

		case gecko_evt_mesh_generic_server_state_changed_id:
			LOG_INFO("evt gecko_evt_mesh_generic_server_client_request_id");
			// uncomment following line to get debug prints for each server state changed event
			//server_state_changed(&(evt->data.evt_mesh_generic_server_state_changed));

			// pass the server state changed event to mesh lib handler that will invoke
			// the callback functions registered by application
			mesh_lib_generic_server_event_handler(evt);
			break;

		case gecko_evt_mesh_node_reset_id:
			LOG_INFO("evt gecko_evt_mesh_node_reset_id");
			initiate_factory_reset();
			break;

		case gecko_evt_mesh_friend_friendship_established_id:
			LOG_INFO("evt gecko_evt_mesh_friend_friendship_established, lpn_address=%x\r\n", evt->data.evt_mesh_friend_friendship_established.lpn_address);
			displayPrintf(DISPLAY_ROW_ACTION,"FRIEND");
			break;

		case gecko_evt_mesh_friend_friendship_terminated_id:
			LOG_INFO("evt gecko_evt_mesh_friend_friendship_terminated, reason=%x\r\n", evt->data.evt_mesh_friend_friendship_terminated.reason);
			displayPrintf(DISPLAY_ROW_ACTION,"NO LPN");
			break;

		case gecko_evt_le_gap_adv_timeout_id:
			// adv timeout events silently discarded
			break;

		case gecko_evt_le_connection_opened_id:
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

		case gecko_evt_gatt_server_user_write_request_id:
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
			//printf("unhandled evt: %8.8x class %2.2x method %2.2x\r\n", evt_id, (evt_id >> 16) & 0xFF, (evt_id >> 24) & 0xFF);
			break;
	}

}

