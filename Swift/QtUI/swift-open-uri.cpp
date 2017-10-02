/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>

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
