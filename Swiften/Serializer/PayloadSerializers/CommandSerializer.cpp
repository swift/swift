/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/CommandSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>

namespace Swift {

CommandSerializer::CommandSerializer() {
}

std::string CommandSerializer::serializePayload(boost::shared_ptr<Command> command)	const {
	XMLElement commandElement("command", "http://jabber.org/protocol/commands");
	commandElement.setAttribute("node", command->getNode());

	if (!command->getSessionID().empty()) {
		commandElement.setAttribute("sessionid", command->getSessionID());
	}

	std::string action = actionToString(command->getAction());
	if (!action.empty()) {
		commandElement.setAttribute("action", action);
	}

	std::string status;
	switch (command->getStatus()) {
		case Command::Executing: status = "executing";break;
		case Command::Completed: status = "completed";break;
		case Command::Canceled: status = "canceled";break;
		case Command::NoStatus: break;
	}
	if (!status.empty()) {
		commandElement.setAttribute("status", status);
	}

	if (command->getAvailableActions().size() > 0) {
		std::string actions = "<actions";
		std::string executeAction = actionToString(command->getExecuteAction());
		if (!executeAction.empty()) {
			actions += " execute='" + executeAction + "'";
		}
		actions += ">";
		foreach (Command::Action action, command->getAvailableActions()) {
			actions += "<" + actionToString(action) + "/>";
		}
		actions += "</actions>";
		commandElement.addNode(boost::make_shared<XMLRawTextNode>(actions));
	}

	foreach (Command::Note note, command->getNotes()) {
		boost::shared_ptr<XMLElement> noteElement(new XMLElement("note"));
		std::string type;
		switch (note.type) {
			case Command::Note::Info: type = "info"; break;
			case Command::Note::Warn: type = "warn"; break;
			case Command::Note::Error: type = "error"; break;
		}
		if (!type.empty()) {
			noteElement->setAttribute("type", type);
		}
		noteElement->addNode(boost::make_shared<XMLTextNode>(note.note));
		commandElement.addNode(noteElement);
	}

	Form::ref form = command->getForm();
	if (form) {
		commandElement.addNode(boost::make_shared<XMLRawTextNode>(FormSerializer().serialize(form)));
	}
	return commandElement.serialize();
}

std::string CommandSerializer::actionToString(Command::Action action) const {
	std::string string;
	switch (action) {
		case Command::Cancel: string = "cancel"; break;
		case Command::Execute: string = "execute"; break;
		case Command::Complete: string = "complete"; break;
		case Command::Prev: string = "prev"; break;
		case Command::Next: string = "next"; break;
		case Command::NoAction: break;
	}
	return string;
}

}
