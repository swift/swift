#ifndef ATTRIBUTEMAP_H
#define ATTRIBUTEMAP_H

#include <map>

#include "Swiften/Base/String.h"

namespace Swift {
	class AttributeMap : public std::map<String,String> {
		public:
			AttributeMap() {}

			String getAttribute(const String& attribute) const {
				AttributeMap::const_iterator i = find(attribute);
				if (i == end()) {
					return "";
				}
				else {
					return i->second;
				}
			}
	};
}

#endif
