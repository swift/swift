/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtDBUSURIHandler.h>

#include <QDBusAbstractAdaptor>
#include <QDBusConnection>

#include <Swift/QtUI/QtSwiftUtil.h>

using namespace Swift;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-member-function"

namespace {
    class DBUSAdaptor: public QDBusAbstractAdaptor {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", "im.swift.Swift.URIHandler");
        public:
            DBUSAdaptor(QtDBUSURIHandler* uriHandler) : QDBusAbstractAdaptor(uriHandler), uriHandler(uriHandler) {
            }

        public slots:
            void openURI(const QString& uri) {
                uriHandler->onURI(Q2PSTRING(uri));
            }

        private:
            QtDBUSURIHandler* uriHandler;
    };
}

QtDBUSURIHandler::QtDBUSURIHandler() {
    new DBUSAdaptor(this);
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.registerService("im.swift.Swift.URIHandler");
    connection.registerObject("/", this);
}

#pragma clang diagnostic pop

#include "QtDBUSURIHandler.moc"
