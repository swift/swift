/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/JID/JID.h>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class BlockList {
		public:
			enum State {
				Init,
				Requesting,
				Available,
				Error
			};
			virtual ~BlockList();

			virtual State getState() const = 0;

			virtual const std::vector<JID>& getItems() const = 0;

			bool isBlocked(const JID& jid) const;

		public:
			boost::signal<void ()> onStateChanged;
			boost::signal<void (const JID&)> onItemAdded;
			boost::signal<void (const JID&)> onItemRemoved;
	};
}
