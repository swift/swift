/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/StatusUtil.h>

#include <Swift/Controllers/Intl.h>

namespace Swift {

std::string statusShowTypeToFriendlyName(StatusShow::Type type) {
	switch (type) {
		case StatusShow::Online: return QT_TRANSLATE_NOOP("", "Available");
		case StatusShow::FFC: return QT_TRANSLATE_NOOP("", "Available");
		case StatusShow::Away: return QT_TRANSLATE_NOOP("", "Away");
		case StatusShow::XA: return QT_TRANSLATE_NOOP("", "Away");
		case StatusShow::DND: return QT_TRANSLATE_NOOP("", "Busy");
		case StatusShow::None: return QT_TRANSLATE_NOOP("", "Offline");
	}
	return "";
}

}
