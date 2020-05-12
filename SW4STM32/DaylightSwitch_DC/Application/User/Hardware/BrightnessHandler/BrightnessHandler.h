/*
 * BrightnessComparator.h
 *
 *  Created on: 06.04.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *  	TODO
 */
#ifndef APPLICATION_USER_HARDWARE_BRIGHTNESSHANDLER_BRIGHTNESSHANDLER_H_
#define APPLICATION_USER_HARDWARE_BRIGHTNESSHANDLER_BRIGHTNESSHANDLER_H_

#include "BrightnessHandlerConfig.h"

#include "../RelayHandler/Definitions.h"

#include <Comparators/RisingComparator.h>

namespace Hardware {
	
	class BrightnessHandler {
		public:
			typedef void (*StateChangedCallbackDefinition)(RelayState newState);

		private:
			BrightnessHandler() = delete;
			BrightnessHandler(const BrightnessHandler&) = delete;

			static StateChangedCallbackDefinition              _stateChangedCallback;
			static BrightnessHandlerConfig                    *_config;
			static Util::Comparators::RisingComparator<float>  _comparator;

			/**
			 * Internally used function. Calls, if possible, the 'brightness changed' callback.
			 */
			static void invokeCallback();

		public:

			/**
			 * Initializes the BrightnessHandler module.
			 *
			 * @param config                ..  Contains the configuration. Must not be null!
			 * @param stateChangedCallback  ..  May hold a callback that gets called as soon as the relay state changes due to change in brightness. May be null.
			 */
			static void init( BrightnessHandlerConfig *config, StateChangedCallbackDefinition stateChangedCallback = nullptr );

			/**
			 * Main function of the BrightnessHandler module. Must be called cyclically from main loop.
			 */
			static void main();

			/**
			 * Assigns the StateChangedCallback which will be called as soon as the relay state changes due to change in brightness.
			 *
			 * @remarks The callback will be called as soon as the relay state ACTUALLY CHANGED.
			 *
			 * @param stateChangedCallback	..	The callback function. May be null.
			 */
			static void setStateChangedCallback(StateChangedCallbackDefinition stateChangedCallback);

			/**
			 * Returns the brightness comparator state.
			 *
			 * In case there is no valid state yet, this function returns ComparatorState::Undefined. This
			 * may happen right after reboot.
			 */
			static Util::Comparators::ComparatorState getComparatorState();


	};

} /* namespace Hardware */

#endif /* APPLICATION_USER_HARDWARE_BRIGHTNESSHANDLER_BRIGHTNESSHANDLER_H_ */
