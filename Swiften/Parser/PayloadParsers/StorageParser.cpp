/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/StorageParser.h>

#include <cassert>

namespace Swift {

StorageParser::StorageParser() : level(TopLevel) {
}

void StorageParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
	if (level == BookmarkLevel) {
		if (element == "conference") {
			assert(!room);
			room = Storage::Room();
			room->autoJoin = attributes.getBoolAttribute("autojoin", false);
			room->jid = JID(attributes.getAttribute("jid"));
			room->name = attributes.getAttribute("name");
		}
		else if (element == "url") {
			assert(!url);
			url = Storage::URL();
			url->name = attributes.getAttribute("name");
			url->url = attributes.getAttribute("url");
		}
	}
	else if (level == DetailLevel) {
		currentText = "";
	}
	++level;
}

void StorageParser::handleEndElement(const std::string& element, const std::string&) {
	--level;
	if (level == BookmarkLevel) {
		if (element == "conference") {
			assert(room);
			getPayloadInternal()->addRoom(*room);
			room.reset();
		}
		else if (element == "url") {
			assert(url);
			getPayloadInternal()->addURL(*url);
			url.reset();
		}
	}
	else if (level == DetailLevel && room) {
		if (element == "nick") {
			room->nick = currentText;
		}
		else if (element == "password") {
			room->password = currentText;
		}
	}
}

void StorageParser::handleCharacterData(const std::string& data) {
	currentText += data;
}

}
