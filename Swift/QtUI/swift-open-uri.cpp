/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <iostream>

int main(int argc, char* argv[]) {
	QCoreApplication app(argc, argv);

	QDBusConnection bus = QDBusConnection::sessionBus();
	if (!bus.isConnected()) {
		return -1;
	}
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " uri" << std::endl;
		return -1;
	}

	QDBusMessage msg = QDBusMessage::createMethodCall("im.swift.Swift.URIHandler", "/", "im.swift.Swift.URIHandler", "openURI");
	msg << argv[1];

	bus.call(msg);

	return 0;
}
