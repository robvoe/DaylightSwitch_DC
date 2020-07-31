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


#include <Comparators/SingleComparator.h>
#include <Stm32/SwoLogger/SwoLogger.h>
#include <Mutex/ArmInterruptPreventionMutex.h>

#include "Hardware/RelayHandler/RelayHandler.h"
#include "Hardware/RelayHandler/Definitions.h"
#include "Hardware/Adc/Adc.h"

#include "AppBaseConfig.h"


namespace Apps {
	
	/**
	 * Defines the application base. Cannot be instantiated.
	 */
	class AppBase {
		private:
			AppBaseConfig&                              _config;
			Util::Comparators::SingleComparator<float>  _comparator;



		protected:
			/********************************* CONSTRUCTORS ********************************/
			
			/**
			 * Constructor. Protected modifier prevents from being instantiated.
			 */
			AppBase(AppBaseConfig &appBaseConfig);

			/********************************* GENERAL LOGIC *******************************/

			/** Gets called as soon as the brightness comparator changes its state. Must be implemented by the application. */
			virtual void handleBrightnessComparatorEvent( Util::Comparators::ComparatorState newComparatorState ) = 0;

		public:

			/** Must be called from main-loop. */
			void main();

			/** May be implemented by the application so it gets called cyclically. */
			virtual void _main() {}

			/** Must be called from "outside" in case the button was released. */
			virtual void handleButtonUp();


			/** Destructor */
			virtual ~AppBase() {}

	};

} /* namespace Apps */

#endif /* APPLICATION_USER_APPS_APPBASE_H_ */
