/*
 * NighttimesClosed.h
 *
 *  Created on: 21.08.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *    Defines the following switching scheme:
 *      - relay is closed at low brightness levels / closed at low brightness levels,
 *      - there is an optional OpenRelayTimer that opens the relay after a specified time.
 */
#include "NighttimesClosed.h"

#include <Stm32/SwoLogger/SwoLogger.h>
#include <Stm32/SoftTimer/SoftTimer.h>
#include <Comparators/ComparatorBase.h>

#include "Hardware/RelayHandler/RelayHandler.h"


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

	NighttimesClosed::NighttimesClosed(AppBaseConfig &appBaseConfig) :
		AppBase(appBaseConfig),
		_openRelayTimer(OpenRelayTimerMillis, false, timerElapsed)
	{
		// Nothing happens here..
	}

	void NighttimesClosed::handleTimer(RelayState newRelayState) {
		if ( UseOpenRelayTimer ) {
			if ( newRelayState == RelayState::Closed )  _openRelayTimer.enable();
			else                                        _openRelayTimer.disable();
		}
	}

	void NighttimesClosed::handleBrightnessComparatorEvent( Util::Comparators::ComparatorState newComparatorState ) {
		const RelayState newRelayState = (newComparatorState == ComparatorState::Low) ? RelayState::Closed : RelayState::Open;
		RelayHandler::enqueueOpenCloseCommand( newRelayState );
		handleTimer(newRelayState);
	}

	void NighttimesClosed::_main() {
		if ( UseOpenRelayTimer )  _openRelayTimer.main();
	}

	RelayState NighttimesClosed::handleButtonUp() {
		const RelayState newRelayState = AppBase::handleButtonUp();
		handleTimer(newRelayState);
		return newRelayState;
	}

} /* namespace Apps */

