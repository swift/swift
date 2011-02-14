/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef ATTRIBUTEMAP_H
#define ATTRIBUTEMAP_H

#include <map>

#include <string>

namespace Swift {
	class AttributeMap : public std::map<std::string,std::string> {
		public:
			AttributeMap() {}

			std::string getAttribute(const std::string& attribute) const {
				AttributeMap::const_iterator i = find(attribute);
				if (i == end()) {
					return "";
				}
				else {
					return i->second;
				}
			}

			bool getBoolAttribute(const std::string& attribute, bool defaultValue = false) const {
				AttributeMap::const_iterator i = find(attribute);
				if (i == end()) {
					return defaultValue;
				}
				else {
					return i->second == "true" || i->second == "1";
				}
			}
	};
}

#endif
