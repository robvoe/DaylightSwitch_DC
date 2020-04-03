/*
 * CppWrapper.cpp
 *
 *  Created on: 29.03.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *    Wraps all C++ functionality to make it accessible from main.c
 */

#include <IncludeStmHal.h>


// Include software tests
#include <Filters/Moving/Test/MedianFilterTest.h>
#include <Filters/Moving/Test/AverageFilterTest.h>


// Include generic utility modules
#include <Stm32/Persistence/PersistentStorage.h>
#include <Comparators/RisingComparator.h> // TODO


// Include project specific modules
#include "Hardware/Adc/Adc.h"
#include "Hardware/RelayHandler/RelayHandler.h"
#include "BrightnessConfig.h"



using namespace Hardware;
using namespace Util::Stm32::Persistence;


// Instanciate persistent storages
PersistentStorage<AdcConfig>          adcConfig(AdcConfig::ResetData);
PersistentStorage<RelayHandlerConfig> relayHandlerConfig(RelayHandlerConfig::ResetData);
PersistentStorage<BrightnessConfig>   brightnessConfig(BrightnessConfig::ResetData);




extern "C" void doCpp(void) {
	// Perform software tests
	Util::Filters::Moving::MovingMedianFilterTest::performAllTests();
	Util::Filters::Moving::MovingAverageFilterTest::performAllTests();


	// Init all necessary modules
	Adc::init(&adcConfig);
	RelayHandler::init(&relayHandlerConfig);
	Util::Comparators::RisingComparator<float> brightnessComparator(brightnessConfig->CompareVoltage, brightnessConfig->HysteresisVoltage, Util::Comparators::State::Undefined);

	// Main loop
	while(1) {
		Adc::main();
		RelayHandler::main();

		if ( Adc::isValidMeasurings()  &&  brightnessComparator.process(Adc::getFilteredMeasuring_PhotoVoltage()) ) {
			// The comparator state changed, handle the event..
			const Util::Comparators::State state = brightnessComparator.getState();
			if ( state == Util::Comparators::State::High  &&  RelayHandler::getRelayState() != RelayState::Closed )
				RelayHandler::enqueueCloseCommand();
			else if ( state == Util::Comparators::State::Low  &&  RelayHandler::getRelayState() != RelayState::Open )
				RelayHandler::enqueueOpenCommand();
		}
	}
}

