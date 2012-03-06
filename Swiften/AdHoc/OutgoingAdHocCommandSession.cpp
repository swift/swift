/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Base/Algorithm.h>

namespace Swift {
OutgoingAdHocCommandSession::OutgoingAdHocCommandSession(const JID& to, const std::string& commandNode, IQRouter* iqRouter) : to_(to), commandNode_(commandNode), iqRouter_(iqRouter), isMultiStage_(false) {

}

OutgoingAdHocCommandSession::~OutgoingAdHocCommandSession() {
	connection_.disconnect();
}

void OutgoingAdHocCommandSession::handleResponse(boost::shared_ptr<Command> payload, ErrorPayload::ref error) {
	if (error) {
		onError(error);
	} else {
		const std::vector<Command::Action>& actions = payload->getAvailableActions();
		actionStates_.clear();
		if (payload->getStatus() == Command::Executing ) {
			actionStates_[Command::Cancel] = EnabledAndPresent;
			actionStates_[Command::Complete] = Present;
			if (std::find(actions.begin(), actions.end(), Command::Complete) != actions.end()) {
				actionStates_[Command::Complete] = EnabledAndPresent;
			}

			if (getIsMultiStage()) {
				actionStates_[Command::Next] = Present;
				actionStates_[Command::Prev] = Present;
			}

		if (std::find(actions.begin(), actions.end(), Command::Next) != actions.end()) {
				actionStates_[Command::Next] = EnabledAndPresent;
			}
		if (std::find(actions.begin(), actions.end(), Command::Prev) != actions.end()) {
				actionStates_[Command::Prev] = EnabledAndPresent;
			}
		}

		sessionID_ = payload->getSessionID();
		if (std::find(actions.begin(), actions.end(), Command::Next) != actions.end()
				|| std::find(actions.begin(), actions.end(), Command::Prev) != actions.end()) {
			isMultiStage_ = true;
		}
		onNextStageReceived(payload);
	}
}

bool OutgoingAdHocCommandSession::getIsMultiStage() const {
	return isMultiStage_;
}

void OutgoingAdHocCommandSession::start() {
	boost::shared_ptr<GenericRequest<Command> > commandRequest = boost::make_shared< GenericRequest<Command> >(IQ::Set, to_, boost::make_shared<Command>(commandNode_), iqRouter_);
	connection_ = commandRequest->onResponse.connect(boost::bind(&OutgoingAdHocCommandSession::handleResponse, this, _1, _2));
	commandRequest->send();
}

void OutgoingAdHocCommandSession::cancel() {
	if (!sessionID_.empty()) {
		submitForm(Form::ref(), Command::Cancel);
	}
}

void OutgoingAdHocCommandSession::goBack() {
	submitForm(Form::ref(), Command::Prev);
}

void OutgoingAdHocCommandSession::complete(Form::ref form) {
	submitForm(form, Command::Complete);
}

void OutgoingAdHocCommandSession::goNext(Form::ref form) {
	submitForm(form, Command::Next);
}

void OutgoingAdHocCommandSession::submitForm(Form::ref form, Command::Action action) {
	boost::shared_ptr<Command> command(boost::make_shared<Command>(commandNode_, sessionID_, action));
	command->setForm(form);
	boost::shared_ptr<GenericRequest<Command> > commandRequest = boost::make_shared< GenericRequest<Command> >(IQ::Set, to_, command, iqRouter_);
	connection_.disconnect();
	connection_ = commandRequest->onResponse.connect(boost::bind(&OutgoingAdHocCommandSession::handleResponse, this, _1, _2));
	commandRequest->send();
}

OutgoingAdHocCommandSession::ActionState OutgoingAdHocCommandSession::getActionState(Command::Action action) const {
	return get(actionStates_, action, Absent);
}

}
