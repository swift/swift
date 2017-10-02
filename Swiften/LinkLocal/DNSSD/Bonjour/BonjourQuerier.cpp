/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuerier.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourBrowseQuery.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourRegisterQuery.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourResolveHostnameQuery.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourResolveServiceQuery.h>

namespace Swift {

BonjourQuerier::BonjourQuerier(EventLoop* eventLoop) : eventLoop(eventLoop), stopRequested(false), thread(nullptr) {
    int fds[2];
    int result = pipe(fds);
    assert(result == 0);
    (void) result;
    interruptSelectReadSocket = fds[0];
    fcntl(interruptSelectReadSocket, F_SETFL, fcntl(interruptSelectReadSocket, F_GETFL)|O_NONBLOCK);
    interruptSelectWriteSocket = fds[1];
}

BonjourQuerier::~BonjourQuerier() {
    assert(!thread);
}

std::shared_ptr<DNSSDBrowseQuery> BonjourQuerier::createBrowseQuery() {
    return std::make_shared<BonjourBrowseQuery>(shared_from_this(), eventLoop);
}

std::shared_ptr<DNSSDRegisterQuery> BonjourQuerier::createRegisterQuery(const std::string& name, int port, const ByteArray& info) {
    return std::make_shared<BonjourRegisterQuery>(name, port, info, shared_from_this(), eventLoop);
}

std::shared_ptr<DNSSDResolveServiceQuery> BonjourQuerier::createResolveServiceQuery(const DNSSDServiceID& service) {
    return std::make_shared<BonjourResolveServiceQuery>(service, shared_from_this(), eventLoop);
}

std::shared_ptr<DNSSDResolveHostnameQuery> BonjourQuerier::createResolveHostnameQuery(const std::string& hostname, int interfaceIndex) {
    return std::make_shared<BonjourResolveHostnameQuery>(hostname, interfaceIndex, shared_from_this(), eventLoop);
}

void BonjourQuerier::addRunningQuery(std::shared_ptr<BonjourQuery> query) {
    {
        std::lock_guard<std::mutex> lock(runningQueriesMutex);
        runningQueries.push_back(query);
    }
    runningQueriesAvailableEvent.notify_one();
    interruptSelect();
}

void BonjourQuerier::removeRunningQuery(std::shared_ptr<BonjourQuery> query) {
    {
        std::lock_guard<std::mutex> lock(runningQueriesMutex);
        erase(runningQueries, query);
    }
}

void BonjourQuerier::interruptSelect() {
    char c = 0;
    write(interruptSelectWriteSocket, &c, 1);
}

void BonjourQuerier::start() {
    assert(!thread);
    thread = new std::thread(boost::bind(&BonjourQuerier::run, shared_from_this()));
}

void BonjourQuerier::stop() {
    if (thread) {
        stopRequested = true;
        assert(runningQueries.empty());
        runningQueriesAvailableEvent.notify_one();
        interruptSelect();
        thread->join();
        delete thread;
        thread = nullptr;
        stopRequested = false;
    }
}

void BonjourQuerier::run() {
    while (!stopRequested) {
        fd_set fdSet;
        int maxSocket;
        {
            std::unique_lock<std::mutex> lock(runningQueriesMutex);
            if (runningQueries.empty()) {
                runningQueriesAvailableEvent.wait(lock);
                if (runningQueries.empty()) {
                    continue;
                }
            }

            // Run all running queries
            FD_ZERO(&fdSet);
            maxSocket = interruptSelectReadSocket;
            FD_SET(interruptSelectReadSocket, &fdSet);

            for (const auto& query : runningQueries) {
                int socketID = query->getSocketID();
                maxSocket = std::max(maxSocket, socketID);
                FD_SET(socketID, &fdSet);
            }
        }

        if (select(maxSocket+1, &fdSet, nullptr, nullptr, nullptr) <= 0) {
            continue;
        }

        if (FD_ISSET(interruptSelectReadSocket, &fdSet)) {
            char dummy;
            while (read(interruptSelectReadSocket, &dummy, 1) > 0) {}
        }

        {
            std::lock_guard<std::mutex> lock(runningQueriesMutex);
            for (auto&& query : runningQueries) {
                if (FD_ISSET(query->getSocketID(), &fdSet)) {
                    query->processResult();
                }
            }
        }
    }
}

}
