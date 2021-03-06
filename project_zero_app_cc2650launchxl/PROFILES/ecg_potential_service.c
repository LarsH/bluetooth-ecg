/******************************************************************************
 * Filename:       ECG_potential_service.c
 *
 * Description:    This file contains the implementation of the service.
 *
 *                 Generated by:
 *                 BDS version: 1.1.3139.0
 *                 Plugin:      Texas Instruments BLE SDK GATT Server plugin 1.0.9
 *                 Time:        Mon Nov 27 2017 11:02:32 GMT+01:00
 *

 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **********************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include <string.h>

//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#ifdef UARTLOG_ENABLE
#  include "UartLog.h"
#endif


#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "ecg_potential_service.h"
#include <driverlib/aon_rtc.h>

#include "ecg_buffer.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
* GLOBAL VARIABLES
*/

// ECG_potential_service Service UUID
CONST uint8_t EcgPotentialServiceUUID[ATT_UUID_SIZE] =
{
  ECG_POTENTIAL_SERVICE_SERV_UUID_BASE128(ECG_POTENTIAL_SERVICE_SERV_UUID)
};

// ECG_enable UUID
CONST uint8_t eps_ECG_enableUUID[ATT_UUID_SIZE] =
{
  EPS_ECG_ENABLE_UUID_BASE128(EPS_ECG_ENABLE_UUID)
};

// ECG_potential_measurement UUID
CONST uint8_t eps_ECG_potential_measurementUUID[ATT_UUID_SIZE] =
{
  EPS_ECG_POTENTIAL_MEASUREMENT_UUID_BASE128(EPS_ECG_POTENTIAL_MEASUREMENT_UUID)
};

// Sensor_Placement UUID
CONST uint8_t eps_Sensor_PlacementUUID[ATT_UUID_SIZE] =
{
  EPS_SENSOR_PLACEMENT_UUID_BASE128(EPS_SENSOR_PLACEMENT_UUID)
};

// Ack UUID
CONST uint8_t eps_AckUUID[ATT_UUID_SIZE] =
{
  EPS_ACK_UUID_BASE128(EPS_ACK_UUID)
};


/*********************************************************************
 * LOCAL VARIABLES
 */

static EcgPotentialServiceCBs_t *pAppCBs = NULL;
static uint8_t eps_icall_rsp_task_id = INVALID_TASK_ID;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t EcgPotentialServiceDecl = { ATT_UUID_SIZE, EcgPotentialServiceUUID };

// Characteristic "ECG enable" Properties (for declaration)
static uint8_t eps_ECG_enableProps = GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_WRITE_NO_RSP;

// Characteristic "ECG enable" Value variable
static uint8_t eps_ECG_enableVal[EPS_ECG_ENABLE_LEN] = {0};

// Length of data in characteristic "ECG enable" Value variable, initialized to minimal size.
static uint16_t eps_ECG_enableValLen = EPS_ECG_ENABLE_LEN_MIN;



// Characteristic "ECG potential measurement" Properties (for declaration)
static uint8_t eps_ECG_potential_measurementProps = GATT_PROP_NOTIFY | GATT_PROP_READ;

// Characteristic "ECG potential measurement" Value variable
static uint8_t eps_ECG_potential_measurementVal[EPS_ECG_POTENTIAL_MEASUREMENT_LEN] = {0};

// Length of data in characteristic "ECG potential measurement" Value variable, initialized to minimal size.
static uint16_t eps_ECG_potential_measurementValLen = EPS_ECG_POTENTIAL_MEASUREMENT_LEN_MIN;

// Characteristic "ECG potential measurement" Client Characteristic Configuration Descriptor
static gattCharCfg_t *eps_ECG_potential_measurementConfig;



// Characteristic "Sensor Placement" Properties (for declaration)
static uint8_t eps_Sensor_PlacementProps = GATT_PROP_READ;

// Characteristic "Sensor Placement" Value variable
static uint8_t eps_Sensor_PlacementVal[EPS_SENSOR_PLACEMENT_LEN] = {0};

// Length of data in characteristic "Sensor Placement" Value variable, initialized to minimal size.
static uint16_t eps_Sensor_PlacementValLen = EPS_SENSOR_PLACEMENT_LEN_MIN;



// Characteristic "Ack" Properties (for declaration)
static uint8_t eps_AckProps = GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_WRITE_NO_RSP;

// Characteristic "Ack" Value variable
static uint8_t eps_AckVal[EPS_ACK_LEN] = {0};

// Length of data in characteristic "Ack" Value variable, initialized to minimal size.
static uint16_t eps_AckValLen = EPS_ACK_LEN_MIN;



