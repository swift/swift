/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Serializer/XML/XMLNode.h"
#include "Swiften/Base/Shared.h"

namespace Swift {
	class XMLTextNode : public XMLNode, public Shared<XMLTextNode> {
		public:
			typedef boost::shared_ptr<XMLTextNode> ref;

			XMLTextNode(const String& text) : text_(text) {
				text_.replaceAll('&', "&amp;"); // Should come first
				text_.replaceAll('<', "&lt;");
				text_.replaceAll('>', "&gt;");
			}

			String serialize() {
				return text_;
			}

			static ref create(const String& text) {
				return ref(new XMLTextNode(text));
			}

		private:
			String text_;
	};
}
