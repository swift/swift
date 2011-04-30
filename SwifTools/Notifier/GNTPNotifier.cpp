/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

// FIXME: This notifier needs finishing (handling callbacks etc.)

#include <SwifTools/Notifier/GNTPNotifier.h>

#include <cassert>
#include <iostream>
#include <boost/bind.hpp>
#include <sstream>

#include <Swiften/Base/foreach.h>
#include <Swiften/Network/ConnectionFactory.h>

namespace Swift {

GNTPNotifier::GNTPNotifier(const std::string& name, const boost::filesystem::path& icon, ConnectionFactory* connectionFactory) : name(name), icon(icon), connectionFactory(connectionFactory), initialized(false), registered(false) {
	// Registration message
	std::ostringstream message;
	message << "GNTP/1.0 REGISTER NONE\r\n";
	message << "Application-Name: " << name << "\r\n";
	message << "Application-Icon: file://" << icon.string() << "\r\n";
	message << "Notifications-Count: " << getAllTypes().size() << "\r\n";
	std::vector<Notifier::Type> defaultTypes = getDefaultTypes();
	std::vector<Notifier::Type> allTypes = getAllTypes();
	foreach(Notifier::Type type, allTypes) {
		message << "\r\n";
		message << "Notification-Name: " << typeToString(type) << "\r\n";
		message << "Notification-Enabled: " << (std::find(defaultTypes.begin(), defaultTypes.end(), type) == defaultTypes.end() ? "false" : "true") << "\r\n";
	}
	message << "\r\n";

	send(message.str());
}

GNTPNotifier::~GNTPNotifier() {
}

void GNTPNotifier::send(const std::string& message) {
	if (currentConnection) {
		return;
	}
	currentMessage = message;
	currentConnection = connectionFactory->createConnection();
	currentConnection->onConnectFinished.connect(boost::bind(&GNTPNotifier::handleConnectFinished, this, _1));
	currentConnection->onDataRead.connect(boost::bind(&GNTPNotifier::handleDataRead, this, _1));
	currentConnection->connect(HostAddressPort(HostAddress("127.0.0.1"), 23053));
}

void GNTPNotifier::showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()>) {
	if (registered) {
		std::ostringstream message;
		message << "GNTP/1.0 NOTIFY NONE\r\n";
		message << "Application-Name: " << name << "\r\n";
		message << "Notification-Name: " << typeToString(type) << "\r\n";
		message << "Notification-Title: " << subject << "\r\n";
		message << "Notification-Text: " << description << "\r\n";
		message << "Notification-Icon: " << picture.string() << "\r\n";
		message << "\r\n";
		send(message.str());
	}
}

void GNTPNotifier::handleConnectFinished(bool error) {
	if (!initialized) {
		initialized = true;
		registered = !error;
	}

	if (!error) {
		currentConnection->write(currentMessage.c_str());
	}
}

void GNTPNotifier::handleDataRead(const ByteArray&) {
	currentConnection->onDataRead.disconnect(boost::bind(&GNTPNotifier::handleDataRead, this, _1));
	currentConnection->onConnectFinished.disconnect(boost::bind(&GNTPNotifier::handleConnectFinished, this, _1));
	currentConnection.reset();
}

}
