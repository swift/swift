/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDServiceID.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h>
#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>

namespace Swift {
	class LinkLocalService {
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

			int getPort() const {
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
