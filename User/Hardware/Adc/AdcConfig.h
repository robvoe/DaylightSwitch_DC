/*
 * AdcConfig.h
 *
 *  Created on: 30.03.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *  	Defines the user configuration of the Adc module. It will be managed by the Persistence module during
 *  	runtime ─ thus, as initiated by the application, all values are stored in Flash memory.
 */
#ifndef APPLICATION_USER_ADC_ADCCONFIG_H_
#define APPLICATION_USER_ADC_ADCCONFIG_H_


#include <stdint-gcc.h>
#include <type_traits>

namespace Hardware {

	/**
	 * This class contains correction data for ADC measurements.
	 *
	 * The typically performed calculation for retrieving corrected measurements is:
	 *   CorrectedVal = (RawVal * FACTOR) + OFFSET
	 */
	struct AdcConfig {
		public:
			/// Photo voltage
			float PhotoVoltageFactor;
			float PhotoVoltageOffset;

			/// Relay voltage
			float RelayVoltageFactor;
			float RelayVoltageOffset;

			/// External supply voltage
			float SupplyVoltageFactor;
			float SupplyVoltageOffset;



			/**
			 * Resets all config fields to defaults.
			 */
			static void ResetData(AdcConfig &instance) {
				instance.PhotoVoltageFactor = 1.0f;
				instance.PhotoVoltageOffset = 0.0f;

				instance.RelayVoltageFactor = 1.0f;
				instance.RelayVoltageOffset = 0.0f;

				instance.SupplyVoltageFactor = 1.0f;
				instance.SupplyVoltageOffset = 0.0f;
			}
	};

	static_assert(std::is_pod<AdcConfig>::value, "AdcConfig must be POD!");

} /*namespace Hardware*/

#endif /* APPLICATION_USER_ADC_ADCCONFIG_H_ */
