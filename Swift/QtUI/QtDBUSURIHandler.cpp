/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtDBUSURIHandler.h"

#include <QDBusAbstractAdaptor>
#include <QDBusConnection>

#include "QtSwiftUtil.h"

using namespace Swift;

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

#include "QtDBUSURIHandler.moc"
