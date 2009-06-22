#pragma once

namespace Swift {
	class JID;

	class MUCRegistry {
		public:
			virtual ~MUCRegistry();

			virtual bool isMUC(const JID&) const = 0;
	};
}
