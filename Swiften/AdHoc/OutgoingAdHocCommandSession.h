/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/Command.h>
#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
	class IQRouter;
	class MainWindow;
	class UIEventStream;
	class AdHocCommandWindowFactory;
	class OutgoingAdHocCommandSession {
		public:
			OutgoingAdHocCommandSession(const DiscoItems::Item& command, AdHocCommandWindowFactory* factory, IQRouter* iqRouter);
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
			bool getIsMultiStage();

			/**
			 * Emitted when the form for the next stage is available.
			 */
			boost::signal<void (Command::ref)> onNextStageReceived;

			/**
			 * Emitted on error.
			 */
			boost::signal<void (ErrorPayload::ref)> onError;
		private:
			void handleResponse(boost::shared_ptr<Command> payload, ErrorPayload::ref error);
		private:
			DiscoItems::Item command_;
			IQRouter* iqRouter_;
			bool isMultiStage_;
			std::string sessionID_;
	};
}
