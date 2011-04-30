/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/RosterItemPayload.h>
#include <Swiften/Elements/Payload.h>


namespace Swift {
	class RosterPayload : public Payload {
		public:
			typedef boost::shared_ptr<RosterPayload> ref;
			typedef std::vector<RosterItemPayload> RosterItemPayloads;

		public:
			RosterPayload() {}

			boost::optional<RosterItemPayload> getItem(const JID& jid) const;

			void addItem(const RosterItemPayload& item) {
				items_.push_back(item);
			}

			const RosterItemPayloads& getItems() const {
				return items_;
			}

			const boost::optional<std::string>& getVersion() const {
				return version_;
			}

			void setVersion(const std::string& version) {
				version_ = version;
			}

		private:
			RosterItemPayloads items_;
			boost::optional<std::string> version_;
	};
}
