/*
 * RelayHandler.h
 *
 *  Created on: 03.04.2020
 *      Author: Robert Voelckner
 *
 *  Description:
 *  	Handles the bi-stable relay and offers a queue of (open/close/delay) commands to the application.
 */

#ifndef APPLICATION_USER_HARDWARE_RELAYHANDLER_RELAYHANDLER_H_
#define APPLICATION_USER_HARDWARE_RELAYHANDLER_RELAYHANDLER_H_

#include "RelayHandlerConfig.h"

#include <stdint-gcc.h>
#include <Lists/StaticMemory/Fifo.h>


namespace Hardware {

	class RelayHandler {
		private:

			RelayHandler() = delete;
			RelayHandler( const RelayHandler& ) = delete;

			/// Definition of the callback function pointer
			typedef void (*CommandProcessedCallbackDefinition)();

			/// Definition of the commands for controlling the relay
			struct RelayCommand{
				enum class Command {
					Open, Close, Delay
				} Command;
				uint32_t DelayDurationMillis;                                 	///< Only used in connection with the Delay command.
				CommandProcessedCallbackDefinition CommandProcessedCallback;  	///< If NULL, nothing will be called.

				struct {
					bool isExecuting;
					uint32_t StartedExecutionAt;
				} ExecutionDetails;                                           	///< These data will be used in connection with Open/Close commands to keep track of current flow duration.
			};

			/// Various fields
			static RelayHandlerConfig                                          *_relayHandlerConfig;
			static Util::Lists::StaticMemory::Fifo<RelayHandler::RelayCommand>  _commandFifo;
			static RelayState                                                   _currentRelayState;


			/// Constants
			static constexpr float MinimumVoltage = 7.0f;             	///< The minimum voltage necessary for opening/closing the relay
			static constexpr uint32_t CurrentFlowMillis = 30;         	///< The amount of milliseconds current will flow through the relay when opening/closing

		public:

			/**
			 * Initializes the RelayHandler module.
			 *
			 * @param relayHandlerConfig  	..	Points to the relay configuration. Must not be null!
			 */
			static void init(RelayHandlerConfig *relayHandlerConfig);

			/**
			 * Main function of the RelayHandler module. Must be called cyclically from main loop.
			 *
			 * As the relay gets opened/closed with current pulses over a constant amount of time,
			 * this function blocks when performing switch operations.
			 */
			static void main();

			/**
			 * Enqueues the command "open relay" - which will be executed by the main() function.
			 *
			 * @param commandProcessedCallback	..	Will be called after the command was successfully executed. If nullptr,
			 *                                	  	no callback will be invoked.
			 */
			static void enqueueOpenCommand(CommandProcessedCallbackDefinition commandProcessedCallback = nullptr);

			/**
			 * Enqueues the command "close relay" - which will be executed by the main() function.
			 *
			 * @param commandProcessedCallback	..	Will be called after the command was successfully executed. If nullptr,
			 *                                	  	no callback will be invoked.
			 */
			static void enqueueCloseCommand(CommandProcessedCallbackDefinition commandProcessedCallback = nullptr);

			/**
			 * Generalizes both previous methods.
			 *
			 * @param newState                  ..  New desired RelayState. Only RelayState::Open and RelayState::Closed are allowed.
			 * @param commandProcessedCallback  ..  Will be called after the command was successfully executed. If nullptr,
			 *                                	  	no callback will be invoked.
			 */
			static void enqueueOpenCloseCommand(RelayState newState, CommandProcessedCallbackDefinition commandProcessedCallback = nullptr);

			/**
			 * Enqueues the command "close relay" - which will be executed by the main() function.
			 *
			 * @param delayDuration           	..	The amount of milliseconds.
			 * @param commandProcessedCallback	..	Will be called after the command was successfully executed. If nullptr,
			 *                                	  	no callback will be invoked.
			 */
			static void enqueueDelayCommand(uint32_t delayDuration, CommandProcessedCallbackDefinition commandProcessedCallback = nullptr);

			/**
			 * Returns the current relay state.
			 */
			static RelayState getRelayState();

			/**
			 * Clears the command queue. Possibly contained commands will not be executed.
			 */
			static void clearCommandQueue();

			/**
			 * Flushes the command queue. Possibly contained commands will be executed.
			 *
			 * @param timeoutMillis	..	Contains the timeout in milliseconds. Eventually not yet
			 *                     	  	executed commands will be removed.
			 */
			static void flushCommandQueue(uint32_t timeoutMillis = 300);

	};

} /*namespace Hardware*/

#endif /* APPLICATION_USER_HARDWARE_RELAYHANDLER_RELAYHANDLER_H_ */
