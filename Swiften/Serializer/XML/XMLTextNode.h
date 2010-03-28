#pragma once

#include "Swiften/Serializer/XML/XMLNode.h"

namespace Swift {
	class XMLTextNode : public XMLNode {
		public:
			XMLTextNode(const String& text) : text_(text) {
				text_.replaceAll('&', "&amp;"); // Should come first
				text_.replaceAll('<', "&lt;");
				text_.replaceAll('>', "&gt;");
			}

			String serialize() {
				return text_;
			}

		private:
			String text_;
	};
}
