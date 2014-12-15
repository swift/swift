/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
