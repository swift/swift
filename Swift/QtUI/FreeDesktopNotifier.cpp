/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma GCC diagnostic ignored "-Wredundant-decls"

#include "Swift/QtUI/FreeDesktopNotifier.h"

#include <QMap>
#include <QString>
#include <QStringList>
#include <QtDBus/QtDBus>
#include <algorithm>
#include <Swiften/Base/Path.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

FreeDesktopNotifier::FreeDesktopNotifier(const std::string& name) : applicationName(name) {
}

void FreeDesktopNotifier::showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()>) {
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        return;
    }
    std::vector<Notifier::Type> defaultTypes = getDefaultTypes();
    if (std::find(defaultTypes.begin(), defaultTypes.end(), type) == defaultTypes.end()) {
        return;
    }

    QString body = description.c_str();
    body = body.replace("&", "&amp;");
    body = body.replace("<", "&lt;");
    body = body.replace(">", "&gt;");

    int timeout = (type == IncomingMessage || type == SystemMessage) ? DEFAULT_MESSAGE_NOTIFICATION_TIMEOUT_SECONDS : DEFAULT_STATUS_NOTIFICATION_TIMEOUT_SECONDS;

    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "", "Notify");

    QStringList actions;
    QMap<QString, QVariant> hints;
    hints["x-canonical-append"] = QString("allowed");
    msg << applicationName.c_str();
    msg << quint32(0); // ID of previous notification to replace
    msg << P2QSTRING(pathToString(imageScaler.getScaledImage(picture, 48))); // Icon to display
    msg << subject.c_str(); // Summary / Header of the message to display
    msg << body; // Body of the message to display
    msg << actions; // Actions from which the user may choose
    msg << hints; // Hints to the server displaying the message
    msg << qint32(timeout*1000); // Timeout in milliseconds

    bus.asyncCall(msg);
}

}
