/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/XML/XMLNode.h>
#include <Swiften/Base/String.h>

namespace Swift {
	class XMLTextNode : public XMLNode {
		public:
			typedef boost::shared_ptr<XMLTextNode> ref;

			XMLTextNode(const std::string& text) : text_(text) {
				String::replaceAll(text_, '&', "&amp;"); // Should come first
				String::replaceAll(text_, '<', "&lt;");
				String::replaceAll(text_, '>', "&gt;");
			}

			std::string serialize() {
				return text_;
			}

			static ref create(const std::string& text) {
				return ref(new XMLTextNode(text));
			}

		private:
			std::string text_;
	};
}
