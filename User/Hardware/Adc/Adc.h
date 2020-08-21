/*
 * Adc.h
 *
 *  Created on: 30.03.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *    Handles ADC readings and corrects/linearizes/filters the measurements.
 *
 *    Futher information:
 *    - before using, static class 'Adc' MUST be initialized using init()
 */
#ifndef APPLICATION_USER_ADC_ADC_H_
#define APPLICATION_USER_ADC_ADC_H_

#include <stdint-gcc.h>
#include "AdcConfig.h"

#include <Filters/Moving/MovingAverageFilter.h>


namespace Hardware {

	class Adc {

		private:
			Adc() = delete;
			Adc( const Adc& ) = delete;

			/// Calibration parameters necessary for determining VDDA.
			static const uint32_t VREFINT_CALIBRATION__MEASURED_DIGITS_ADDRESS { UINT32_C( 0x1FFF75AA ) };
			static constexpr float VREFINT_CALIBRATION__APPLIED_VOLTAGE { 3.0f };

			/// Various fields
			static AdcConfig *_adcConfig;
			static uint32_t   _collectedMeasurementsCount;      	///< Counts performed measurements. IMPORTANT: Overflow must be considered when using this value!


			/// Raw measuring results
			static constexpr uint32_t MeasuringChannelCount { 4 };  	///< Channels:   Relay voltage, Supply voltage, Photo voltage, Internal voltage reference
			static volatile uint16_t _rawDigitsMeasurings[MeasuringChannelCount];

			/// Measurings that were already converted to physical units, but no correction calculations applied yet.
			static float _uncorrectedMeasuring_RelayVoltage;      	///< Holds the voltage of the relay voltage rail (typically approx. 7..8 volts)
			static float _uncorrectedMeasuring_SupplyVoltage;     	///< Holds the overall supply voltage. Should be somthing around either 12V or 24V.
			static float _uncorrectedMeasuring_PhotoVoltage;      	///< Photo input voltage.

			/// Measurings that were already converted to physical units. No further corrections necessary.
			static float _correctedMeasuring_VDDA;
			static float _correctedMeasuring_RelayVoltage;        	///< Holds the voltage of the relay voltage rail (typically approx. 7..8 volts)
			static float _correctedMeasuring_SupplyVoltage;        	///< Holds the overall supply voltage. Should be somthing around either 12V or 24V.
			static float _correctedMeasuring_PhotoVoltage;         	///< Photo input voltage.

			/// Average filter instances
			static constexpr uint32_t AverageFilterSize { 16 };
			static Util::Filters::Moving::MovingAverageFilter<float, AverageFilterSize>  _averageFilter_RelayVoltage;
			static Util::Filters::Moving::MovingAverageFilter<float, AverageFilterSize>  _averageFilter_SupplyVoltage;
			static Util::Filters::Moving::MovingAverageFilter<float, AverageFilterSize>  _averageFilter_PhotoVoltage;


		public:

			/**
			 * Initializes the ADC conversion module.
			 *
			 * @param adcConfig ..	Contains the ADC configuration. Must not be null!
			 */
			static void init( AdcConfig *adcConfig );

			/**
			 * Main function of the ADC module. Must be called cyclically from main loop.
			 */
			static void main();

			/**
			 * Waits until the filters are filled-up with completely new data.
			 *
			 * @param timeout  ..  The maximum time [ms] this function is allowed to block. A value of 0 disables the timeout; the function may block indefinitely.
			 */
			static void flushFilters( uint32_t timeout = UINT32_C( 400 ) );

			/**
			 * Returns if there are already valid measurings present. Will only return 'false' right after initializing this module.
			 */
			static bool isValidMeasurings();

			/**
			 * Invalidates all measurings until old filter values were overwritten by new ones.
			 */
			static void invalidateMeasurings();


			/// Getter functions for corrected measurings
			static float getFilteredMeasuring_RelayVoltage();
			static float getFilteredMeasuring_SupplyVoltage();
			static float getFilteredMeasuring_PhotoVoltage();


	};

} /*namespace Hardware*/

#endif /* APPLICATION_USER_ADC_ADC_H_ */
