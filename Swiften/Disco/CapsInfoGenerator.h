#ifndef SWIFTEN_CapsInfoGenerator_H
#define SWIFTEN_CapsInfoGenerator_H

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

#endif
