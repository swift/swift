#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>

#include "Swiften/Elements/RosterItemPayload.h"
#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class LinkLocalService;
	class LinkLocalServiceBrowser;
	class RosterPayload;
	class Presence;

	class LinkLocalPresenceManager {
		public:
			LinkLocalPresenceManager(LinkLocalServiceBrowser*);

			boost::shared_ptr<RosterPayload> getRoster() const;
			std::vector<boost::shared_ptr<Presence> > getAllPresence() const;

			boost::optional<LinkLocalService> getServiceForJID(const JID&) const;

			boost::signal<void (boost::shared_ptr<RosterPayload>)> onRosterChanged;
			boost::signal<void (boost::shared_ptr<Presence>)> onPresenceChanged;

		private:
			void handleServiceAdded(const LinkLocalService&);
			void handleServiceChanged(const LinkLocalService&);
			void handleServiceRemoved(const LinkLocalService&);

			RosterItemPayload getRosterItem(const LinkLocalService& service) const;
			String getRosterName(const LinkLocalService& service) const;
			boost::shared_ptr<Presence> getPresence(const LinkLocalService& service) const;

		private:
			LinkLocalServiceBrowser* browser;
	};
}
