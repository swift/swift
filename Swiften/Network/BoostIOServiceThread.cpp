/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/BoostIOServiceThread.h>

#include <memory>

#include <boost/bind.hpp>

namespace Swift {

BoostIOServiceThread::BoostIOServiceThread(std::shared_ptr<boost::asio::io_service> ioService) {
    if (!!ioService) {
        ioService_ = ioService;
        thread_ = nullptr;
    }
    else {
        ioService_ = std::make_shared<boost::asio::io_service>();
        thread_ = new std::thread(boost::bind(&BoostIOServiceThread::doRun, this));
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
    boost::asio::io_service::work work(*ioService_);
    ioService_->run();
}

}
