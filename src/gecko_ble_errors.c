/*
 * ble_errors.c
 *
 *  Created on: Feb 13, 2019
 *      Author: Dan Walkes
 *
 * @brief A file to help with handling of bluetooth BLE errors from the Blue Gecko
 */

#include "gecko_ble_errors.h"


/**
 * Redefine the enumeration for use in the bleResponseFailureDescription function below
 */
#undef BG_ERROR_ENUM
#define BG_ERROR_ENUM(enumval,detail)\
		case enumval:\
			detailstr=detail;\
			break;

/**
 * @return a const char* to the failure description string for the error represented by
 * @param error.
 * For instance passing error enum bg_err_hardware_ps_store_full will
 * return "Flash reserved for PS store is full"
 */
const char *bleResponseFailureDescription(enum bg_error error)
{
	const char *detailstr = "Unknown";
	switch(error) {
		BG_ERROR_LIST
	}
	return detailstr;
}


/**
 * Redefine the enumeration for use in the bleResponseString function below
 */
#undef BG_ERROR_ENUM
#define BG_ERROR_ENUM(enumval,detail)\
		case enumval:\
			enumstr=#enumval;\
			break;

/**
 * @return the response string corresponding to the @param error bg_error enumeration.
 * For instance passing error bg_err_hardware_ps_store_full will return
 * "bg_err_hardware_ps_store_full"
 */
const char *bleResponseString(enum bg_error error)
{
	const char *enumstr = "Unknown";
	switch(error) {
		BG_ERROR_LIST
	}
	return enumstr;
}




