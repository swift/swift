/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Elements/CapsInfo.h"

namespace Swift {
	class DiscoInfo;

	class CapsInfoGenerator {
		public:
			CapsInfoGenerator(const String& node);

			CapsInfo generateCapsInfo(const DiscoInfo& discoInfo) const;

		private:
			String node_;
	};
}
