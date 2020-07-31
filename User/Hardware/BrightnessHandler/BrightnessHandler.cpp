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

	BrightnessHandler::BrightnessComparatorEventCallbackDefinition BrightnessHandler::_brightnessComparatorEventCallback = nullptr;
//	BrightnessHandlerConfig                                       *BrightnessHandler::_config = nullptr;


	using namespace Util::Comparators;


//	void BrightnessHandler::init( BrightnessHandlerConfig *config, BrightnessComparatorEventCallbackDefinition brightnessComparatorEventCallback ) {
//		_config = config;
//		_brightnessComparatorEventCallback = brightnessComparatorEventCallback;
//		_comparator.CompareValue = config->CompareVoltage;
//		_comparator.CompareHysteresis = config->HysteresisVoltage;
//	}
//
//	void BrightnessHandler::main() {
//		if ( !Adc::isValidMeasurings() ) {
//			return;
//		}
//
//		// Synchronize 'config' fields to comparator
//		if ( _config->CompareVoltage != _comparator.CompareValue  ||  _config->HysteresisVoltage != _comparator.CompareHysteresis ) {
//			Util::Mutex::ArmInterruptPreventionMutex mutex;  // --> Prevent interrupts so copy process will be performed "in one go"
//			_comparator.CompareValue = _config->CompareVoltage;
//			_comparator.CompareHysteresis = _config->HysteresisVoltage;
//		}
//
//		const bool comparatorStateChanged = _comparator.process(Adc::getFilteredMeasuring_PhotoVoltage());
//		if ( comparatorStateChanged ) {
//			volatile BrightnessComparatorEventCallbackDefinition callback = _brightnessComparatorEventCallback;
//			if ( callback != nullptr )  callback( _comparator.getState() );
//		}
//
//	}
//
//	void BrightnessHandler::setBrightnessComparatorEventCallback(BrightnessHandler::BrightnessComparatorEventCallbackDefinition brightnessComparatorEventCallback) {
//		_brightnessComparatorEventCallback = brightnessComparatorEventCallback;
//	}
//
//	Util::Comparators::ComparatorState BrightnessHandler::getComparatorState() {
//		return _comparator.getState();
//	}

} /* namespace Hardware */
