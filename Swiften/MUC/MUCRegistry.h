/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/JID/JID.h>

namespace Swift {
	class JID;

	class MUCRegistry {
		public:
			~MUCRegistry();

			bool isMUC(const JID& j) const;
			void addMUC(const JID& j);
			void removeMUC(const JID& j);

		private:
			std::vector<JID> mucs;
	};
}
