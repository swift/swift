/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

#include "Swiften/Base/String.h"

#include "Swiften/Serializer/XML/XMLNode.h"

namespace Swift {
	class XMLElement : public XMLNode {
		public:
			typedef boost::shared_ptr<XMLElement> ref;

			XMLElement(const String& tag, const String& xmlns = "");

			void setAttribute(const String& attribute, const String& value);
			void addNode(boost::shared_ptr<XMLNode> node);

			virtual String serialize();

		private:
			String tag_;
			std::map<String, String> attributes_;
			std::vector< boost::shared_ptr<XMLNode> > childNodes_;
	};
}
