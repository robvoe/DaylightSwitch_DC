/*
 * Definitions.h
 *
 *  Created on: 03.04.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *  	Contains type definitions used by the RelayHandler module.
 */

#ifndef APPLICATION_USER_HARDWARE_RELAYHANDLER_DEFINITIONS_H_
#define APPLICATION_USER_HARDWARE_RELAYHANDLER_DEFINITIONS_H_


namespace Hardware {

	/// Determines the relay state
	enum class RelayState {
		Open,
		Closed,
		Indeterminate  	///< The relay was not set to a certain state, yet. Thus, it could be either open or closed.
	};


	/**
	 * Inversion operator for RelayState.
	 *
	 * @param value  The value to be inversed.
	 * @return  Returns the logical inverse of RelayState.
	 */
	extern RelayState operator!(RelayState value);

} /*namespace Hardware*/



#endif /* APPLICATION_USER_HARDWARE_RELAYHANDLER_DEFINITIONS_H_ */
