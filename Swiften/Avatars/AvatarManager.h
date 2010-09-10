/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include "Swiften/Base/boost_bsignals.h"

namespace Swift {
	class JID;

	class AvatarManager {
		public:
			virtual ~AvatarManager();

			virtual boost::filesystem::path getAvatarPath(const JID&) const = 0;

			boost::signal<void (const JID&)> onAvatarChanged;
	};
}
