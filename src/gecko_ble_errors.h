/*
 * gecko_ble_errors.h
 *
 *  Created on: Feb 13, 2019
 *      Author: Dan Walkes
 *
 * Macros to assist with logging of Blue Gecko BLE errors
 */

#ifndef SRC_GECKO_BLE_ERRORS_H_
#define SRC_GECKO_BLE_ERRORS_H_

#include <native_gecko.h>
#include "log.h"
/**
 * Generate the list below by copying and pasting the definition of enum bg_error from bg_errorcodes.h into a file, then using this sed command:
 * cat ERRORS.txt | sed 's/[^a-z]*\([a-z0-9_]*\).*\/\/\(.*\)/BG_ERROR_ENUM(\1,\"\2\")\\/g'
 * Then quote any quotes within strings, and add BG_ERROR_ENUM(bg_err_last,"last_entry") to avoid errors on case statements
 */
#define BG_ERROR_LIST \
BG_ERROR_ENUM(bg_err_hardware_ps_store_full,"Flash reserved for PS store is full")\
BG_ERROR_ENUM(bg_err_hardware_ps_key_not_found,"PS key not found")\
BG_ERROR_ENUM(bg_err_hardware_i2c_ack_missing,"Acknowledge for i2c was not received.")\
BG_ERROR_ENUM(bg_err_hardware_i2c_timeout,"I2C read or write timed out.")\
BG_ERROR_ENUM(bg_err_invalid_conn_handle,"Invalid GATT connection handle.")\
BG_ERROR_ENUM(bg_err_waiting_response,"Waiting response from GATT server to previous procedure.")\
BG_ERROR_ENUM(bg_err_gatt_connection_timeout,"GATT connection is closed due procedure timeout.")\
BG_ERROR_ENUM(bg_err_invalid_param,"Command contained invalid parameter")\
BG_ERROR_ENUM(bg_err_wrong_state,"Device is in wrong state to receive command")\
BG_ERROR_ENUM(bg_err_out_of_memory,"Device has run out of memory")\
BG_ERROR_ENUM(bg_err_not_implemented,"Feature is not implemented")\
BG_ERROR_ENUM(bg_err_invalid_command,"Command was not recognized")\
BG_ERROR_ENUM(bg_err_timeout,"A command or procedure failed or a link lost due to timeout")\
BG_ERROR_ENUM(bg_err_not_connected,"Connection handle passed is to command is not a valid handle")\
BG_ERROR_ENUM(bg_err_flow,"Command would cause either underflow or overflow error")\
BG_ERROR_ENUM(bg_err_user_attribute,"User attribute was accessed through API which is not supported")\
BG_ERROR_ENUM(bg_err_invalid_license_key,"No valid license key found")\
BG_ERROR_ENUM(bg_err_command_too_long,"Command maximum length exceeded")\
BG_ERROR_ENUM(bg_err_out_of_bonds,"Bonding procedure can't be started because device has no space left for bond.")\
BG_ERROR_ENUM(bg_err_unspecified,"Unspecified error")\
BG_ERROR_ENUM(bg_err_hardware,"Hardware failure")\
BG_ERROR_ENUM(bg_err_buffers_full,"Command not accepted, because internal buffers are full")\
BG_ERROR_ENUM(bg_err_disconnected,"Command or Procedure failed due to disconnection")\
BG_ERROR_ENUM(bg_err_too_many_requests,"Too many Simultaneous Requests")\
BG_ERROR_ENUM(bg_err_not_supported,"Feature is not supported in this firmware build")\
BG_ERROR_ENUM(bg_err_no_bonding,"The bonding does not exist.")\
BG_ERROR_ENUM(bg_err_crypto,"Error using crypto functions")\
BG_ERROR_ENUM(bg_err_data_corrupted,"Data was corrupted.")\
BG_ERROR_ENUM(bg_err_command_incomplete,"Data received does not form a complete command")\
BG_ERROR_ENUM(bg_err_not_initialized,"Feature or subsystem not initialized")\
BG_ERROR_ENUM(bg_err_invalid_sync_handle,"Invalid periodic advertising sync handle")\
BG_ERROR_ENUM(bg_err_smp_passkey_entry_failed,"The user input of passkey failed, for example, the user cancelled the operation")\
BG_ERROR_ENUM(bg_err_smp_oob_not_available,"Out of Band data is not available for authentication")\
BG_ERROR_ENUM(bg_err_smp_authentication_requirements,"The pairing procedure cannot be performed as authentication requirements cannot be met due to IO capabilities of one or both devices")\
BG_ERROR_ENUM(bg_err_smp_confirm_value_failed,"The confirm value does not match the calculated compare value")\
BG_ERROR_ENUM(bg_err_smp_pairing_not_supported,"Pairing is not supported by the device")\
BG_ERROR_ENUM(bg_err_smp_encryption_key_size,"The resultant encryption key size is insufficient for the security requirements of this device")\
BG_ERROR_ENUM(bg_err_smp_command_not_supported,"The SMP command received is not supported on this device")\
BG_ERROR_ENUM(bg_err_smp_unspecified_reason,"Pairing failed due to an unspecified reason")\
BG_ERROR_ENUM(bg_err_smp_repeated_attempts,"Pairing or authentication procedure is disallowed because too little time has elapsed since last pairing request or security request")\
BG_ERROR_ENUM(bg_err_smp_invalid_parameters,"The Invalid Parameters error code indicates: the command length is invalid or a parameter is outside of the specified range.")\
BG_ERROR_ENUM(bg_err_smp_dhkey_check_failed,"Indicates to the remote device that the DHKey Check value received doesn't match the one calculated by the local device.")\
BG_ERROR_ENUM(bg_err_smp_numeric_comparison_failed,"Indicates that the confirm values in the numeric comparison protocol do not match.")\
BG_ERROR_ENUM(bg_err_smp_bredr_pairing_in_progress,"Indicates that the pairing over the LE transport failed due to a Pairing Request sent over the BR/EDR transport in process.")\
BG_ERROR_ENUM(bg_err_smp_cross_transport_key_derivation_generation_not_allowed,"Indicates that the BR/EDR Link Key generated on the BR/EDR transport cannot be used to derive and distribute keys for the LE transport.")\
BG_ERROR_ENUM(bg_err_bt_error_success,"Command completed succesfully")\
BG_ERROR_ENUM(bg_err_bt_unknown_connection_identifier,"Connection does not exist, or connection open request was cancelled.")\
BG_ERROR_ENUM(bg_err_bt_authentication_failure,"Pairing or authentication failed due to incorrect results in the pairing or authentication procedure. This could be due to an incorrect PIN or Link Key")\
BG_ERROR_ENUM(bg_err_bt_pin_or_key_missing,"Pairing failed because of missing PIN, or authentication failed because of missing Key")\
BG_ERROR_ENUM(bg_err_bt_memory_capacity_exceeded,"Controller is out of memory.")\
BG_ERROR_ENUM(bg_err_bt_connection_timeout,"Link supervision timeout has expired.")\
BG_ERROR_ENUM(bg_err_bt_connection_limit_exceeded,"Controller is at limit of connections it can support.")\
BG_ERROR_ENUM(bg_err_bt_synchronous_connectiontion_limit_exceeded,"The Synchronous Connection Limit to a Device Exceeded error code indicates that the Controller has reached the limit to the number of synchronous connections that can be achieved to a device. ")\
BG_ERROR_ENUM(bg_err_bt_acl_connection_already_exists,"The ACL Connection Already Exists error code indicates that an attempt to create a new ACL Connection to a device when there is already a connection to this device.")\
BG_ERROR_ENUM(bg_err_bt_command_disallowed,"Command requested cannot be executed because the Controller is in a state where it cannot process this command at this time.")\
BG_ERROR_ENUM(bg_err_bt_connection_rejected_due_to_limited_resources,"The Connection Rejected Due To Limited Resources error code indicates that an incoming connection was rejected due to limited resources.")\
BG_ERROR_ENUM(bg_err_bt_connection_rejected_due_to_security_reasons,"The Connection Rejected Due To Security Reasons error code indicates that a connection was rejected due to security requirements not being fulfilled, like authentication or pairing.")\
BG_ERROR_ENUM(bg_err_bt_connection_rejected_due_to_unacceptable_bd_addr,"The Connection was rejected because this device does not accept the BD_ADDR. This may be because the device will only accept connections from specific BD_ADDRs.")\
BG_ERROR_ENUM(bg_err_bt_connection_accept_timeout_exceeded,"The Connection Accept Timeout has been exceeded for this connection attempt.")\
BG_ERROR_ENUM(bg_err_bt_unsupported_feature_or_parameter_value,"A feature or parameter value in the HCI command is not supported.")\
BG_ERROR_ENUM(bg_err_bt_invalid_command_parameters,"Command contained invalid parameters.")\
BG_ERROR_ENUM(bg_err_bt_remote_user_terminated,"User on the remote device terminated the connection.")\
BG_ERROR_ENUM(bg_err_bt_remote_device_terminated_connection_due_to_low_resources,"The remote device terminated the connection because of low resources")\
BG_ERROR_ENUM(bg_err_bt_remote_powering_off,"Remote Device Terminated Connection due to Power Off")\
BG_ERROR_ENUM(bg_err_bt_connection_terminated_by_local_host,"Local device terminated the connection.")\
BG_ERROR_ENUM(bg_err_bt_repeated_attempts,"The Controller is disallowing an authentication or pairing procedure because too little time has elapsed since the last authentication or pairing attempt failed.")\
BG_ERROR_ENUM(bg_err_bt_pairing_not_allowed,"The device does not allow pairing. This can be for example, when a device only allows pairing during a certain time window after some user input allows pairing")\
BG_ERROR_ENUM(bg_err_bt_unsupported_remote_feature,"The remote device does not support the feature associated with the issued command.")\
BG_ERROR_ENUM(bg_err_bt_unspecified_error,"No other error code specified is appropriate to use.")\
BG_ERROR_ENUM(bg_err_bt_ll_response_timeout,"Connection terminated due to link-layer procedure timeout.")\
BG_ERROR_ENUM(bg_err_bt_ll_procedure_collision,"LL procedure has collided with the same transaction or procedure that is already in progress.")\
BG_ERROR_ENUM(bg_err_bt_encryption_mode_not_acceptable,"The requested encryption mode is not acceptable at this time.")\
BG_ERROR_ENUM(bg_err_bt_link_key_cannot_be_changed,"Link key cannot be changed because a fixed unit key is being used.")\
BG_ERROR_ENUM(bg_err_bt_instant_passed,"LMP PDU or LL PDU that includes an instant cannot be performed because the instant when this would have occurred has passed.")\
BG_ERROR_ENUM(bg_err_bt_pairing_with_unit_key_not_supported,"It was not possible to pair as a unit key was requested and it is not supported.")\
BG_ERROR_ENUM(bg_err_bt_different_transaction_collision,"LMP transaction was started that collides with an ongoing transaction.")\
BG_ERROR_ENUM(bg_err_bt_channel_assessment_not_supported,"The Controller cannot perform channel assessment because it is not supported.")\
BG_ERROR_ENUM(bg_err_bt_insufficient_security,"The HCI command or LMP PDU sent is only possible on an encrypted link.")\
BG_ERROR_ENUM(bg_err_bt_parameter_out_of_mandatory_range,"A parameter value requested is outside the mandatory range of parameters for the given HCI command or LMP PDU.")\
BG_ERROR_ENUM(bg_err_bt_simple_pairing_not_supported_by_host,"The IO capabilities request or response was rejected because the sending Host does not support Secure Simple Pairing even though the receiving Link Manager does.")\
BG_ERROR_ENUM(bg_err_bt_host_busy_pairing,"The Host is busy with another pairing operation and unable to support the requested pairing. The receiving device should retry pairing again later.")\
BG_ERROR_ENUM(bg_err_bt_connection_rejected_due_to_no_suitable_channel_found,"The Controller could not calculate an appropriate value for the Channel selection operation.")\
BG_ERROR_ENUM(bg_err_bt_controller_busy,"Operation was rejected because the controller is busy and unable to process the request.")\
BG_ERROR_ENUM(bg_err_bt_unacceptable_connection_interval,"Remote device terminated the connection because of an unacceptable connection interval.")\
BG_ERROR_ENUM(bg_err_bt_advertising_timeout,"Ddvertising for a fixed duration completed or, for directed advertising, that advertising completed without a connection being created.")\
BG_ERROR_ENUM(bg_err_bt_connection_terminated_due_to_mic_failure,"Connection was terminated because the Message Integrity Check (MIC) failed on a received packet.")\
BG_ERROR_ENUM(bg_err_bt_connection_failed_to_be_established,"LL initiated a connection but the connection has failed to be established. Controller did not receive any packets from remote end.")\
BG_ERROR_ENUM(bg_err_bt_mac_connection_failed,"The MAC of the 802.11 AMP was requested to connect to a peer, but the connection failed.")\
BG_ERROR_ENUM(bg_err_bt_coarse_clock_adjustment_rejected_but_will_try_to_adjust_using_clock_dragging,"The master, at this time, is unable to make a coarse adjustment to the piconet clock, using the supplied parameters. Instead the master will attempt to move the clock using clock dragging.")\
BG_ERROR_ENUM(bg_err_bt_unknown_advertising_identifier,"A command was sent from the Host that should identify an Advertising or Sync handle, but the Advertising or Sync handle does not exist.")\
BG_ERROR_ENUM(bg_err_bt_limit_reached,"Number of operations requested has been reached and has indicated the completion of the activity (e.g., advertising or scanning).")\
BG_ERROR_ENUM(bg_err_bt_operation_cancelled_by_host,"A request to the Controller issued by the Host and still pending was successfully canceled.")\
BG_ERROR_ENUM(bg_err_bt_packet_too_long,"An attempt was made to send or receive a packet that exceeds the maximum allowed packet length.")\
BG_ERROR_ENUM(bg_err_application_file_open_failed,"File open failed.")\
BG_ERROR_ENUM(bg_err_application_xml_parse_failed,"XML parsing failed.")\
BG_ERROR_ENUM(bg_err_application_device_connection_failed,"Device connection failed.")\
BG_ERROR_ENUM(bg_err_application_device_comunication_failed,"Device communication failed.")\
BG_ERROR_ENUM(bg_err_application_authentication_failed,"Device authentication failed.")\
BG_ERROR_ENUM(bg_err_application_incorrect_gatt_database,"Device has incorrect GATT database.")\
BG_ERROR_ENUM(bg_err_application_disconnected_due_to_procedure_collision,"Device disconnected due to procedure collision.")\
BG_ERROR_ENUM(bg_err_application_disconnected_due_to_secure_session_failed,"Device disconnected due to failure to establish or reestablish a secure session.")\
BG_ERROR_ENUM(bg_err_application_encryption_decryption_error,"Encrypion/decryption operation failed.")\
BG_ERROR_ENUM(bg_err_application_maximum_retries,"Maximum allowed retries exceeded.")\
BG_ERROR_ENUM(bg_err_application_data_parse_failed,"Data parsing failed.")\
BG_ERROR_ENUM(bg_err_application_pairing_removed,"Pairing established by the application layer protocol has been removed.")\
BG_ERROR_ENUM(bg_err_application_inactive_timeout,"Inactive timeout.")\
BG_ERROR_ENUM(bg_err_application_mismatched_or_insufficient_security,"Mismatched or insufficient security level")\
BG_ERROR_ENUM(bg_err_att_invalid_handle,"The attribute handle given was not valid on this server")\
BG_ERROR_ENUM(bg_err_att_read_not_permitted,"The attribute cannot be read")\
BG_ERROR_ENUM(bg_err_att_write_not_permitted,"The attribute cannot be written")\
BG_ERROR_ENUM(bg_err_att_invalid_pdu,"The attribute PDU was invalid")\
BG_ERROR_ENUM(bg_err_att_insufficient_authentication,"The attribute requires authentication before it can be read or written.")\
BG_ERROR_ENUM(bg_err_att_request_not_supported,"Attribute Server does not support the request received from the client.")\
BG_ERROR_ENUM(bg_err_att_invalid_offset,"Offset specified was past the end of the attribute")\
BG_ERROR_ENUM(bg_err_att_insufficient_authorization,"The attribute requires authorization before it can be read or written.")\
BG_ERROR_ENUM(bg_err_att_prepare_queue_full,"Too many prepare writes have been queueud")\
BG_ERROR_ENUM(bg_err_att_att_not_found,"No attribute found within the given attribute handle range.")\
BG_ERROR_ENUM(bg_err_att_att_not_long,"The attribute cannot be read or written using the Read Blob Request")\
BG_ERROR_ENUM(bg_err_att_insufficient_enc_key_size,"The Encryption Key Size used for encrypting this link is insufficient.")\
BG_ERROR_ENUM(bg_err_att_invalid_att_length,"The attribute value length is invalid for the operation")\
BG_ERROR_ENUM(bg_err_att_unlikely_error,"The attribute request that was requested has encountered an error that was unlikely, and therefore could not be completed as requested.")\
BG_ERROR_ENUM(bg_err_att_insufficient_encryption,"The attribute requires encryption before it can be read or written.")\
BG_ERROR_ENUM(bg_err_att_unsupported_group_type,"The attribute type is not a supported grouping attribute as defined by a higher layer specification.")\
BG_ERROR_ENUM(bg_err_att_insufficient_resources,"Insufficient Resources to complete the request")\
BG_ERROR_ENUM(bg_err_att_out_of_sync,"The server requests the client to rediscover the database.")\
BG_ERROR_ENUM(bg_err_att_value_not_allowed,"The attribute parameter value was not allowed.")\
BG_ERROR_ENUM(bg_err_att_application,"When this is returned in a BGAPI response, the application tried to read or write the value of a user attribute from the GATT database.")\
BG_ERROR_ENUM(bg_err_mesh_already_exists,"Returned when trying to add a key or some other unique resource with an ID which already exists")\
BG_ERROR_ENUM(bg_err_mesh_does_not_exist,"Returned when trying to manipulate a key or some other resource with an ID which does not exist")\
BG_ERROR_ENUM(bg_err_mesh_limit_reached,"Returned when an operation cannot be executed because a pre-configured limit for keys, key bindings, elements, models, virtual addresses, provisioned devices, or provisioning sessions is reached")\
BG_ERROR_ENUM(bg_err_mesh_invalid_address,"Returned when trying to use a reserved address or add a \"pre-provisioned\" device using an address already used by some other device")\
BG_ERROR_ENUM(bg_err_mesh_malformed_data,"In a BGAPI response, the user supplied malformed data; in a BGAPI event, the remote end responded with malformed or unrecognized data")\
BG_ERROR_ENUM(bg_err_mesh_already_initialized,"An attempt was made to initialize a subsystem that was already initialized.")\
BG_ERROR_ENUM(bg_err_mesh_not_initialized,"An attempt was made to use a subsystem that wasn't initialized yet. Call the subsystem's init function first.")\
BG_ERROR_ENUM(bg_err_mesh_no_friend_offer,"Returned when trying to establish a friendship as a Low Power Node, but no acceptable friend offer message was received.")\
BG_ERROR_ENUM(bg_err_mesh_prov_link_closed,"Provisioning link was unexpectedly closed before provisioning was complete.")\
BG_ERROR_ENUM(bg_err_mesh_prov_invalid_pdu,"An unrecognized provisioning PDU was received.")\
BG_ERROR_ENUM(bg_err_mesh_prov_invalid_pdu_format,"A provisioning PDU with wrong length or containing field values that are out of bounds was received.")\
BG_ERROR_ENUM(bg_err_mesh_prov_unexpected_pdu,"An unexpected (out of sequence) provisioning PDU was received.")\
BG_ERROR_ENUM(bg_err_mesh_prov_confirmation_failed,"The computed confirmation value did not match the expected value.")\
BG_ERROR_ENUM(bg_err_mesh_prov_out_of_resources,"Provisioning could not be continued due to unsufficient resources.")\
BG_ERROR_ENUM(bg_err_mesh_prov_decryption_failed,"The provisioning data block could not be decrypted.")\
BG_ERROR_ENUM(bg_err_mesh_prov_unexpected_error,"An unexpected error happened during provisioning.")\
BG_ERROR_ENUM(bg_err_mesh_prov_cannot_assign_addr,"Device could not assign unicast addresses to all of its elements.")\
BG_ERROR_ENUM(bg_err_mesh_address_temporarily_unavailable,"Returned when trying to reuse an address of a previously deleted device before an IV Index Update has been executed.")\
BG_ERROR_ENUM(bg_err_mesh_address_already_used,"Returned when trying to assign an address that is used by one of the devices in the Device Database, or by the Provisioner itself.")\
BG_ERROR_ENUM(bg_err_mesh_foundation_invalid_address,"Returned when address in request was not valid")\
BG_ERROR_ENUM(bg_err_mesh_foundation_invalid_model,"Returned when model identified is not found for a given element")\
BG_ERROR_ENUM(bg_err_mesh_foundation_invalid_app_key,"Returned when the key identified by AppKeyIndex is not stored in the node")\
BG_ERROR_ENUM(bg_err_mesh_foundation_invalid_net_key,"Returned when the key identified by NetKeyIndex is not stored in the node")\
BG_ERROR_ENUM(bg_err_mesh_foundation_insufficient_resources,"Returned when The node cannot serve the request due to insufficient resources")\
BG_ERROR_ENUM(bg_err_mesh_foundation_key_index_exists,"Returned when the key identified is already stored in the node and the new NetKey value is different")\
BG_ERROR_ENUM(bg_err_mesh_foundation_invalid_publish_params,"Returned when the model does not support the publish mechanism")\
BG_ERROR_ENUM(bg_err_mesh_foundation_not_subscribe_model,"Returned when  the model does not support the subscribe mechanism")\
BG_ERROR_ENUM(bg_err_mesh_foundation_storage_failure,"Returned when storing of the requested parameters failed")\
BG_ERROR_ENUM(bg_err_mesh_foundation_not_supported,"Returned when requested setting is not supported")\
BG_ERROR_ENUM(bg_err_mesh_foundation_cannot_update,"Returned when the requested update operation cannot be performed due to general constraints")\
BG_ERROR_ENUM(bg_err_mesh_foundation_cannot_remove,"Returned when the requested delete operation cannot be performed due to general constraints")\
BG_ERROR_ENUM(bg_err_mesh_foundation_cannot_bind,"Returned when the requested bind operation cannot be performed due to general constraints")\
BG_ERROR_ENUM(bg_err_mesh_foundation_temporarily_unable,"Returned when The node cannot start advertising with Node Identity or Proxy since the maximum number of parallel advertising is reached")\
BG_ERROR_ENUM(bg_err_mesh_foundation_cannot_set,"Returned when the requested state cannot be set")\
BG_ERROR_ENUM(bg_err_mesh_foundation_unspecified,"Returned when an unspecified error took place")\
BG_ERROR_ENUM(bg_err_mesh_foundation_invalid_binding,"Returned when the NetKeyIndex and AppKeyIndex combination is not valid for a Config AppKey Update")\
BG_ERROR_ENUM(bg_err_filesystem_file_not_found,"File not found")\
BG_ERROR_ENUM(bg_err_l2cap_remote_disconnected,"Returned when remote disconnects the connection-oriented channel by sending disconnection request.")\
BG_ERROR_ENUM(bg_err_l2cap_local_disconnected,"Returned when local host disconnect the connection-oriented channel by sending disconnection request.")\
BG_ERROR_ENUM(bg_err_l2cap_cid_not_exist,"Returned when local host did not find a connection-oriented channel with given destination CID.")\
BG_ERROR_ENUM(bg_err_l2cap_le_disconnected,"Returned when connection-oriented channel disconnected due to LE connection is dropped.")\
BG_ERROR_ENUM(bg_err_l2cap_flow_control_violated,"Returned when connection-oriented channel disconnected due to remote end send data even without credit.")\
BG_ERROR_ENUM(bg_err_l2cap_flow_control_credit_overflowed,"Returned when connection-oriented channel disconnected due to remote end send flow control credits exceed 65535.")\
BG_ERROR_ENUM(bg_err_l2cap_no_flow_control_credit,"Returned when connection-oriented channel has run out of flow control credit and local application still trying to send data.")\
BG_ERROR_ENUM(bg_err_l2cap_connection_request_timeout,"Returned when connection-oriented channel has not received connection response message within maximum timeout.")\
BG_ERROR_ENUM(bg_err_l2cap_invalid_cid,"Returned when local host received a connection-oriented channel connection response with an invalid destination CID.")\
BG_ERROR_ENUM(bg_err_l2cap_wrong_state,"Returned when local host application tries to send a command which is not suitable for L2CAP channel's current state.")\
BG_ERROR_ENUM(bg_err_security_image_signature_verification_failed,"Device firmware signature verification failed.")\
BG_ERROR_ENUM(bg_err_security_file_signature_verification_failed,"File signature verification failed.")\
BG_ERROR_ENUM(bg_err_security_image_checksum_error,"Device firmware checksum is not valid.")\
BG_ERROR_ENUM(bg_err_last,"last_entry")



/**
 * A macro to log the result of a Blue Gecko Bluetooth API function call defined by call
 * @param call is the function to call, with arguments.  Stringified for logging purposes only
 * @param result is the result structure returned by the call.  This result is checked for success, and if
 * not bg_err_success a descriptive warning message is logged
 */
#define BTSTACK_LOG_RESULT(call,result)\
if( result != bg_err_success ) { \
	LOG_WARN("%s failed with result %d (%s - detail: %s)",\
		#call,result,bleResponseString((enum bg_error)result),bleResponseFailureDescription((enum bg_error)result));\
}\

/**
 * A macro to check the result of the Blue Gecko function call defined by call
 * Wrap your function call with this macro to print a descriptive warning message when an error occurs in the
 * call result return code.
 * @param call is the function to call, along with any arguments, for instance
 * BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_get_rssi(ble_connection->connection_handle))
 */
#define BTSTACK_CHECK_RESPONSE(call) \
{\
	BTSTACK_LOG_RESULT(call,call->result)\
}

const char *bleResponseFailureDescription(enum bg_error error);
const char *bleResponseString(enum bg_error error);


#endif /* SRC_GECKO_BLE_ERRORS_H_ */
