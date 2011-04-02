/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QObject>
#include <SwifTools/URIHandler/URIHandler.h>

class QUrl;

namespace Swift {
	class QtURIHandler : public QObject, public URIHandler {
		public:
			QtURIHandler();

			virtual void start() {
			}

			virtual void stop() {
			}

		private:
			bool eventFilter(QObject* obj, QEvent* event);
	};
}
