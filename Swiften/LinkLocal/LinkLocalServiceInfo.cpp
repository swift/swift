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

}
