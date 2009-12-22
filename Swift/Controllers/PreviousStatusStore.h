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
