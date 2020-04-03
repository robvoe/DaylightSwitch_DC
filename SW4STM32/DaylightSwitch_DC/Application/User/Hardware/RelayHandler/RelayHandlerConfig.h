/*
 * RelayHandlerConfig.h
 *
 *  Created on: 03.04.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *  	Defines the user configuration of the RelayHandler module. It will be managed by the Persistence module during
 *  	runtime ─ thus, as initiated by the application, all values are stored in Flash memory.
 */

#ifndef APPLICATION_USER_HARDWARE_RELAYHANDLER_RELAYHANDLERCONFIG_H_
#define APPLICATION_USER_HARDWARE_RELAYHANDLER_RELAYHANDLERCONFIG_H_


#include "Definitions.h"

namespace Hardware {

	/**
	 * Holds the configuration of the RelayHandler.
	 */
	struct RelayHandlerConfig {
		RelayState InitialState;	///< Holds the relay state that shall be initialized.

		static void ResetData(RelayHandlerConfig &instance) {
			instance.InitialState = RelayState::Open;
		}
	};

} /*namespace Hardware*/


#endif /* APPLICATION_USER_HARDWARE_RELAYHANDLER_RELAYHANDLERCONFIG_H_ */
