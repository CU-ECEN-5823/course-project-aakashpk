# Current Implementation progress
All project update documents on [google drive](https://drive.google.com/drive/u/0/folders/1rQfYgGdwMprrHQvmEfe_ihg3yPjacLZz)

* Low Power and Friend implementation working, proven using energy profiler.
* Publish and Subscribe over lightness model working.Publish working over lighting lightness model. 
* This model has a request.kind field that can be used to differentiate between the sensors. 
* The lightness model gives a 16bit value that can be sent, this should be enough for both sensors
* Added a gatt characteristic for light setpoint, change in value gets raised as event. Logging the event for now
* writes setpoint values to flash when changed from GATT
* Tested Control functions for pump and light with light value not changing in case of unreliable data


### TODOs
* Write setpoint at startup to the gatt char, and update setpoint when it gets changed from gatt
* PWM signal write and can be shown on DSO.
* Config data to use either or both sensors, have to add seperate GATT char for that.
* If time permits, try getting the vendor model working. provisioning can be done using the embedded provisioner.


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
