/*
 * BaseApp.cpp
 *
 *  Created on: 24.05.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *    Defines the application base.
 */

#include "AppBase.h"

#include <Stm32/SwoLogger/SwoLogger.h>

namespace Apps {

		AppBase::AppBase(AppBaseConfig &appBaseConfig) :
			_config(appBaseConfig),
			_comparator(appBaseConfig.CompareVoltage, appBaseConfig.HysteresisVoltage, Util::Comparators::ComparatorState::Undefined)
		{
			// Nothing happens here..
		};

		void AppBase::main() {
			_main();

			if ( !Hardware::Adc::isValidMeasurings() )  return;

			// Synchronize 'config' fields to comparator
			if ( _config.CompareVoltage != _comparator.CompareValue  ||  _config.HysteresisVoltage != _comparator.CompareHysteresis ) {
				Util::Mutex::ArmInterruptPreventionMutex mutex;  // --> Prevent interrupts so copy process will be performed "in one go"
				_comparator.CompareValue = _config.CompareVoltage;
				_comparator.CompareHysteresis = _config.HysteresisVoltage;
			}

			const bool comparatorStateChanged = _comparator.process(Hardware::Adc::getFilteredMeasuring_PhotoVoltage());
			if ( comparatorStateChanged ) {
				Util::Stm32::SwoLogger::log("BrightnessComparator event");
				handleBrightnessComparatorEvent(_comparator.getState());
			}
		}

		void AppBase::handleButtonUp() {
			using namespace Hardware;
			using namespace Util::Stm32;
			const RelayState newRelayState = !RelayHandler::getRelayState();
			RelayHandler::enqueueOpenCloseCommand(newRelayState);
			SwoLogger::log("Button up event");
		}


} /* namespace Apps */

