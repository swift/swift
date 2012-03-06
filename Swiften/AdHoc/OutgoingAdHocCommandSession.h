/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>
#include <map>

#include <Swiften/JID/JID.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/Command.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <boost/signals/connection.hpp>

namespace Swift {
	class IQRouter;
	class MainWindow;
	class UIEventStream;

	class OutgoingAdHocCommandSession {
		public:

			/**
			 * Availability of action.
			 */
			enum ActionState {
				Absent /** Action isn't applicable to this command. */ = 0,
				Present /** Action is applicable to this command */= 1,
				Enabled /** Action is applicable and currently available */ = 2,
				EnabledAndPresent = 3};

			OutgoingAdHocCommandSession(const JID& to, const std::string& commandNode, IQRouter* iqRouter);
			~OutgoingAdHocCommandSession();
			/**
			 * Send initial request to the target.
			 */
			void start();
			/**
			 * Cancel command session with the target.
			 */
			void cancel();
			/**
			 * Return to the previous stage.
			 */
			void goBack();
			/**
			 * Send the form to complete the command.
			 * \param form Form for submission - if missing the command will be submitted with no form.
			 */
			void complete(Form::ref form);
			/**
			 * Send the form to advance to the next stage of the command.
			 * \param form Form for submission - if missing the command will be submitted with no form.
			 */
			void goNext(Form::ref form);

			/**
			 * Is the form multi-stage?
			 */
			bool getIsMultiStage() const;

			/**
			 * Emitted when the form for the next stage is available.
			 */
			boost::signal<void (Command::ref)> onNextStageReceived;

			/**
			 * Emitted on error.
			 */
			boost::signal<void (ErrorPayload::ref)> onError;

			/**
			 * Get the state of a given action.
			 * This is useful for a UI to determine which buttons should be visible,
			 * and which enabled.
			 * Use for Next, Prev, Cancel and Complete only.
			 * If no actions are available, the command has completed.
			 */
			ActionState getActionState(Command::Action action) const;

		private:
			void handleResponse(boost::shared_ptr<Command> payload, ErrorPayload::ref error);
			void submitForm(Form::ref, Command::Action action);

		private:
			JID to_;
			std::string commandNode_;
			IQRouter* iqRouter_;
			bool isMultiStage_;
			std::string sessionID_;
			std::map<Command::Action, ActionState> actionStates_;
			boost::bsignals::connection connection_;
	};
}
