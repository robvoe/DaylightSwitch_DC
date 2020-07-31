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


extern Apps::DaytimesClosed app;  // --> Points to the app instance

namespace Apps {
	using namespace Util::Stm32;
	using namespace Util::Comparators;
	using namespace Hardware;

	/** Gets called as soon as relay shall open */
	static void timerElapsed(SoftTimer &softTimer) {
		softTimer.disable();
		RelayHandler::enqueueOpenCommand();
		SwoLogger::log("OpenRelayTimer elapsed");
	}

	DaytimesClosed::DaytimesClosed(AppBaseConfig &appBaseConfig) :
		AppBase(appBaseConfig),
		_openRelayTimer(OpenRelayTimerMillis, false, timerElapsed)
	{
		// Nothing happens here..
	}

	void DaytimesClosed::handleBrightnessComparatorEvent( Util::Comparators::ComparatorState newComparatorState ) {
		const RelayState newRelayState = (newComparatorState == ComparatorState::High) ? RelayState::Closed : RelayState::Open;
		RelayHandler::enqueueOpenCloseCommand( newRelayState );

		// Now, handle the timer
		if ( UseOpenRelayTimer ) {
			if (newRelayState == RelayState::Closed)
				_openRelayTimer.enable();
			else if (newRelayState == RelayState::Open)
				_openRelayTimer.disable();
		}
	}

	void DaytimesClosed::_main() {
		if ( UseOpenRelayTimer )  _openRelayTimer.main();
	}

} /* namespace Apps */