/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t ECG_potential_serviceAttrTbl[] =
{
  // ECG_potential_service Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&EcgPotentialServiceDecl
  },
    // ECG enable Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &eps_ECG_enableProps
    },
      // ECG enable Characteristic Value
      {
        { ATT_UUID_SIZE, eps_ECG_enableUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_WRITE,
        0,
        eps_ECG_enableVal
      },
    // ECG potential measurement Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &eps_ECG_potential_measurementProps
    },
      // ECG potential measurement Characteristic Value
      {
        { ATT_UUID_SIZE, eps_ECG_potential_measurementUUID },
        GATT_PERMIT_READ,
        0,
        eps_ECG_potential_measurementVal
      },
      // ECG potential measurement CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&eps_ECG_potential_measurementConfig
      },
    // Sensor Placement Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &eps_Sensor_PlacementProps
    },
      // Sensor Placement Characteristic Value
      {
        { ATT_UUID_SIZE, eps_Sensor_PlacementUUID },
        GATT_PERMIT_READ,
        0,
        eps_Sensor_PlacementVal
      },
    // Ack Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &eps_AckProps
    },
      // Ack Characteristic Value
      {
        { ATT_UUID_SIZE, eps_AckUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_WRITE,
        0,
        eps_AckVal
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t ECG_potential_service_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                           uint16_t maxLen, uint8_t method );
static bStatus_t ECG_potential_service_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len, uint16_t offset,
                                            uint8_t method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t ECG_potential_serviceCBs =
{
  ECG_potential_service_ReadAttrCB,  // Read callback function pointer
  ECG_potential_service_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * EcgPotentialService_AddService- Initializes the EcgPotentialService service by registering
 *          GATT attributes with the GATT server.
 *
 *    rspTaskId - The ICall Task Id that should receive responses for Indications.
 */
extern bStatus_t EcgPotentialService_AddService( uint8_t rspTaskId )
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  eps_ECG_potential_measurementConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( eps_ECG_potential_measurementConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, eps_ECG_potential_measurementConfig );
  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( ECG_potential_serviceAttrTbl,
                                        GATT_NUM_ATTRS( ECG_potential_serviceAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &ECG_potential_serviceCBs );
  Log_info1("Registered service, %d attributes", (IArg)GATT_NUM_ATTRS( ECG_potential_serviceAttrTbl ));
  eps_icall_rsp_task_id = rspTaskId;

  return ( status );
}

/*
 * EcgPotentialService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t EcgPotentialService_RegisterAppCBs( EcgPotentialServiceCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    pAppCBs = appCallbacks;
    Log_info1("Registered callbacks to application. Struct %p", (IArg)appCallbacks);
    return ( SUCCESS );
  }
  else
  {
    Log_warning0("Null pointer given for app callbacks.");
    return ( FAILURE );
  }
}

/*
 * EcgPotentialService_SetParameter - Set a EcgPotentialService parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
bStatus_t EcgPotentialService_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  uint8_t  *pAttrVal;
  uint16_t *pValLen;
  uint16_t valMinLen;
  uint16_t valMaxLen;
  uint8_t sendNotiInd = FALSE;
  gattCharCfg_t *attrConfig;
  uint8_t needAuth = TRUE;

  switch ( param )
  {
    case EPS_ECG_ENABLE_ID:
      pAttrVal  =  eps_ECG_enableVal;
      pValLen   = &eps_ECG_enableValLen;
      valMinLen =  EPS_ECG_ENABLE_LEN_MIN;
      valMaxLen =  EPS_ECG_ENABLE_LEN;
      Log_info2("SetParameter : %s len: %d", (IArg)"ECG_enable", (IArg)len);
      break;

    case EPS_ECG_POTENTIAL_MEASUREMENT_ID:
      pAttrVal  =  eps_ECG_potential_measurementVal;
      pValLen   = &eps_ECG_potential_measurementValLen;
      valMinLen =  EPS_ECG_POTENTIAL_MEASUREMENT_LEN_MIN;
      valMaxLen =  EPS_ECG_POTENTIAL_MEASUREMENT_LEN;
      sendNotiInd = TRUE;
      attrConfig  = eps_ECG_potential_measurementConfig;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"ECG_potential_measurement", (IArg)len);
      break;

    case EPS_SENSOR_PLACEMENT_ID:
      pAttrVal  =  eps_Sensor_PlacementVal;
      pValLen   = &eps_Sensor_PlacementValLen;
      valMinLen =  EPS_SENSOR_PLACEMENT_LEN_MIN;
      valMaxLen =  EPS_SENSOR_PLACEMENT_LEN;
      Log_info2("SetParameter : %s len: %d", (IArg)"Sensor_Placement", (IArg)len);
      break;

    case EPS_ACK_ID:
      pAttrVal  =  eps_AckVal;
      pValLen   = &eps_AckValLen;
      valMinLen =  EPS_ACK_LEN_MIN;
      valMaxLen =  EPS_ACK_LEN;
      Log_info2("SetParameter : %s len: %d", (IArg)"Ack", (IArg)len);
      break;

    default:
      Log_error1("SetParameter: Parameter #%d not valid.", (IArg)param);
      return INVALIDPARAMETER;
  }

  // Check bounds, update value and send notification or indication if possible.
  if ( len <= valMaxLen && len >= valMinLen )
  {
    memcpy(pAttrVal, value, len);
    *pValLen = len; // Update length for read and get.

    if (sendNotiInd)
    {
      Log_info2("Trying to send noti/ind: connHandle %x, %s",
                (IArg)attrConfig[0].connHandle,
                (IArg)((attrConfig[0].value==0)?"\x1b[33mNoti/ind disabled\x1b[0m" :
                       (attrConfig[0].value==1)?"Notification enabled" :
                                                "Indication enabled"));
      // Try to send notification.
      GATTServApp_ProcessCharCfg( attrConfig, pAttrVal, needAuth,
                                  ECG_potential_serviceAttrTbl, GATT_NUM_ATTRS( ECG_potential_serviceAttrTbl ),
                                  eps_icall_rsp_task_id,  ECG_potential_service_ReadAttrCB);
    }
  }
  else
  {
    Log_error3("Length outside bounds: Len: %d MinLen: %d MaxLen: %d.", (IArg)len, (IArg)valMinLen, (IArg)valMaxLen);
    ret = bleInvalidRange;
  }

  return ret;
}


/*
 * EcgPotentialService_GetParameter - Get a EcgPotentialService parameter.
 *
 *    param - Profile parameter ID
 *    len   - pointer to a variable that contains the maximum length that can be written to *value.
              After the call, this value will contain the actual returned length.
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
bStatus_t EcgPotentialService_GetParameter( uint8_t param, uint16_t *len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case EPS_ECG_ENABLE_ID:
      *len = MIN(*len, eps_ECG_enableValLen);
      memcpy(value, eps_ECG_enableVal, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"ECG_enable", (IArg)*len);
      break;

    case EPS_ACK_ID:
      *len = MIN(*len, eps_AckValLen);
      memcpy(value, eps_AckVal, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"Ack", (IArg)*len);
      break;

    default:
      Log_error1("GetParameter: Parameter #%d not valid.", (IArg)param);
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}

/*********************************************************************
 * @internal
 * @fn          ECG_potential_service_findCharParamId
 *
 * @brief       Find the logical param id of an attribute in the service's attr table.
 *
 *              Works only for Characteristic Value attributes and
 *              Client Characteristic Configuration Descriptor attributes.
 *
 * @param       pAttr - pointer to attribute
 *
 * @return      uint8_t paramID (ref ECG_potential_service.h) or 0xFF if not found.
 */
static uint8_t ECG_potential_service_findCharParamId(gattAttribute_t *pAttr)
{
  // Is this a Client Characteristic Configuration Descriptor?
  if (ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID == *(uint16_t *)pAttr->type.uuid)
    return ECG_potential_service_findCharParamId(pAttr - 1); // Assume the value attribute precedes CCCD and recurse

  // Is this attribute in "ECG enable"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, eps_ECG_enableUUID, pAttr->type.len))
    return EPS_ECG_ENABLE_ID;

  // Is this attribute in "ECG potential measurement"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, eps_ECG_potential_measurementUUID, pAttr->type.len))
    return EPS_ECG_POTENTIAL_MEASUREMENT_ID;

  // Is this attribute in "Sensor Placement"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, eps_Sensor_PlacementUUID, pAttr->type.len))
    return EPS_SENSOR_PLACEMENT_ID;

  // Is this attribute in "Ack"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, eps_AckUUID, pAttr->type.len))
    return EPS_ACK_ID;

  else
    return 0xFF; // Not found. Return invalid.
}

/*********************************************************************
 * @fn          ECG_potential_service_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t ECG_potential_service_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;
  uint16_t valueLen;
  uint8_t paramID = 0xFF;

  // Find settings for the characteristic to be read.
  paramID = ECG_potential_service_findCharParamId( pAttr );
  switch ( paramID )
  {
    case EPS_ECG_ENABLE_ID:
      valueLen = eps_ECG_enableValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"ECG_enable",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for ECG enable can be inserted here */
      break;

    case EPS_ECG_POTENTIAL_MEASUREMENT_ID:
      valueLen = eps_ECG_potential_measurementValLen;
      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"ECG_potential_measurement",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for ECG potential measurement can be inserted here */
      break;

    case EPS_SENSOR_PLACEMENT_ID:
      valueLen = eps_Sensor_PlacementValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"Sensor_Placement",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for Sensor Placement can be inserted here */
      break;

    case EPS_ACK_ID:
      valueLen = eps_AckValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"Ack",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for Ack can be inserted here */
      break;

    default:
      Log_error0("Attribute was not found.");
      return ATT_ERR_ATTR_NOT_FOUND;
  }
  // Check bounds and return the value
  if ( offset > valueLen )  // Prevent malicious ATT ReadBlob offsets.
  {
    Log_error0("An invalid offset was requested.");
    status = ATT_ERR_INVALID_OFFSET;
  }
  else
  {
    *pLen = MIN(maxLen, valueLen - offset);  // Transmit as much as possible
    memcpy(pValue, pAttr->pValue + offset, *pLen);
  }

  return status;
}

