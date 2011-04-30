/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/CommandParser.h>

#include <boost/cast.hpp>

#include <Swiften/Parser/PayloadParsers/FormParserFactory.h>
#include <Swiften/Parser/PayloadParsers/FormParser.h>

namespace Swift {

CommandParser::CommandParser() : level_(TopLevel), inNote_(false), inActions_(false), noteType_(Command::Note::Info), formParser_(0)  {
	formParserFactory_ = new FormParserFactory();
}

CommandParser::~CommandParser() {
	delete formParserFactory_;
}

void CommandParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	++level_;
	if (level_ == PayloadLevel) {
		boost::optional<Command::Action> action = parseAction(attributes.getAttribute("action"));
		if (action) {
			getPayloadInternal()->setAction(*action);
		}

		std::string status = attributes.getAttribute("status");
		if (status == "executing") {
			getPayloadInternal()->setStatus(Command::Executing);
		}
		else if (status == "completed") {
			getPayloadInternal()->setStatus(Command::Completed);
		}
		else if (status == "canceled") {
			getPayloadInternal()->setStatus(Command::Canceled);
		}

		getPayloadInternal()->setNode(attributes.getAttribute("node"));
		getPayloadInternal()->setSessionID(attributes.getAttribute("sessionid"));
	}
	else if (level_ == FormOrNoteOrActionsLevel) {
		assert(!formParser_);
		if (formParserFactory_->canParse(element, ns, attributes)) {
			formParser_ = boost::polymorphic_downcast<FormParser*>(formParserFactory_->createPayloadParser());
			assert(formParser_);
		}
		else if (element == "note") {
			inNote_ = true;
			currentText_.clear();
			std::string noteType = attributes.getAttribute("type");
			if (noteType == "info") {
				noteType_ = Command::Note::Info;
			}
			else if (noteType == "warn") {
				noteType_ = Command::Note::Warn;
			}
			else if (noteType == "error") {
				noteType_ = Command::Note::Error;
			}
			else {
				noteType_ = Command::Note::Info;
			}
		}
		else if (element == "actions") {
			inActions_ = true;
			boost::optional<Command::Action> action = parseAction(attributes.getAttribute("execute"));
			if (action) {
				getPayloadInternal()->setExecuteAction(*action);
			}
		}
	}
	else if (level_ == ActionsActionLevel) {
	}

	if (formParser_) {
		formParser_->handleStartElement(element, ns, attributes);
	}
}

void CommandParser::handleEndElement(const std::string& element, const std::string& ns) {
	if (formParser_) {
		formParser_->handleEndElement(element, ns);
	}

	if (level_ == FormOrNoteOrActionsLevel) {
		if (formParser_) {
			Form::ref form(boost::dynamic_pointer_cast<Form>(formParser_->getPayload()));
			assert(form);
			getPayloadInternal()->setForm(form);
			delete formParser_;
			formParser_ = 0;
		}
		else if (inNote_) {
			inNote_ = false;
			getPayloadInternal()->addNote(Command::Note(currentText_, noteType_));
		}
		else if (inActions_) {
			inActions_ = false;
		}
	}
	else if (level_ == ActionsActionLevel && inActions_) {
		boost::optional<Command::Action> action = parseAction(element);
		if (action) {
			getPayloadInternal()->addAvailableAction(*action);
		}
	}
	--level_;
}

void CommandParser::handleCharacterData(const std::string& data) {
	if (formParser_) {
		formParser_->handleCharacterData(data);
	}
	else {
		currentText_ += data;
	}
}

boost::optional<Command::Action> CommandParser::parseAction(const std::string& action) {
	if (action == "execute") {
		return Command::Execute;
	}
	else if (action == "cancel") {
		return Command::Cancel;
	}
	else if (action == "prev") {
		return Command::Prev;
	}
	else if (action == "next") {
		return Command::Next;
	}
	else if (action == "complete") {
		return Command::Complete;
	}
	return boost::optional<Command::Action>();
}

}
