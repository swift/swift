/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/XML/XMLNode.h>

namespace Swift {
	class XMLRawTextNode : public XMLNode {
		public:
			XMLRawTextNode(const std::string& text) : text_(text) {
			}

			std::string serialize() {
				return text_;
			}

		private:
			std::string text_;
	};
}
