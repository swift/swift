/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/AdHoc/OutgoingAdHocCommandSession.h>

#include <boost/bind.hpp>

#include <Swiften/Queries/GenericRequest.h>

namespace Swift {
OutgoingAdHocCommandSession::OutgoingAdHocCommandSession(const DiscoItems::Item& command, IQRouter* iqRouter) : command_(command), iqRouter_(iqRouter), isMultiStage_(false) {

}

void OutgoingAdHocCommandSession::handleResponse(boost::shared_ptr<Command> payload, ErrorPayload::ref error) {
	if (error) {
		onError(error);
	} else {
		const std::vector<Command::Action> actions = payload->getAvailableActions();
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

bool OutgoingAdHocCommandSession::getIsMultiStage() {
	return isMultiStage_;
}

void OutgoingAdHocCommandSession::start() {
	boost::shared_ptr<Payload> commandPayload(new Command(command_.getNode()));
	boost::shared_ptr<GenericRequest<Command> > commandRequest(new GenericRequest<Command>(IQ::Set, command_.getJID(), commandPayload, iqRouter_));
	commandRequest->onResponse.connect(boost::bind(&OutgoingAdHocCommandSession::handleResponse, this, _1, _2));
	commandRequest->send();
}

void OutgoingAdHocCommandSession::cancel() {
	if (!sessionID_.empty()) {
		boost::shared_ptr<Payload> commandPayload(new Command(command_.getNode(), sessionID_, Command::Cancel));
		boost::shared_ptr<GenericRequest<Command> > commandRequest(new GenericRequest<Command>(IQ::Set, command_.getJID(), commandPayload, iqRouter_));
		commandRequest->onResponse.connect(boost::bind(&OutgoingAdHocCommandSession::handleResponse, this, _1, _2));
		commandRequest->send();
	}
}

void OutgoingAdHocCommandSession::goBack() {
	boost::shared_ptr<Payload> commandPayload(new Command(command_.getNode(), sessionID_, Command::Prev));
	boost::shared_ptr<GenericRequest<Command> > commandRequest(new GenericRequest<Command>(IQ::Set, command_.getJID(), commandPayload, iqRouter_));
	commandRequest->onResponse.connect(boost::bind(&OutgoingAdHocCommandSession::handleResponse, this, _1, _2));
	commandRequest->send();
}

void OutgoingAdHocCommandSession::complete(Form::ref form) {
	Command* command = new Command(command_.getNode(), sessionID_, Command::Complete);
	boost::shared_ptr<Payload> commandPayload(command);
	command->setForm(form);
	boost::shared_ptr<GenericRequest<Command> > commandRequest(new GenericRequest<Command>(IQ::Set, command_.getJID(), commandPayload, iqRouter_));
	commandRequest->onResponse.connect(boost::bind(&OutgoingAdHocCommandSession::handleResponse, this, _1, _2));
	commandRequest->send();
}

void OutgoingAdHocCommandSession::goNext(Form::ref form) {
	Command* command = new Command(command_.getNode(), sessionID_, Command::Next);
	boost::shared_ptr<Payload> commandPayload(command);
	command->setForm(form);
	boost::shared_ptr<GenericRequest<Command> > commandRequest(new GenericRequest<Command>(IQ::Set, command_.getJID(), commandPayload, iqRouter_));
	commandRequest->onResponse.connect(boost::bind(&OutgoingAdHocCommandSession::handleResponse, this, _1, _2));
	commandRequest->send();
}

OutgoingAdHocCommandSession::ActionState OutgoingAdHocCommandSession::getActionState(Command::Action action) {
	return actionStates_[action];
}

}
