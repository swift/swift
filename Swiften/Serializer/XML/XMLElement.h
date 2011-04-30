/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

#include <string>

#include <Swiften/Serializer/XML/XMLNode.h>

namespace Swift {
	class XMLElement : public XMLNode {
		public:
			typedef boost::shared_ptr<XMLElement> ref;

			XMLElement(const std::string& tag, const std::string& xmlns = "", const std::string& text = "");

			void setAttribute(const std::string& attribute, const std::string& value);
			void addNode(boost::shared_ptr<XMLNode> node);

			virtual std::string serialize();

		private:
			std::string tag_;
			std::map<std::string, std::string> attributes_;
			std::vector< boost::shared_ptr<XMLNode> > childNodes_;
	};
}
