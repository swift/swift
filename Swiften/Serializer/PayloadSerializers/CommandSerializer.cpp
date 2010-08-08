/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/CommandSerializer.h"

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/foreach.h"
#include "Swiften/Serializer/XML/XMLElement.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"
#include "Swiften/Serializer/XML/XMLRawTextNode.h"
#include "Swiften/Serializer/PayloadSerializerCollection.h"

namespace Swift {

CommandSerializer::CommandSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

String CommandSerializer::serializePayload(boost::shared_ptr<Command> command)	const {
	XMLElement commandElement("command", "http://jabber.org/protocol/comands");
	commandElement.setAttribute("node", command->getNode());

	if (!command->getSessionID().isEmpty()) {
		commandElement.setAttribute("sessionid", command->getSessionID());
	}

	String action = actionToString(command->getPerformedAction());
	if (!action.isEmpty()) {
		commandElement.setAttribute("action", action);
	}

	String status;
	switch (command->getStatus()) {
		case Command::Executing: status = "executing";break;
		case Command::Completed: status = "completed";break;
		case Command::Canceled: status = "canceled";break;
		case Command::NoStatus: break;
	}
	if (!status.isEmpty()) {
		commandElement.setAttribute("status", status);
	}

	if (command->getAvailableActions().size() > 0) {
		String actions = "<actions";
		String executeAction = actionToString(command->getExecuteAction());
		if (!executeAction.isEmpty()) {
			actions += " execute='" + executeAction + "'";
		}
		actions += ">";
		foreach (Command::Action action, command->getAvailableActions()) {
			actions += "<" + actionToString(action) + "/>";
		}
		actions += "</actions>";
		commandElement.addNode(boost::shared_ptr<XMLRawTextNode>(new XMLRawTextNode(actions)));
	}

	foreach (Command::Note note, command->getNotes()) {
		boost::shared_ptr<XMLElement> noteElement(new XMLElement("note"));
		String type;
		switch (note.type) {
			case Command::Note::Info: type = "info";
			case Command::Note::Warn: type = "warn";
			case Command::Note::Error: type = "error";
		}
		if (!type.isEmpty()) {
			noteElement->setAttribute("type", type);
		}
		noteElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(note.note)));
		commandElement.addNode(noteElement);
	}

	boost::shared_ptr<Payload> payload = command->getPayload();
	if (payload) {
		PayloadSerializer* serializer = serializers->getPayloadSerializer(payload);
		if (serializer) {
			commandElement.addNode(boost::shared_ptr<XMLRawTextNode>(new XMLRawTextNode(serializer->serialize(payload))));
		}
	}
	return commandElement.serialize();
}

String CommandSerializer::actionToString(Command::Action action) const {
	String string;
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
