#ifndef SWIFTEN_XMLElement_H
#define SWIFTEN_XMLElement_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

#include "Swiften/Base/String.h"
#include "Swiften/Serializer/XML/XMLNode.h"

namespace Swift {
	class XMLElement : public XMLNode {
		public:
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
#endif
