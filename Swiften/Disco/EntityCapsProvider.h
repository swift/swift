/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/DiscoInfo.h>

namespace Swift {
	/**
	 * This class provides information about capabilities of entities on the network.
	 * This information is provided in the form of service discovery
	 * information.
	 */
	class EntityCapsProvider {
		public:
			virtual ~EntityCapsProvider();

			/**
			 * Returns the service discovery information of the given JID.
			 */
			virtual DiscoInfo::ref getCaps(const JID&) const = 0;

			/**
			 * Emitted when the capabilities of a JID changes.
			 */
			boost::signal<void (const JID&)> onCapsChanged;
	};
}
