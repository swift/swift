/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <utility> /* std::pair */
#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/StatusShow.h"

namespace Swift {
	typedef std::pair<StatusShow::Type, String> TypeStringPair;
	class PreviousStatusStore {
		public:
			PreviousStatusStore();
			~PreviousStatusStore();
			void addStatus(StatusShow::Type status, const String& message);
			std::vector<TypeStringPair> getSuggestions(const String& message);

		private:
			std::vector<TypeStringPair> exactMatchSuggestions(StatusShow::Type status, const String& message);
			std::vector<TypeStringPair> store_;
	};
}
