/*
 * DaytimesClosed.h
 *
 *  Created on: 24.05.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *    Defines the following switching scheme:
 *      - relay is closed high brightness levels / open at low brightness levels
 *      -
 */
#ifndef APPLICATION_USER_APPS_DAYTIMESCLOSED_H_
#define APPLICATION_USER_APPS_DAYTIMESCLOSED_H_

#include "AppBase.h"

#include <Stm32/SoftTimer/SoftTimer.h>

#include "Hardware/RelayHandler/RelayHandler.h"

namespace Apps {
	
	/**
	 * TODO
	 */
	class DaytimesClosed: public AppBase {
			
		public:
			/******************************** INTERNAL TYPES *******************************/

			/********************************** CONSTANTS **********************************/

			static constexpr uint32_t  OpenRelayTimerMillis_Bright = 120 /*minutes*/ * (60 * 1000);  ///< This timer gets enabled by BrightnessComparatorEvent or by ButtonEvent
			static constexpr uint32_t  OpenRelayTimerMillis_Dark   =  30 /*minutes*/ * (60 * 1000);  ///< This timer gets enabled by ButtonEvent
			static constexpr bool      UseOpenRelayTimer_Bright = true;
			static constexpr bool      UseOpenRelayTimer_Dark   = true;


			/********************************* CONSTRUCTORS ********************************/

			DaytimesClosed(AppBaseConfig &appBaseConfig);
			
			DaytimesClosed( const DaytimesClosed& other ) = delete;  // Copy constructor
			DaytimesClosed( const DaytimesClosed&& other ) = delete; // Move constructor
			
			/******************************** GETTERS/SETTERS ******************************/
			

			/******************************** OVERRIDDEN LOGIC *****************************/

			void handleBrightnessComparatorEvent( Util::Comparators::ComparatorState newComparatorState ) override;
			void _main() override;
			Hardware::RelayState handleButtonUp() override;



			virtual ~DaytimesClosed() {}

		private:
			/*********************************** FIELDS ************************************/

			Util::Stm32::SoftTimer _openRelayTimer_Bright;
			Util::Stm32::SoftTimer _openRelayTimer_Dark;

			/********************************* GENERAL LOGIC *******************************/

//			enum class EventType {
//				ButtonUp, BrightnessComparator
//			};

			void handleTimers(Hardware::RelayState newRelayState);

	};

} /* namespace Apps */

#endif /* APPLICATION_USER_APPS_DAYTIMESCLOSED_H_ */
