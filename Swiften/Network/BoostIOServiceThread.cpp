#include "Swiften/Network/BoostIOServiceThread.h"

namespace Swift {

BoostIOServiceThread::BoostIOServiceThread() : thread_(boost::bind(&BoostIOServiceThread::doRun, this)) {
}

BoostIOServiceThread::~BoostIOServiceThread() {
	ioService_.stop();
	thread_.join();
}

void BoostIOServiceThread::doRun() {
	boost::asio::io_service::work work(ioService_);
	ioService_.run();
}

}
