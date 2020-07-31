/*
 * Adc.cpp
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
#include "Adc.h"

#include <IncludeStmHal.h>
#include <Stm32/SwoLogger/SwoLogger.h>

#include <Filters/Moving/MovingAverageFilter.h>


extern ADC_HandleTypeDef hadc1;


/// Internal flags
static bool finishedMeasuring;

namespace Hardware {
	using namespace Util::Filters::Moving;

	// Class fields
	AdcConfig *Adc::_adcConfig;

	volatile uint16_t Adc::_rawDigitsMeasurings[Adc::MeasuringChannelCount];

	uint32_t Adc::_collectedMeasurementsCount;

	float Adc::_uncorrectedMeasuring_RelayVoltage;
	float Adc::_uncorrectedMeasuring_SupplyVoltage;
	float Adc::_uncorrectedMeasuring_PhotoVoltage;
	float Adc::_correctedMeasuring_VDDA;
	float Adc::_correctedMeasuring_RelayVoltage;
	float Adc::_correctedMeasuring_SupplyVoltage;
	float Adc::_correctedMeasuring_PhotoVoltage;

	MovingAverageFilter<float, Adc::AverageFilterSize>  Adc::_averageFilter_RelayVoltage(0.0f);
	MovingAverageFilter<float, Adc::AverageFilterSize>  Adc::_averageFilter_SupplyVoltage(0.0f);
	MovingAverageFilter<float, Adc::AverageFilterSize>  Adc::_averageFilter_PhotoVoltage(0.0f);



	void Adc::init( AdcConfig *adcConfig ) {
		finishedMeasuring = false;
		_collectedMeasurementsCount = {0};

		Adc::_adcConfig = adcConfig;

		// Assign initial values
		_uncorrectedMeasuring_RelayVoltage = {0.0f};
		_uncorrectedMeasuring_SupplyVoltage = {0.0f};
		_uncorrectedMeasuring_PhotoVoltage = {0.0f};
		_correctedMeasuring_VDDA = {0.0f};
		_correctedMeasuring_RelayVoltage = {0.0f};
		_correctedMeasuring_SupplyVoltage = {0.0f};
		_correctedMeasuring_PhotoVoltage = {0.0f};

		// Calibrate the ADC
		HAL_Delay( 30 );
		HAL_StatusTypeDef result = HAL_ADCEx_Calibration_Start( &hadc1, ADC_SINGLE_ENDED );
		if ( result != HAL_OK )
			while (1);
	}


	void Adc::main(){
		// Error occurred? Reinitialize!
		if ( (hadc1.State & HAL_ADC_STATE_ERROR_INTERNAL) || (hadc1.State & HAL_ADC_STATE_ERROR_CONFIG) || (hadc1.State & HAL_ADC_STATE_ERROR_DMA) ) {
			HAL_ADC_Init( &hadc1 );
			finishedMeasuring = false;
		}

		if ( hadc1.State & HAL_ADC_STATE_READY ) {
			if ( finishedMeasuring == true ) {
				// Previous conversion finished? Process values..
				finishedMeasuring = false;

				// Determine VDDA
				_correctedMeasuring_VDDA = Adc::VREFINT_CALIBRATION__APPLIED_VOLTAGE * (float)(*(uint16_t*)VREFINT_CALIBRATION__MEASURED_DIGITS_ADDRESS) / (float)(_rawDigitsMeasurings[3]);
				const float voltsPerDigit = Adc::_correctedMeasuring_VDDA / 4096.0f;

				// Convert all measurings to physical equivalents (voltage/current)
				_uncorrectedMeasuring_RelayVoltage  = voltsPerDigit * Adc::_rawDigitsMeasurings[0] / 0.4125f;
				_uncorrectedMeasuring_SupplyVoltage = voltsPerDigit * Adc::_rawDigitsMeasurings[1] / 0.1304f;
				_uncorrectedMeasuring_PhotoVoltage  = voltsPerDigit * Adc::_rawDigitsMeasurings[2];

				// Apply linearization to the measurings
				_correctedMeasuring_RelayVoltage  = (_uncorrectedMeasuring_RelayVoltage * _adcConfig->RelayVoltageFactor) + _adcConfig->RelayVoltageOffset;
				_correctedMeasuring_SupplyVoltage = (_uncorrectedMeasuring_SupplyVoltage * _adcConfig->SupplyVoltageFactor) + _adcConfig->SupplyVoltageOffset;
				_correctedMeasuring_PhotoVoltage  = (_uncorrectedMeasuring_PhotoVoltage * _adcConfig->PhotoVoltageFactor) + _adcConfig->PhotoVoltageOffset;

				// Apply filtering to the measurings
				_averageFilter_RelayVoltage.process(_correctedMeasuring_RelayVoltage);
				_averageFilter_SupplyVoltage.process(_correctedMeasuring_SupplyVoltage);
				_averageFilter_PhotoVoltage.process(_correctedMeasuring_PhotoVoltage);

				_collectedMeasurementsCount++;
			}

			HAL_ADC_Start_DMA( &hadc1, (uint32_t*)_rawDigitsMeasurings, Adc::MeasuringChannelCount );

			// Debug outputs via SWO
			// TODO Perhaps, we want to remove that..
			if ( !_averageFilter_PhotoVoltage.containsEmptyElements() && !_averageFilter_SupplyVoltage.containsEmptyElements() ) {
				static uint32_t inhibit = {0};
				inhibit++;
				if ( inhibit >= 19 ) {
					inhibit = {0};
					using namespace Util::Stm32;
					SwoLogger::log("V_Photo ", _averageFilter_PhotoVoltage.getOutput());
					SwoLogger::log("V_Supply ", _averageFilter_SupplyVoltage.getOutput());
				}
			}
		}
	}


	/**
	 * Will be called by the STM HAL as soon as a conversion is complete.
	 */
	extern "C" void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* hadc ) {
		finishedMeasuring = true;
		HAL_ADC_Stop_DMA( &hadc1 );
	}

//	extern "C" {
//		void  HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
//			HAL_ADC_Stop_DMA( &hadc1 );
//			while(1);
//		}
//	}


	void Adc::flushFilters( uint32_t timeout ) {
		const uint32_t startedAtTime = HAL_GetTick();
		const uint32_t startedAtMeasurementsCount = _collectedMeasurementsCount;
		while ( true ) {
			Adc::main();
			if ( timeout && (HAL_GetTick() - startedAtTime) > timeout ) return;
			if ( (_collectedMeasurementsCount - startedAtMeasurementsCount) > AverageFilterSize ) return;
		}
	}

	bool Adc::isValidMeasurings() {
		return !_averageFilter_SupplyVoltage.containsEmptyElements();  // We could have chosen any of the other filters here instead..
	}


	float Adc::getFilteredMeasuring_RelayVoltage() {
		return _averageFilter_RelayVoltage.getOutput();
	}

	float Adc::getFilteredMeasuring_SupplyVoltage() {
		return _averageFilter_SupplyVoltage.getOutput();
	}

	float Adc::getFilteredMeasuring_PhotoVoltage() {
		return _averageFilter_PhotoVoltage.getOutput();
	}

} /*namespace Hardware*/
