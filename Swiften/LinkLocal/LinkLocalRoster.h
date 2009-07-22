#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <set>

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/LinkLocal/DNSSDService.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Elements/Presence.h"

namespace Swift {
	class HostAddress;

	class LinkLocalRoster {
		public:
			LinkLocalRoster(boost::shared_ptr<DNSSDService> service);

			boost::shared_ptr<RosterPayload> getRoster() const;
			std::vector<boost::shared_ptr<Presence> > getAllPresence() const;

			boost::signal<void (boost::shared_ptr<RosterPayload>)> onRosterChanged;
			boost::signal<void (boost::shared_ptr<Presence>)> onPresenceChanged;

			bool hasItem(const JID&) const;
			String getHostname(const JID&) const;
			int getPort(const JID&) const;

		private:
			RosterItemPayload getRosterItem(const LinkLocalServiceID& service, const DNSSDService::ResolveResult& info) const;
			String getRosterName(const LinkLocalServiceID& service, const DNSSDService::ResolveResult& info) const;
			JID getJIDForService(const LinkLocalServiceID& service) const;
			boost::shared_ptr<Presence> getPresence(const LinkLocalServiceID& service, const DNSSDService::ResolveResult& info) const;

			void handleStopped(bool);
			void handleServiceRegistered(const LinkLocalServiceID& service);
			void handleServiceAdded(const LinkLocalServiceID&);
			void handleServiceRemoved(const LinkLocalServiceID&);
			void handleServiceResolved(const LinkLocalServiceID& service, const DNSSDService::ResolveResult& result);

		private:
			boost::shared_ptr<DNSSDService> dnsSDService;
			boost::optional<LinkLocalServiceID> selfService;
			typedef std::map<LinkLocalServiceID, DNSSDService::ResolveResult> ServiceMap;
			ServiceMap services;
	};
}
