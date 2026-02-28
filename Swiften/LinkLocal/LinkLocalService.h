/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDServiceID.h>
#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>

namespace Swift {
    class SWIFTEN_API LinkLocalService {
        public:
            LinkLocalService(
                    const DNSSDServiceID& id,
                    const DNSSDResolveServiceQuery::Result& info) :
                        id(id),
                        info(info) {}

            const DNSSDServiceID& getID() const {
                return id;
            }

            const std::string& getName() const {
                return id.getName();
            }

            unsigned short getPort() const {
                return info.port;
            }

            const std::string& getHostname() const {
                return info.host;
            }

            LinkLocalServiceInfo getInfo() const {
                return LinkLocalServiceInfo::createFromTXTRecord(info.info);
            }

            std::string getDescription() const;

            JID getJID() const;

        private:
            DNSSDServiceID id;
            DNSSDResolveServiceQuery::Result info;
    };
}
