#ifndef SWIFTEN_XMLNode_H
#define SWIFTEN_XMLNode_H

#include "Swiften/Base/String.h"

namespace Swift {
	class XMLNode {
		public:
			virtual ~XMLNode();

			virtual String serialize() = 0;
	};
}

#endif
