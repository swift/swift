/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <list>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDQuerier.h>

namespace Swift {
    class BonjourQuerier :
            public DNSSDQuerier,
            public boost::enable_shared_from_this<BonjourQuerier> {
        public:
            BonjourQuerier(EventLoop* eventLoop);
            ~BonjourQuerier();

            boost::shared_ptr<DNSSDBrowseQuery> createBrowseQuery();
            boost::shared_ptr<DNSSDRegisterQuery> createRegisterQuery(
                    const std::string& name, int port, const ByteArray& info);
            boost::shared_ptr<DNSSDResolveServiceQuery> createResolveServiceQuery(
                    const DNSSDServiceID&);
            boost::shared_ptr<DNSSDResolveHostnameQuery> createResolveHostnameQuery(
                    const std::string& hostname, int interfaceIndex);

            void start();
            void stop();

        private:
            friend class BonjourQuery;

            void addRunningQuery(boost::shared_ptr<BonjourQuery>);
            void removeRunningQuery(boost::shared_ptr<BonjourQuery>);
            void interruptSelect();
            void run();

        private:
            EventLoop* eventLoop;
            bool stopRequested;
            boost::thread* thread;
            boost::mutex runningQueriesMutex;
            std::list< boost::shared_ptr<BonjourQuery> > runningQueries;
            int interruptSelectReadSocket;
            int interruptSelectWriteSocket;
            boost::condition_variable runningQueriesAvailableEvent;
    };
}
