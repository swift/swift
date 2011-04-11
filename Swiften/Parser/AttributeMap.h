/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <string>
#include <map>
#include <boost/optional/optional_fwd.hpp>

#include <Swiften/Parser/Attribute.h>

namespace Swift {
	class AttributeMap {
		public:
			class Entry {
				public:
					Entry(const Attribute& attribute, const std::string& value) : attribute(attribute), value(value) {
					}

					const Attribute& getAttribute() const {
						return attribute;
					}

					const std::string& getValue() const {
						return value;
					}

				private:
					Attribute attribute;
					std::string value;
			};

			AttributeMap();

			std::string getAttribute(const std::string& attribute, const std::string& ns = "") const;
			bool getBoolAttribute(const std::string& attribute, bool defaultValue = false) const;
			boost::optional<std::string> getAttributeValue(const std::string&) const;

			void addAttribute(const std::string& name, const std::string& ns, const std::string& value);

			const std::vector<Entry>& getEntries() const {
				return attributes;
			}

		private:
			typedef std::vector<Entry> AttributeValueMap;
			AttributeValueMap attributes;
	};
}
