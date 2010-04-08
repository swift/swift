/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Network/MainBoostIOServiceThread.h"

#include "Swiften/Network/BoostIOServiceThread.h"

namespace Swift {

BoostIOServiceThread& MainBoostIOServiceThread::getInstance() {
	static BoostIOServiceThread instance;
	return instance;
}

}
