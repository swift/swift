/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
