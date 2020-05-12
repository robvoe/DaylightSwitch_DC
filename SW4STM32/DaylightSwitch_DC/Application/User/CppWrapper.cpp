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
#include <Stm32/SoftTimer/SoftTimer.h>

// Include project specific modules
#include "Hardware/Adc/Adc.h"
#include "Hardware/RelayHandler/RelayHandler.h"
#include "Hardware/BrightnessHandler/BrightnessHandler.h"



using namespace Hardware;
using namespace Util::Stm32;
using namespace Util::Stm32::Persistence;
using namespace Util::Stm32::ButtonDriver;



// Instanciate persistent storages
PersistentStorage<AdcConfig>          adcConfig(AdcConfig::ResetData);
PersistentStorage<RelayHandlerConfig> relayHandlerConfig(RelayHandlerConfig::ResetData);
PersistentStorage<BrightnessHandlerConfig>   brightnessConfig(BrightnessHandlerConfig::ResetData);



void handleHoldImpulse(Button& button);
void handleButtonUp(Button& button);
void handleOpenRelayTimer();
void handleRelayChangeDueToBrightness(RelayState newRelayState);



#define OPEN_RELAY_TIMER_MINUTES  180 * (60 * 1000)
SoftTimer  openRelayTimer(OPEN_RELAY_TIMER_MINUTES, false, handleOpenRelayTimer);


extern "C" void doCpp(void) {
	// Perform software tests
	Util::Filters::Moving::MovingMedianFilterTest::performAllTests();
	Util::Filters::Moving::MovingAverageFilterTest::performAllTests();


	// Init all necessary modules
	Adc::init(&adcConfig);
	RelayHandler::init(&relayHandlerConfig);
	BrightnessHandler::init(&brightnessConfig, handleRelayChangeDueToBrightness);
	Button     button(TASTER_GPIO_Port, TASTER_Pin, GPIO_PinState::GPIO_PIN_SET, true, handleHoldImpulse, handleButtonUp);



	// Main loop
	while(1) {
		Adc::main();
		RelayHandler::main();
		BrightnessHandler::main();
		button.main();
		openRelayTimer.main();
	}
}


void handleHoldImpulse(Button& button) {
	if ( !button.State.isHoldImpulse() )  return;
	const float currentVoltage = Adc::getFilteredMeasuring_PhotoVoltage();
	const float hysteresis = brightnessConfig->HysteresisVoltage;
	brightnessConfig->CompareVoltage = currentVoltage + hysteresis / 2;
	brightnessConfig.saveToFlash();

	// Now give feedback by switching the relay several times
	RelayState relayState = RelayHandler::getRelayState();
	for ( uint32_t i = 0; i<4; i++ ) {
		relayState = !relayState;
		RelayHandler::enqueueOpenCloseCommand(relayState);
		RelayHandler::enqueueDelayCommand(600);
	}
}


void handleButtonUp(Button& button) {
	const RelayState newRelayState = !RelayHandler::getRelayState();
	RelayHandler::enqueueOpenCloseCommand(newRelayState);
}


void handleOpenRelayTimer() {
	openRelayTimer.disable();
	RelayHandler::enqueueOpenCommand();
}



void handleRelayChangeDueToBrightness(RelayState newRelayState) {
	if (newRelayState == RelayState::Closed)
		openRelayTimer.enable();
	else if (newRelayState == RelayState::Open)
		openRelayTimer.disable();
}
