/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QObject>
#include <SwifTools/URIHandler/URIHandler.h>

namespace Swift {
	class QtDBUSURIHandler : public QObject, public URIHandler {
		public:
			QtDBUSURIHandler();
	};
}
