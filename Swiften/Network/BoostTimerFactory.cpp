/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Network/BoostTimerFactory.h"
#include "Swiften/Network/BoostTimer.h"

namespace Swift {

BoostTimerFactory::BoostTimerFactory(boost::asio::io_service* ioService) : ioService(ioService) {
}

boost::shared_ptr<Timer> BoostTimerFactory::createTimer(int milliseconds) {
	return boost::shared_ptr<Timer>(new BoostTimer(milliseconds, ioService));
}

}
