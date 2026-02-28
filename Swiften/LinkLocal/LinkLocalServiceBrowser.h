/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDQuerier.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDServiceID.h>
#include <Swiften/LinkLocal/LinkLocalService.h>
#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>

namespace Swift {
    class SWIFTEN_API LinkLocalServiceBrowser {
        public:
            LinkLocalServiceBrowser(std::shared_ptr<DNSSDQuerier> querier);
            ~LinkLocalServiceBrowser();

            void start();
            void stop();
            bool isRunning() const;
            bool hasError() const;

            void registerService(
                    const std::string& name,
                    unsigned short port,
                    const LinkLocalServiceInfo& info = LinkLocalServiceInfo());
            void updateService(
                    const LinkLocalServiceInfo& info = LinkLocalServiceInfo());
            void unregisterService();
            bool isRegistered() const;

            std::vector<LinkLocalService> getServices() const;

            // FIXME: Ugly that we need this
            std::shared_ptr<DNSSDQuerier> getQuerier() const {
                return querier;
            }

            boost::signals2::signal<void (const LinkLocalService&)> onServiceAdded;
            boost::signals2::signal<void (const LinkLocalService&)> onServiceChanged;
            boost::signals2::signal<void (const LinkLocalService&)> onServiceRemoved;
            boost::signals2::signal<void (const DNSSDServiceID&)> onServiceRegistered;
            boost::signals2::signal<void (bool)> onStopped;

        private:
            void handleServiceAdded(const DNSSDServiceID&);
            void handleServiceRemoved(const DNSSDServiceID&);
            void handleServiceResolved(const DNSSDServiceID& service, const boost::optional<DNSSDResolveServiceQuery::Result>& result);
            void handleRegisterFinished(const boost::optional<DNSSDServiceID>&);
            void handleBrowseError();

        private:
            std::shared_ptr<DNSSDQuerier> querier;
            boost::optional<DNSSDServiceID> selfService;
            std::shared_ptr<DNSSDBrowseQuery> browseQuery;
            std::shared_ptr<DNSSDRegisterQuery> registerQuery;
            typedef std::map<DNSSDServiceID, std::shared_ptr<DNSSDResolveServiceQuery> > ResolveQueryMap;
            ResolveQueryMap resolveQueries;
            typedef std::map<DNSSDServiceID, DNSSDResolveServiceQuery::Result> ServiceMap;
            ServiceMap services;
            bool haveError;
    };
}
