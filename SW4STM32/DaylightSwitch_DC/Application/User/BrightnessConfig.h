/*
 * BrightnessConfig.h
 *
 *  Created on: 03.04.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *  	Defines the user configuration of the Brightness comparator. It will be managed by the Persistence module during
 *  	runtime ─ thus, as initiated by the application, all values are stored in Flash memory.
 */
#ifndef APPLICATION_USER_BRIGHTNESS_CONFIG_H_
#define APPLICATION_USER_BRIGHTNESS_CONFIG_H_


#include <stdint-gcc.h>
#include <type_traits>


struct BrightnessConfig {
		static constexpr float InitialCompareVoltage = 0.15f;
		static constexpr float InitialHysteresisVoltage = 0.1f;

	public:
		float CompareVoltage;
		float HysteresisVoltage;



		/**
		 * Resets all config fields to defaults.
		 */
		static void ResetData(BrightnessConfig &instance) {
			instance.CompareVoltage = InitialCompareVoltage;
			instance.HysteresisVoltage = InitialHysteresisVoltage;
		}
};

static_assert(std::is_pod<BrightnessConfig>::value, "BrightnessConfig must be POD!");


#endif /* APPLICATION_USER_ADC_ADCCONFIG_H_ */
