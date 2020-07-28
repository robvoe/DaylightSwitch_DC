/*
 * Definitions.cpp
 *
 *  Created on: 07.04.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *    Contains type definitions used by the RelayHandler module.
 */

#include "Definitions.h"


namespace Hardware {

	RelayState operator!(RelayState value) {
		if ( value == RelayState::Closed )
			return RelayState::Open;
		else
			return RelayState::Closed;
	}

} /*namespace hardware*/
