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

#include "main.h" // --> Includes various definitions (e.g. button port definitions)


// Include software tests
#include <Filters/Moving/Test/MedianFilterTest.h>
#include <Filters/Moving/Test/AverageFilterTest.h>


// Include generic utility modules
#include <Stm32/Persistence/PersistentStorage.h>
#include <Stm32/ButtonDriver/Button.h>

// Include project specific modules
#include "Hardware/Adc/Adc.h"
#include "Hardware/RelayHandler/RelayHandler.h"
#include "Hardware/BrightnessHandler/BrightnessHandler.h"



using namespace Hardware;
using namespace Util::Stm32::Persistence;
using namespace Util::Stm32::ButtonDriver;


// Instanciate persistent storages
PersistentStorage<AdcConfig>          adcConfig(AdcConfig::ResetData);
PersistentStorage<RelayHandlerConfig> relayHandlerConfig(RelayHandlerConfig::ResetData);
PersistentStorage<BrightnessHandlerConfig>   brightnessConfig(BrightnessHandlerConfig::ResetData);


//void doClose();
//void doOpen() {
//	RelayHandler::enqueueOpenCommand(doClose);
//	RelayHandler::enqueueDelayCommand(800);
//}
//void doClose() {
//	RelayHandler::enqueueCloseCommand(doOpen);
//	RelayHandler::enqueueDelayCommand(800);
//}




extern "C" void doCpp(void) {
	// Perform software tests
	Util::Filters::Moving::MovingMedianFilterTest::performAllTests();
	Util::Filters::Moving::MovingAverageFilterTest::performAllTests();


	// Init all necessary modules
	Adc::init(&adcConfig);
	RelayHandler::init(&relayHandlerConfig);
	BrightnessHandler::init(&brightnessConfig);
	Button  button(TASTER_GPIO_Port, TASTER_Pin, GPIO_PinState::GPIO_PIN_SET, true, [](Button& button){
		/* ButtonDown handler */
		if ( !button.State.isHoldImpulse() )  return;
		const float currentVoltage = Adc::getFilteredMeasuring_PhotoVoltage();
		const float hysteresis = brightnessConfig->HysteresisVoltage;
		brightnessConfig->CompareVoltage = currentVoltage + hysteresis / 2;
		brightnessConfig.saveToFlash();
	}, [](Button& button){
		/* ButtonUp handler */
		const RelayState newRelayState = (RelayHandler::getRelayState() == RelayState::Closed) ? RelayState::Open : RelayState::Closed;
		if ( newRelayState == RelayState::Closed )
			RelayHandler::enqueueCloseCommand();
		else /*if ( newRelayState == RelayState::Open )*/
			RelayHandler::enqueueOpenCommand();
	});
	//Util::Comparators::RisingComparator<float> brightnessComparator(brightnessConfig->CompareVoltage, brightnessConfig->HysteresisVoltage, Util::Comparators::ComparatorState::Undefined);


	// Main loop
	while(1) {
		Adc::main();
		RelayHandler::main();
		BrightnessHandler::main();
		button.main();


//		if ( Adc::isValidMeasurings()  &&  brightnessComparator.process(Adc::getFilteredMeasuring_PhotoVoltage()) ) {
//			// The comparator state changed, handle the event..
//			const Util::Comparators::State state = brightnessComparator.getState();
//			if ( state == Util::Comparators::State::High  &&  RelayHandler::getRelayState() != RelayState::Closed )
//				RelayHandler::enqueueCloseCommand();
//			else if ( state == Util::Comparators::State::Low  &&  RelayHandler::getRelayState() != RelayState::Open )
//				RelayHandler::enqueueOpenCommand();
//		}
	}
}

