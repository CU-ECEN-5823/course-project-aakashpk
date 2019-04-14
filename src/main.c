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
#include "button.h"
#include "scheduler.h"
#include "events.h"


extern void gecko_main_init();
extern void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);


// Maximum number of simultaneous Bluetooth connections
#define MAX_CONNECTIONS 2

// heap for Bluetooth stack
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


int main(void)
{

  // Initialize stack
  gecko_main_init();


  gpioInit();
  buttonInit();
  displayInit();
  logInit();

  // Init and Register button press ISRs
  GPIOINT_Init();
  GPIOINT_CallbackRegister(BUTTON0_PIN,button_press);

  //Init scheduler and Register Tasks
  scheduler_init();
  scheduler_event_register(Button_Press_Task,Task0);
  scheduler_event_register(Button_Release_Task,Task1);


  while (1) {
	struct gecko_cmd_packet *evt = gecko_wait_event();
	bool pass = mesh_bgapi_listener(evt);
	if (pass) {
		handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
	}
  };
}

