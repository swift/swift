#ifndef SWIFTEN_XMLRawTextNode_H
#define SWIFTEN_XMLRawTextNode_H

#include "Swiften/Serializer/XML/XMLNode.h"

namespace Swift {
	class XMLRawTextNode : public XMLNode {
		public:
			XMLRawTextNode(const String& text) : text_(text) {
			}

			String serialize() {
				return text_;
			}

		private:
			String text_;
	};
}

#endif
