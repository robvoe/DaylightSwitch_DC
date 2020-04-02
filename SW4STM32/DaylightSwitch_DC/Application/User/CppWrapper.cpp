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


// Include generic utility modules
#include <Stm32/Persistence/PersistentStorage.h>

// Include project specific modules
#include "Hardware/Adc/Adc.h"


using namespace Hardware;
using namespace Util::Stm32::Persistence;

// Instanciate persistent storages
PersistentStorage<AdcConfig> adcConfig(AdcConfig::ResetData);



extern "C" {
	void doCpp(void) {
		// Init all necessary modules
		Adc::init(&adcConfig);


		while(1) {
			Adc::main();
		}
	}
}

