#include "Swiften/Network/BoostTimerFactory.h"
#include "Swiften/Network/BoostTimer.h"

namespace Swift {

BoostTimerFactory::BoostTimerFactory(boost::asio::io_service* ioService) : ioService(ioService) {
}

boost::shared_ptr<Timer> BoostTimerFactory::createTimer(int milliseconds) {
	return boost::shared_ptr<Timer>(new BoostTimer(milliseconds, ioService));
}

}
