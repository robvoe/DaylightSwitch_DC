/*
 * RelayHandler.cpp
 *
 *  Created on: 03.04.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *  	Handles the bi-stable relay and offers a queue of (open/close/delay) commands to the application.
 */

#include "RelayHandler.h"

#include <Hardware/Adc/Adc.h>

#include <IncludeStmHal.h>
#include <main.h>                    // Necessary for the pin/port constants


namespace Hardware {

	RelayHandlerConfig                                           *RelayHandler::_relayHandlerConfig;
	Util::Lists::StaticMemory::Fifo<RelayHandler::RelayCommand>   RelayHandler::_commandFifo;
	RelayState                                                    RelayHandler::_currentRelayState;

	void RelayHandler::init(RelayHandlerConfig *relayHandlerConfig) {
		_relayHandlerConfig = relayHandlerConfig;
		_currentRelayState = RelayState::Indeterminate;

		if ( relayHandlerConfig->InitialState == RelayState::Closed )
			enqueueCloseCommand();
		else if ( relayHandlerConfig->InitialState == RelayState::Open )
			enqueueOpenCommand();
	}


	void RelayHandler::main() {
		static uint32_t lastCommandExecutedAt = 0;
		if ( _commandFifo.isEmpty() )  return;

		const auto postprocessRemoveCommand = []{
			RelayCommand cmd;  _commandFifo.dequeue(&cmd);
			if ( cmd.CommandProcessedCallback != nullptr )  cmd.CommandProcessedCallback();
			lastCommandExecutedAt = HAL_GetTick();
		};
		RelayCommand *nextCommand = _commandFifo.peek();

		// Excecute Delay command
		if ( nextCommand->Command == RelayCommand::Command::Delay ) {
			const uint32_t millisPassed = HAL_GetTick() - lastCommandExecutedAt;
			if ( millisPassed >= nextCommand->DelayDurationMillis )
				postprocessRemoveCommand();
		}

		// Handle execution of Open/Close command
		else if ( nextCommand->Command == RelayCommand::Command::Open  ||  nextCommand->Command == RelayCommand::Command::Close ) {
			// Currently no command being executed? Then start it!
			if ( !nextCommand->ExecutionDetails.isExecuting ) {
				if (  (_currentRelayState == RelayState::Open  &&  nextCommand->Command == RelayCommand::Command::Open)  ||
					  (_currentRelayState == RelayState::Closed  &&  nextCommand->Command == RelayCommand::Command::Close)  ) {
					postprocessRemoveCommand();
					return;
				}
				if ( Adc::isValidMeasurings()  &&  Adc::getFilteredMeasuring_RelayVoltage() >= MinimumVoltage ) {
					if ( nextCommand->Command == RelayCommand::Command::Open )
						HAL_GPIO_WritePin( RELAY_OPEN_GPIO_Port, RELAY_OPEN_Pin, GPIO_PIN_SET );
					else if ( nextCommand->Command == RelayCommand::Command::Close )
						HAL_GPIO_WritePin( RELAY_CLOSE_GPIO_Port, RELAY_CLOSE_Pin, GPIO_PIN_SET );
					nextCommand->ExecutionDetails.isExecuting = true;
					nextCommand->ExecutionDetails.StartedExecutionAt = HAL_GetTick();
				}
			}

			// Currently a command running? Finish execution if necessary!
			if ( nextCommand->ExecutionDetails.isExecuting ) {
				const uint32_t duration = HAL_GetTick()-nextCommand->ExecutionDetails.StartedExecutionAt;
				if ( duration >=  CurrentFlowMillis ) {
					HAL_GPIO_WritePin( RELAY_CLOSE_GPIO_Port, RELAY_CLOSE_Pin, GPIO_PIN_RESET );
					HAL_GPIO_WritePin( RELAY_OPEN_GPIO_Port, RELAY_OPEN_Pin, GPIO_PIN_RESET );
					_currentRelayState = (nextCommand->Command == RelayCommand::Command::Open) ? RelayState::Open : RelayState::Closed;
					postprocessRemoveCommand();
				}
			}
		}
	}

	void RelayHandler::enqueueOpenCommand(CommandProcessedCallbackDefinition commandProcessedCallback) {
		const RelayCommand cmd{
			RelayCommand::Command::Open,
			0,
			commandProcessedCallback,
			{false, 0}
		};
		_commandFifo.enqueue( cmd );
	}

	void RelayHandler::enqueueCloseCommand(CommandProcessedCallbackDefinition commandProcessedCallback) {
		const RelayCommand cmd{
			RelayCommand::Command::Close,
			0,
			commandProcessedCallback,
			{false, 0}
		};
		_commandFifo.enqueue( cmd );
	}

	void RelayHandler::enqueueOpenCloseCommand(RelayState newState, CommandProcessedCallbackDefinition commandProcessedCallback) {
		if ( newState == RelayState::Open )
			enqueueOpenCommand(commandProcessedCallback);
		else if ( newState == RelayState::Closed )
			enqueueCloseCommand(commandProcessedCallback);
	}

	void RelayHandler::enqueueDelayCommand(uint32_t delayDuration, CommandProcessedCallbackDefinition commandProcessedCallback) {
		if ( !delayDuration )
			return;
		const RelayCommand cmd{RelayCommand::Command::Delay, delayDuration};
		_commandFifo.enqueue( cmd );
	}

	RelayState RelayHandler::getRelayState() {
		return _currentRelayState;
	}

	void RelayHandler::clearCommandQueue() {
		// At first, we check if an Open/Close command is currently being executed. If so, stop the current from flowing!
		RelayCommand *command = _commandFifo.peek();
		if ( command != nullptr  &&  (command->Command == RelayCommand::Command::Open || command->Command == RelayCommand::Command::Close) ) {
			if ( command->ExecutionDetails.isExecuting ) {
				HAL_GPIO_WritePin( RELAY_CLOSE_GPIO_Port, RELAY_CLOSE_Pin, GPIO_PIN_RESET );
				HAL_GPIO_WritePin( RELAY_OPEN_GPIO_Port, RELAY_OPEN_Pin, GPIO_PIN_RESET );
			}
		}
		// Now we may clear the list.
		_commandFifo.clear();
	}

	void RelayHandler::flushCommandQueue(uint32_t timeoutMillis) {
		uint32_t startedAt = HAL_GetTick();
		while ( (HAL_GetTick()-startedAt) <= timeoutMillis ) {
			if ( _commandFifo.isEmpty() )  break;
			main();
			Adc::main();
		}
		_commandFifo.clear();
	}




	/**
	  * @brief PWR PVD interrupt callback
	  * @retval None
	  */
	extern "C" void HAL_PWR_PVDCallback(void) {
		HAL_GPIO_WritePin( RELAY_CLOSE_GPIO_Port, RELAY_CLOSE_Pin, GPIO_PIN_RESET );
		HAL_GPIO_WritePin( RELAY_OPEN_GPIO_Port, RELAY_OPEN_Pin, GPIO_PIN_SET );
		while(1);
	}


} /*namespace Hardware*/
