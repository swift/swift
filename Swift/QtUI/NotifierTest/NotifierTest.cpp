/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>
#include <string>

#include <boost/bind.hpp>

#include <QApplication>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Notifier/GrowlNotifier.h>

using namespace Swift;

void notificationClicked(const std::string& message) {
    std::cout << "Notification clicked: " << message << std::endl;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    GrowlNotifier notifier("Swift-NotifierTest");
    notifier.showMessage(Notifier::ContactAvailable, "Contact is available", "The contact has become available", ByteArray(), boost::bind(&notificationClicked, "Message 1"));
    return app.exec();
}
