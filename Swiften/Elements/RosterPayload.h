#ifndef SWIFTEN_RosterPayload_H
#define SWIFTEN_RosterPayload_H

#include <vector>
#include <boost/optional.hpp>

#include "Swiften/Elements/RosterItemPayload.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class RosterPayload : public Payload {
		public:
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

		private:
			RosterItemPayloads items_;
	};
}

#endif
