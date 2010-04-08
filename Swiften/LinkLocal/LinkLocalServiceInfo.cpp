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
	if (!firstName.isEmpty()) {
		result += getEncoded("1st=" + firstName);
	}
	if (!lastName.isEmpty()) {
		result += getEncoded("last=" + lastName);
	}
	if (!email.isEmpty()) {
		result += getEncoded("email=" + email);
	}
	if (jid.isValid()) {
		result += getEncoded("jid=" + jid.toString());
	}
	if (!message.isEmpty()) {
		result += getEncoded("msg=" + message);
	}
	if (!nick.isEmpty()) {
		result += getEncoded("nick=" + nick);
	}
	if (port) {
		result += getEncoded("port.p2pj=" + String(boost::lexical_cast<std::string>(*port)));
	}

	switch (status) {
		case Available: result += getEncoded("status=avail"); break;
		case Away: result += getEncoded("status=away"); break;
		case DND: result += getEncoded("status=dnd"); break;
	}

	return result;
}

ByteArray LinkLocalServiceInfo::getEncoded(const String& s) {
	ByteArray sizeByte;
	sizeByte.resize(1);
	assert(s.getLength() < 256);
	sizeByte[0] = s.getUTF8Size();
	return sizeByte + ByteArray(s);
}

LinkLocalServiceInfo LinkLocalServiceInfo::createFromTXTRecord(const ByteArray& record) {
	LinkLocalServiceInfo info;
	size_t i = 0;
	while (i < record.getSize()) {
		std::pair<String,String> entry = readEntry(record, &i);
		if (entry.first.isEmpty()) {
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

std::pair<String,String> LinkLocalServiceInfo::readEntry(const ByteArray& record, size_t* index) {
	size_t& i = *index;
	String key;
	String value;

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
