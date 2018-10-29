/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <list>
#include <memory>
#include <set>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDQuerier.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
    class FakeDNSSDQuery;
    class EventLoop;

    class SWIFTEN_API FakeDNSSDQuerier :
            public DNSSDQuerier,
            public EventOwner,
            public std::enable_shared_from_this<FakeDNSSDQuerier> {
        public:
            FakeDNSSDQuerier(const std::string& domain, EventLoop* eventLoop);
            ~FakeDNSSDQuerier();

            void start() {}
            void stop() {}

            void clearAllQueriesEverRun() {
                allQueriesEverRun.clear();
            }

            std::shared_ptr<DNSSDBrowseQuery> createBrowseQuery();
            std::shared_ptr<DNSSDRegisterQuery> createRegisterQuery(
                    const std::string& name, unsigned short port, const ByteArray& info);
            std::shared_ptr<DNSSDResolveServiceQuery> createResolveServiceQuery(
                    const DNSSDServiceID&);
            std::shared_ptr<DNSSDResolveHostnameQuery> createResolveHostnameQuery(
                    const std::string& hostname, int interfaceIndex);

            void addRunningQuery(std::shared_ptr<FakeDNSSDQuery>);
            void removeRunningQuery(std::shared_ptr<FakeDNSSDQuery>);

            void addService(const DNSSDServiceID& id);
            void removeService(const DNSSDServiceID& id);
            void setServiceInfo(const DNSSDServiceID& id, const DNSSDResolveServiceQuery::Result& info);
            bool isServiceRegistered(const std::string& name, unsigned short port, const ByteArray& info);
            void setAddress(const std::string& hostname, boost::optional<HostAddress> address);

            void setBrowseError();
            void setRegisterError();

        public:
            template<typename T>
            std::vector< std::shared_ptr<T> > getAllQueriesEverRun() const {
                std::vector< std::shared_ptr<T> > result;
                for (const auto& i : allQueriesEverRun) {
                    if (std::shared_ptr<T> resultQuery = std::dynamic_pointer_cast<T>(i)) {
                        result.push_back(resultQuery);
                    }
                }
                return result;
            }

        private:
            template<typename T>
            std::vector< std::shared_ptr<T> > getQueries() const {
                std::vector< std::shared_ptr<T> > result;
                for (const auto& runningQuerie : runningQueries) {
                    if (std::shared_ptr<T> resultQuery = std::dynamic_pointer_cast<T>(runningQuerie)) {
                        result.push_back(resultQuery);
                    }
                }
                return result;
            }

        private:
            std::string domain;
            EventLoop* eventLoop;
            typedef std::list< std::shared_ptr<FakeDNSSDQuery> > QueryList;
            QueryList runningQueries;
            QueryList allQueriesEverRun;
            std::set<DNSSDServiceID> services;
            typedef std::map<DNSSDServiceID,DNSSDResolveServiceQuery::Result> ServiceInfoMap;
            ServiceInfoMap serviceInfo;
            std::map<std::string, boost::optional<HostAddress> > addresses;
    };
}
