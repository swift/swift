/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class JID;

	class AvatarProvider {
		public:
			virtual ~AvatarProvider();

			virtual String getAvatarHash(const JID&) const = 0;

			boost::signal<void (const JID&)> onAvatarChanged;
	};
}
