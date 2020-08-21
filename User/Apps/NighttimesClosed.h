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
#ifndef APPLICATION_USER_APPS_NIGHTTIMESCLOSED_H_
#define APPLICATION_USER_APPS_NIGHTTIMESCLOSED_H_

#include "AppBase.h"

#include <Stm32/SoftTimer/SoftTimer.h>

#include "Hardware/RelayHandler/RelayHandler.h"

namespace Apps {
	
	/**
	 * TODO
	 */
	class NighttimesClosed: public AppBase {
			
		public:
			/******************************** INTERNAL TYPES *******************************/

			/********************************** CONSTANTS **********************************/

			static constexpr uint32_t  OpenRelayTimerMillis = 120 /*minutes*/ * (60 * 1000);
			static constexpr bool      UseOpenRelayTimer = true;


			/********************************* CONSTRUCTORS ********************************/

			NighttimesClosed(AppBaseConfig &appBaseConfig);
			
			NighttimesClosed( const NighttimesClosed& other ) = delete;  // Copy constructor
			NighttimesClosed( const NighttimesClosed&& other ) = delete; // Move constructor
			
			/******************************** GETTERS/SETTERS ******************************/
			

			/******************************** OVERRIDDEN LOGIC *****************************/

			void handleBrightnessComparatorEvent( Util::Comparators::ComparatorState newComparatorState ) override;
			void _main() override;
			Hardware::RelayState handleButtonUp() override;



			virtual ~NighttimesClosed() {}

		private:
			/*********************************** FIELDS ************************************/

			Util::Stm32::SoftTimer _openRelayTimer;

			/********************************* GENERAL LOGIC *******************************/

			void handleTimer(Hardware::RelayState newRelayState);

	};

} /* namespace Apps */

#endif /* APPLICATION_USER_APPS_NIGHTTIMESCLOSED_H_ */
