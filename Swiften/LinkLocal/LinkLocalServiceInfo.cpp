/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

#include <boost/lexical_cast.hpp>

namespace Swift {

ByteArray LinkLocalServiceInfo::toTXTRecord() const {
	ByteArray result(getEncoded("txtvers=1"));
	if (!firstName.empty()) {
		result += getEncoded("1st=" + firstName);
	}
	if (!lastName.empty()) {
		result += getEncoded("last=" + lastName);
	}
	if (!email.empty()) {
		result += getEncoded("email=" + email);
	}
	if (jid.isValid()) {
		result += getEncoded("jid=" + jid.toString());
	}
	if (!message.empty()) {
		result += getEncoded("msg=" + message);
	}
	if (!nick.empty()) {
		result += getEncoded("nick=" + nick);
	}
	if (port) {
		result += getEncoded("port.p2pj=" + std::string(boost::lexical_cast<std::string>(*port)));
	}

	switch (status) {
		case Available: result += getEncoded("status=avail"); break;
		case Away: result += getEncoded("status=away"); break;
		case DND: result += getEncoded("status=dnd"); break;
	}

	return result;
}

ByteArray LinkLocalServiceInfo::getEncoded(const std::string& s) {
	ByteArray sizeByte;
	sizeByte.resize(1);
	sizeByte[0] = s.size();
	return sizeByte + ByteArray(s);
}

LinkLocalServiceInfo LinkLocalServiceInfo::createFromTXTRecord(const ByteArray& record) {
	LinkLocalServiceInfo info;
	size_t i = 0;
	while (i < record.getSize()) {
		std::pair<std::string,std::string> entry = readEntry(record, &i);
		if (entry.first.empty()) {
			break;
		}
		else if (entry.first == "1st") {
			info.setFirstName(entry.second);
		}
		else if (entry.first == "last") {
			info.setLastName(entry.second);
		}
		else if (entry.first == "email") {
			info.setEMail(entry.second);
		}
		else if (entry.first == "jid") {
			info.setJID(JID(entry.second));
		}
		else if (entry.first == "msg") {
			info.setMessage(entry.second);
		}
		else if (entry.first == "nick") {
			info.setNick(entry.second);
		}
		else if (entry.first == "port.p2pj") {
			info.setPort(boost::lexical_cast<int>(entry.second));
		}
		else if (entry.first == "status") {
			if (entry.second == "away") {
				info.setStatus(Away);
			}
			else if (entry.second == "dnd") {
				info.setStatus(DND);
			}
		}
	}
	return info;
}

std::pair<std::string,std::string> LinkLocalServiceInfo::readEntry(const ByteArray& record, size_t* index) {
	size_t& i = *index;
	std::string key;
	std::string value;

	size_t entryEnd = i + 1 + record[i];
	++i;
	bool inKey = true;
	while (i < entryEnd && i < record.getSize()) {
		if (inKey) {
			if (record[i] == '=') {
				inKey = false;
			}
			else {
				key += record[i];
			}
		}
		else {
			value += record[i];
		}
		++i;
	}
	return std::make_pair(key, value);
}

}
