# Current Implementation progress
All project documents on [google drive](https://drive.google.com/drive/u/0/folders/1rQfYgGdwMprrHQvmEfe_ihg3yPjacLZz)

* Establish friendship with a low power node
* Receive data on lighting lightness and generic on off model over mesh
* Differentiate between the 2 different sensor data using the request kind attribute
* Actuate 2 different actuators based on the read sensor data
* Exposed a setpoint and deadband parameter over GATT
* Register changes to the parameter from the Blue Gecko App and change the setpoint and deadband accordingly
* The changes are written to flash, hence saved over power cycle
* A change in actuator output is also written to flash so that the last value is held at startup.
* A config parameter is also exposed to GATT, this can be changed to select the number of actuators connected to the node.
* Override the state of pump based on button press from the sensor node.
* Integrated the LCD so that the actuator values and mesh state is shown on the screen
* Logging over serial for debugging functionality.


## ECEN 5823 Bluetooth Mesh Skeleton Project

This project contains skeleton code used for coursework in University of Colorado [ECEN 5823 IoT Embedded Firmware](https://sites.google.com/colorado.edu/ecen5823/home).

Below is an overview of the sequence used to generate this repository:
* The project was generated starting with the new project Wizard built into [Simplicity Studio 4](https://www.silabs.com/products/development-tools/software/simplicity-studio).  
* The AppBuilder project was used with application type "Bluetooth Mesh SDK" with version 2.11.2.0 and Bluetooth SDK 2.9.3.0, application stack "Bluetooth Mesh SDK 1.4.1.0"
* The SOC- BT Mesh Empty project application was used.
* Board and part were configured for BRD4104A Rev 00 and EFR32BG13P632F512GM48 respectively
* Configurations were setup to target GNU ARM 7.2.1.
* Simplicity project Workspace paths were setup to pull in emlib functions needed for display support, including middleware/glib directory and glib/glib glib/dmd directories
* Relevant emlib project files were copied from SiliconLabs\SimplicityStudio\v4\developer\sdks\gecko_sdk_suite\v2.5\platform as needed and added into the respective directories at the root.
* The main.c file in the root folder was renamed [gecko_main.c](gecko_main.c).  Contents of the main while loop were moved into functions and the main() function was #ifdef'd out.
* The [src](src) subfolder was added to contain code specific to the ECEN 5823 course and source files were added to support ECEN 5823 and the simplicity studio exercise assignment.
