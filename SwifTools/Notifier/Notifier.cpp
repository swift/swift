/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "SwifTools/Notifier/Notifier.h"

namespace Swift {

Notifier::Notifier() : enabled(true) {
}

Notifier::~Notifier() {
}

void Notifier::showMessage(Type type, const String& subject, const String& description, const boost::filesystem::path& picture, boost::function<void()> callback) {
	if (enabled) {
		doShowMessage(type, subject, description, picture, callback);
	}
}

String Notifier::typeToString(Type type) {
	switch (type) {
		case ContactAvailable: return "Contact Becomes Available";
		case ContactUnavailable: return "Contact Becomes Unavailable";
		case ContactStatusChange: return "Contact Changes Status";
		case IncomingMessage: return "Incoming Message";
		case SystemMessage: return "System Message";
	}
	assert(false);
	return "";
}

std::vector<Notifier::Type> Notifier::getAllTypes() {
	std::vector<Type> result;
	result.push_back(ContactAvailable);
	result.push_back(ContactUnavailable);
	result.push_back(ContactStatusChange);
	result.push_back(IncomingMessage);
	result.push_back(SystemMessage);
	return result;
}

std::vector<Notifier::Type> Notifier::getDefaultTypes() {
	std::vector<Type> result;
	result.push_back(IncomingMessage);
	result.push_back(SystemMessage);
	return result;
}

}
