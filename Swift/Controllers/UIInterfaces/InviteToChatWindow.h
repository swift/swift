/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <string>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class InviteToChatWindow {
		public:
			virtual ~InviteToChatWindow() {};

			virtual std::string getReason() const = 0;

			virtual std::vector<JID> getJIDs() const = 0;

			boost::signal<void ()> onCompleted;
			boost::signal<void ()> onDismissed;
	};
}

