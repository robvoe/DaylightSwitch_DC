/*
 * DaytimesClosed.cpp
 *
 *  Created on: 24.05.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *    TODO
 */
#include "DaytimesClosed.h"

#include <Stm32/SwoLogger/SwoLogger.h>
#include <Stm32/SoftTimer/SoftTimer.h>
#include <Comparators/ComparatorBase.h>

#include "Hardware/RelayHandler/RelayHandler.h"


namespace Apps {
	using namespace Util::Stm32;
	using namespace Util::Comparators;
	using namespace Hardware;

	/** Gets called as soon as relay shall open */
	static void timerElapsed_Dark(SoftTimer &softTimer) {
		softTimer.disable();
		RelayHandler::enqueueOpenCommand();
		SwoLogger::log("OpenRelayTimer_Dark elapsed");
	}

	/** Gets called as soon as relay shall open */
	static void timerElapsed_Bright(SoftTimer &softTimer) {
		softTimer.disable();
		RelayHandler::enqueueOpenCommand();
		SwoLogger::log("OpenRelayTimer_Bright elapsed");
	}

	DaytimesClosed::DaytimesClosed(AppBaseConfig &appBaseConfig) :
		AppBase(appBaseConfig),
		_openRelayTimer_Bright(OpenRelayTimerMillis_Bright, false, timerElapsed_Bright),
		_openRelayTimer_Dark(OpenRelayTimerMillis_Dark, false, timerElapsed_Dark)
	{
		// Nothing happens here..
	}

	void DaytimesClosed::handleTimers(RelayState newRelayState) {
		if ( UseOpenRelayTimer_Dark )  _openRelayTimer_Dark.disable();
		if ( UseOpenRelayTimer_Bright )  _openRelayTimer_Bright.disable();

		if ( newRelayState != RelayState::Closed )  return;

		// Turn on timer, depending on brightness
		using namespace Util::Comparators;
		const ComparatorState brightnessComparatorState = getBrightnessComparatorState();
		if ( brightnessComparatorState == ComparatorState::High ) {
			if ( UseOpenRelayTimer_Bright )  _openRelayTimer_Bright.enable();
		}
		else if ( brightnessComparatorState == ComparatorState::Low  ||  brightnessComparatorState == ComparatorState::Undefined ) {
			if ( UseOpenRelayTimer_Dark )  _openRelayTimer_Dark.enable();
		}

	}

	void DaytimesClosed::handleBrightnessComparatorEvent( Util::Comparators::ComparatorState newComparatorState ) {
		const RelayState newRelayState = (newComparatorState == ComparatorState::High) ? RelayState::Closed : RelayState::Open;
		RelayHandler::enqueueOpenCloseCommand( newRelayState );
		handleTimers(newRelayState);
	}

	void DaytimesClosed::_main() {
		if ( UseOpenRelayTimer_Bright )  _openRelayTimer_Bright.main();
		if ( UseOpenRelayTimer_Dark )  _openRelayTimer_Dark.main();
	}

	Hardware::RelayState DaytimesClosed::handleButtonUp() {
		const RelayState newRelayState = AppBase::handleButtonUp();
		handleTimers(newRelayState);
		return newRelayState;
	}

} /* namespace Apps */

