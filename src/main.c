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
#include "letimer.h"
#include "Si7021_I2C.h"


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

#define PERIOD_MS (3000) // Change for Measurement period, in ms

int main(void)
{

  // Initialize stack
  gecko_main_init();
  buttonInit();

  gpioInit();

  displayInit();
  logInit();

  /*Initialize LETIMER to generate pulse at period interval
   * and for logging time stamps
   */
  LETIMER_pulse_setup(PERIOD_MS,sleepEM4);

//  LETIMER_stop_intr();

  //Init scheduler, Tasks registered later
  scheduler_init();
	#if DEVICE_IS_SENSOR_NODE


  /*
   * Initialize temp sensor,
   * using for now, will change to
   * project sensors once drivers are written
   */
  temp_sensor_init();


   //register LETIMER & I2C0 interrupt handlers
   LETIMER_register_UFISR(log_temp);
   // Init and Register button press ISRs
   GPIOINT_Init();
   GPIOINT_CallbackRegister(BUTTON0_PIN,button_press);

   //Register Sensor Tasks
  scheduler_event_register(log_temp_task, Task0);
  scheduler_event_register(Button_Press_Task,Task1);
	#else

  //Register Actuator Tasks
   scheduler_event_register(light_actuator_task, LIGHT_TASK);
   scheduler_event_register(pump_actuator_task,WATER_TASK);
   scheduler_event_register(light_setpoint_change_task,SETPOINT_CHANGE_TASK);
	#endif


  while (1) {
	struct gecko_cmd_packet *evt = gecko_wait_event();
	bool pass = mesh_bgapi_listener(evt);
	if (pass) {
		handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
	}
  };
}

