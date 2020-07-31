/*
 * AppBase.h
 *
 *  Created on: 06.04.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *  	Defines the user configuration for switching thresholds. It will be managed by the Persistence module during
 *  	runtime ─ thus, as initiated by the application, all values are stored in Flash memory.
 */
#ifndef APPLICATION_USER_APP_BASE_CONFIG_H_
#define APPLICATION_USER_APP_BASE_CONFIG_H_


#include <stdint-gcc.h>
#include <type_traits>



struct AppBaseConfig {
		static constexpr float InitialCompareVoltage = 0.6f;
		static constexpr float InitialHysteresisVoltage = 0.2f;

		static constexpr float MinSwitchBackVoltage = 0.05f;  // Should be slightly higher than the OpAmp's lower rail voltage.
		static constexpr float MaxCompareVoltage = 3.25f;

	public:
		float CompareVoltage;
		float HysteresisVoltage;



		/**
		 * Resets all config fields to defaults.
		 */
		static void ResetData(AppBaseConfig &instance) {
			instance.CompareVoltage = InitialCompareVoltage;
			instance.HysteresisVoltage = InitialHysteresisVoltage;
		}

		/**
		 * Deduces and sets the field @see CompareVoltage
		 *
		 * @param currentVoltage  ..  Contains the currently present (brightness) voltage.
		 */
		void deduceCompareVoltage(float currentVoltage) {
			CompareVoltage = currentVoltage + HysteresisVoltage / 2.0f;

			// Check if "switch back" voltage is too low
			const float switchBackVoltage = CompareVoltage - HysteresisVoltage;
			if (switchBackVoltage < MinSwitchBackVoltage) {
				const float delta = MinSwitchBackVoltage - switchBackVoltage;
				CompareVoltage += delta;  // Shift CompareVoltage up by the delta so it is always possible to switch back
			}

			// Check if CompareVoltage is too high
			if (CompareVoltage > MaxCompareVoltage) {
				CompareVoltage = MaxCompareVoltage;
			}
		}
};

static_assert(std::is_pod<AppBaseConfig>::value, "AppBaseConfig must be POD!");



#endif /* APPLICATION_USER_ADC_ADCCONFIG_H_ */
