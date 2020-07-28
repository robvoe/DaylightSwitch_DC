/*
 * BaseApp.h
 *
 *  Created on: 24.05.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *    Defines the application base.
 */
#ifndef APPLICATION_USER_APPS_APPBASE_H_
#define APPLICATION_USER_APPS_APPBASE_H_

#include <string>

#include <Comparators/ComparatorBase.h>
#include <Stm32/SwoLogger/SwoLogger.h>

#include "Hardware/RelayHandler/RelayHandler.h"
#include "Hardware/RelayHandler/Definitions.h"


namespace Apps {
	
	/**
	 * Defines the application base. Cannot be instantiated.
	 */
	class AppBase {
		protected:
			/********************************* CONSTRUCTORS ********************************/
			
			/**
			 * Constructor. Protected modifier prevents from being instantiated.
			 */
			AppBase() {};

			/********************************* GENERAL LOGIC *******************************/
		public:

			virtual void main() {}

			virtual void handleButtonUp() {
				using namespace Hardware;
				const RelayState newRelayState = !RelayHandler::getRelayState();
				RelayHandler::enqueueOpenCloseCommand(newRelayState);
				Stm32::SwoLogger::info("Button up event");
			}

			virtual void handleBrightnessComparatorEvent( Util::Comparators::ComparatorState newComparatorState ) = 0;

			/**
			 * Destructor.
			 */
			virtual ~AppBase() {}

	};

} /* namespace Apps */

#endif /* APPLICATION_USER_APPS_APPBASE_H_ */
