/*
 * BrightnessHandler.cpp
 *
 *  Created on: 06.04.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *    TODO
 */
#include "Hardware/Adc/Adc.h"
#include "Hardware/RelayHandler/RelayHandler.h"

#include "BrightnessHandler.h"

#include <Comparators/SingleComparator.h>
#include <Mutex/ArmInterruptPreventionMutex.h>

namespace Hardware {

	#define CLOSE_RELAY  1
	#define OPEN_RELAY   2
	#define BRIGHT       CLOSE_RELAY



	BrightnessHandler::StateChangedCallbackDefinition BrightnessHandler::_stateChangedCallback = nullptr;
	BrightnessHandlerConfig                          *BrightnessHandler::_config = nullptr;
	Util::Comparators::SingleComparator<float>        BrightnessHandler::_comparator(0.0f, 0.0f, Util::Comparators::ComparatorState::Undefined);

	using namespace Util::Comparators;


	/** Internally used function. Calls, if possible, the brightness handler callback. */
	void BrightnessHandler::invokeCallback() {
		volatile const StateChangedCallbackDefinition callback = _stateChangedCallback;
		if ( callback != nullptr )   callback(RelayHandler::getRelayState());
	}

	void BrightnessHandler::init( BrightnessHandlerConfig *config, StateChangedCallbackDefinition stateChangedCallback ) {
		_config = config;
		_stateChangedCallback = stateChangedCallback;
		_comparator.CompareValue = config->CompareVoltage;
		_comparator.CompareHysteresis = config->HysteresisVoltage;
	}

	void BrightnessHandler::main() {
		if ( !Adc::isValidMeasurings() ) {
			return;
		}

		// Synchronize 'config' fields to comparator
		if ( _config->CompareVoltage != _comparator.CompareValue  ||  _config->HysteresisVoltage != _comparator.CompareHysteresis ) {
			Util::Mutex::ArmInterruptPreventionMutex mutex;  // --> Prevent interrupts so copy process will be performed "in one go"
			_comparator.CompareValue = _config->CompareVoltage;
			_comparator.CompareHysteresis = _config->HysteresisVoltage;
		}

		const bool comparatorStateChanged = _comparator.process(Adc::getFilteredMeasuring_PhotoVoltage());
		if ( comparatorStateChanged ) {
			const RelayState newRelayState =
			#if BRIGHT == CLOSE_RELAY
				(_comparator.getState() == ComparatorState::High) ? RelayState::Closed : RelayState::Open;
			#elif BRIGHT == OPEN_RELAY
				(_comparator.getState() == ComparatorState::High) ? RelayState::Open : RelayState::Closed;
			#endif
			RelayHandler::enqueueOpenCloseCommand( newRelayState, invokeCallback );
		}

	}

	void BrightnessHandler::setStateChangedCallback(BrightnessHandler::StateChangedCallbackDefinition stateChangedCallback) {
		_stateChangedCallback = stateChangedCallback;
	}

	Util::Comparators::ComparatorState BrightnessHandler::getComparatorState() {
		return _comparator.getState();
	}

} /* namespace Hardware */
