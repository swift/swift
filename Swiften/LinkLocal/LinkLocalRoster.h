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

		private:
			RosterItemPayload getRosterItem(const DNSSDService::Service& service, const DNSSDService::ResolveResult& info) const;
			String getRosterName(const DNSSDService::Service& service, const DNSSDService::ResolveResult& info) const;
			JID getJIDForService(const DNSSDService::Service& service) const;
			boost::shared_ptr<Presence> getPresence(const DNSSDService::Service& service, const DNSSDService::ResolveResult& info) const;

			void handleStopped(bool);
			void handleServiceRegistered(const DNSSDService::Service& service);
			void handleServiceAdded(const DNSSDService::Service&);
			void handleServiceRemoved(const DNSSDService::Service&);
			void handleServiceResolved(const DNSSDService::Service& service, const DNSSDService::ResolveResult& result);

		private:
			boost::shared_ptr<DNSSDService> dnsSDService;
			boost::optional<DNSSDService::Service> selfService;
			typedef std::map<DNSSDService::Service, DNSSDService::ResolveResult> ServiceMap;
			ServiceMap services;
	};
}
