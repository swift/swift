#ifndef SWIFTEN_IDGenerator_H
#define SWIFTEN_IDGenerator_H

#include "Swiften/Base/String.h"

namespace Swift {
	class IDGenerator {
		public:
			IDGenerator();

			String generateID();

		private:
			String currentID_;
	};
}

#endif