/*********************************************************************
 * @fn      ECG_potential_service_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t ECG_potential_service_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t len, uint16_t offset,
                                        uint8_t method )
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;
  uint8_t   changeParamID = 0xFF;
  uint16_t writeLenMin;
  uint16_t writeLenMax;
  uint16_t *pValueLenVar;

  // See if request is regarding a Client Characterisic Configuration
  if (ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID == *(uint16_t *)pAttr->type.uuid)
  {
    // Allow notification and indication, but do not check if really allowed per CCCD.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY |
                                                     GATT_CLIENT_CFG_INDICATE );
    if (SUCCESS == status && pAppCBs && pAppCBs->pfnCfgChangeCb)
       pAppCBs->pfnCfgChangeCb( connHandle, ECG_POTENTIAL_SERVICE_SERV_UUID,
                                ECG_potential_service_findCharParamId(pAttr), pValue, len );

     return status;
  }

  // Find settings for the characteristic to be written.
  paramID = ECG_potential_service_findCharParamId( pAttr );
  switch ( paramID )
  {
    case EPS_ECG_ENABLE_ID:
      writeLenMin  = EPS_ECG_ENABLE_LEN_MIN;
      writeLenMax  = EPS_ECG_ENABLE_LEN;
      pValueLenVar = &eps_ECG_enableValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"ECG_enable",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for ECG enable can be inserted here */
      setADCstore(1);       /* Received enable from phone, enable measuring */
      break;

    case EPS_ACK_ID:
      writeLenMin  = EPS_ACK_LEN_MIN;
      writeLenMax  = EPS_ACK_LEN;
      pValueLenVar = &eps_AckValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"Ack",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for Ack can be inserted here */
      requestNewData();
      break;

    default:
      Log_error0("Attribute was not found.");
      return ATT_ERR_ATTR_NOT_FOUND;
  }
  // Check whether the length is within bounds.
  if ( offset >= writeLenMax )
  {
    Log_error0("An invalid offset was requested.");
    status = ATT_ERR_INVALID_OFFSET;
  }
  else if ( offset + len > writeLenMax )
  {
    Log_error0("Invalid value length was received.");
    status = ATT_ERR_INVALID_VALUE_SIZE;
  }
  else if ( offset + len < writeLenMin && ( method == ATT_EXECUTE_WRITE_REQ || method == ATT_WRITE_REQ ) )
  {
    // Refuse writes that are lower than minimum.
    // Note: Cannot determine if a Reliable Write (to several chars) is finished, so those will
    //       only be refused if this attribute is the last in the queue (method is execute).
    //       Otherwise, reliable writes are accepted and parsed piecemeal.
    Log_error0("Invalid value length was received.");
    status = ATT_ERR_INVALID_VALUE_SIZE;
  }
  else
  {
    // Copy pValue into the variable we point to from the attribute table.
    memcpy(pAttr->pValue + offset, pValue, len);

    // Only notify application and update length if enough data is written.
    //
    // Note: If reliable writes are used (meaning several attributes are written to using ATT PrepareWrite),
    //       the application will get a callback for every write with an offset + len larger than _LEN_MIN.
    // Note: For Long Writes (ATT Prepare + Execute towards only one attribute) only one callback will be issued,
    //       because the write fragments are concatenated before being sent here.
    if ( offset + len >= writeLenMin )
    {
      changeParamID = paramID;
      *pValueLenVar = offset + len; // Update data length.
    }
  }

  // Let the application know something changed (if it did) by using the
  // callback it registered earlier (if it did).
  if (changeParamID != 0xFF)
    if ( pAppCBs && pAppCBs->pfnChangeCb )
      pAppCBs->pfnChangeCb( connHandle, ECG_POTENTIAL_SERVICE_SERV_UUID, paramID, pValue, len+offset ); // Call app function from stack task context.

  return status;
}
