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

#include <Comparators/SingleComparator.h>
#include "BrightnessHandlerConfig.h"

#include "../RelayHandler/Definitions.h"


namespace Hardware {
	
	class BrightnessHandler {
		public:
			//typedef void (*StateChangedCallbackDefinition)(RelayState newState);
			typedef void (*BrightnessComparatorEventCallbackDefinition)(Util::Comparators::ComparatorState);

		private:
			BrightnessHandler() = delete;
			BrightnessHandler(const BrightnessHandler&) = delete;

			static BrightnessComparatorEventCallbackDefinition  _brightnessComparatorEventCallback;
			static BrightnessHandlerConfig                     *_config;
			static Util::Comparators::SingleComparator<float>   _comparator;

			/**
			 * Internally used function. Calls, if possible, the 'brightness changed' callback.
			 */
			static void invokeCallback();

		public:

			/**
			 * Initializes the BrightnessHandler module.
			 *
			 * @param config                             ..  Contains the configuration. Must not be null!
			 * @param brightnessComparatorEventCallback  ..  May hold a callback that gets called as soon as the brightness comparator output changes. May be nullptr.
			 */
			static void init( BrightnessHandlerConfig *config, BrightnessComparatorEventCallbackDefinition brightnessComparatorEventCallback = nullptr );

			/**
			 * Main function of the BrightnessComparatorHandler module. Must be called cyclically from main loop.
			 */
			static void main();

			/**
			 * Assigns the BrightnessComparatorEventCallback which will be called as soon as the brightness comparator output changes.
			 *
			 * @param brightnessComparatorEventCallback	..	The callback function. May be null.
			 */
			static void setBrightnessComparatorEventCallback(BrightnessComparatorEventCallbackDefinition brightnessComparatorEventCallback);

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
