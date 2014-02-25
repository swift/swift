/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class SWIFTEN_API MAMArchived : public Payload {
		public:
			virtual ~MAMArchived();

			void setBy(const JID& by) { by_ = by; }
			const JID& getBy() const { return by_; }

			void setID(const std::string& id) { id_ = id; }
			const std::string& getID() const { return id_; }

		private:
			JID by_;
			std::string id_;
	};
}
