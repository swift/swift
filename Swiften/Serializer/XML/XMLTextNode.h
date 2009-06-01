#ifndef SWIFTEN_XMLTextNode_H
#define SWIFTEN_XMLTextNode_H

#include "Swiften/Serializer/XML/XMLNode.h"

namespace Swift {
	class XMLTextNode : public XMLNode {
		public:
			XMLTextNode(const String& text) : text_(text) {
				text_.replaceAll('&', "&amp;"); // Should come first
				text_.replaceAll('<', "&lt;");
			}

			String serialize() {
				return text_;
			}

		private:
			String text_;
	};
}

#endif
