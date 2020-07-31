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

namespace Apps {
	
	/**
	 * TODO
	 */
	class DaytimesClosed: public AppBase {
			
		public:
			/******************************** INTERNAL TYPES *******************************/

			/********************************** CONSTANTS **********************************/

			static constexpr uint32_t  OpenRelayTimerMillis = 30 /*minutes*/ * (60 * 1000);
			static constexpr bool      UseOpenRelayTimer = false;

			/*********************************** FIELDS ************************************/

			Util::Stm32::SoftTimer _openRelayTimer;

			/********************************* CONSTRUCTORS ********************************/

			DaytimesClosed(AppBaseConfig &appBaseConfig);
			
			DaytimesClosed( const DaytimesClosed& other ) = delete;  // Copy constructor
			DaytimesClosed( const DaytimesClosed&& other ) = delete; // Move constructor
			
			/******************************** GETTERS/SETTERS ******************************/
			
			/********************************* GENERAL LOGIC *******************************/

			void handleBrightnessComparatorEvent( Util::Comparators::ComparatorState newComparatorState ) override;
			void _main() override;


			virtual ~DaytimesClosed() {}
	};

} /* namespace Apps */

#endif /* APPLICATION_USER_APPS_DAYTIMESCLOSED_H_ */
