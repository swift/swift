/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <thread>

#include <boost/bind.hpp>

#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/BoostIOServiceThread.h>
#include <Swiften/Network/BoostTimer.h>
#include <Swiften/Network/MainBoostIOServiceThread.h>
#include <Swiften/Roster/GetRosterRequest.h>

using namespace Swift;

using namespace Swift;

bool connecting_ = false;
Client* client_;
SimpleEventLoop eventLoop_;
int count = 0;

void handleTick(std::shared_ptr<BoostTimer> timer) {
    std::cout << "Count " << count++ << std::endl;
    if (timer) {
        timer->stop();
    }
    if (connecting_) {
        client_->disconnect();
    } else {
        if (count > 60) {
            eventLoop_.stop();
            return;
        }
        client_->connect();
    }
    connecting_ = !connecting_;

    int delay = 500;
//    int delay = 0;
    std::shared_ptr<BoostTimer> newTimer(BoostTimer::create(delay, &MainBoostIOServiceThread::getInstance().getIOService()));
    newTimer->onTick.connect(boost::bind(&handleTick, timer));
    newTimer->start();
}

int main(int, char**) {
    char* jidChars = getenv("SWIFT_CLIENTTEST_JID");
    if (!jidChars) {
        std::cerr << "Please set the SWIFT_CLIENTTEST_JID environment variable" << std::endl;
        return -1;
    }
    char* passChars = getenv("SWIFT_CLIENTTEST_PASS");
    if (!passChars) {
        std::cerr << "Please set the SWIFT_CLIENTTEST_PASS environment variable" << std::endl;
        return -1;
    }

    JID jid(jidChars);
    std::string pass(passChars);

    client_ = new Swift::Client(jid, pass);
    handleTick(std::shared_ptr<BoostTimer>());
    eventLoop_.run();

    delete client_;
    return 0;

}
