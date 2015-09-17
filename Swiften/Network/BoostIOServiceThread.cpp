/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/BoostIOServiceThread.h>

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

BoostIOServiceThread::BoostIOServiceThread(boost::shared_ptr<boost::asio::io_service> ioService) {
	if (!!ioService) {
		ioService_ = ioService;
		thread_ = NULL;
	}
	else {
		ioService_ = boost::make_shared<boost::asio::io_service>();
		thread_ = new boost::thread(boost::bind(&BoostIOServiceThread::doRun, this));
	}
}

BoostIOServiceThread::~BoostIOServiceThread() {
	if (thread_) {
		ioService_->stop();
		thread_->join();
		delete thread_;
	}
}

void BoostIOServiceThread::doRun() {
	assert(thread_);
	boost::asio::io_service::work work(*ioService_);
	ioService_->run();
}

}
