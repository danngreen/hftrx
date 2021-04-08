/******************************************************************************
* Copyright (C) 2010 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xscuwdt_sinit.c
* @addtogroup scuwdt_v2_2
* @{
*
* This file contains method for static initialization (compile-time) of the
* driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- ---------------------------------------------
* 1.00a sdm 01/15/10 First release
* 2.1 	sk  02/26/15 Modified the code for MISRA-C:2012 compliance.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include <xparameters.h>
#include <xscuwdt.h>

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
extern XScuWdt_Config XScuWdt_ConfigTable[XPAR_XSCUWDT_NUM_INSTANCES];

/*****************************************************************************/
/**
* Lookup the device configuration based on the unique device ID. The table
* contains the configuration info for each device in the system.
*
* @param	DeviceId is the unique device ID of the device being looked up.
*
* @return	A pointer to the configuration table entry corresponding to the
*		given device ID, or NULL if no match is found.
*
* @note		None.
*
******************************************************************************/
XScuWdt_Config *XScuWdt_LookupConfig(u16 DeviceId)
{
	XScuWdt_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0U; Index < XPAR_XSCUWDT_NUM_INSTANCES; Index++) {
		if (XScuWdt_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XScuWdt_ConfigTable[Index];
			break;
		}
	}

	return (XScuWdt_Config *)CfgPtr;
}
/** @} */