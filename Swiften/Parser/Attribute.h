/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

namespace Swift {
	class Attribute {
		public:
			Attribute(const std::string& name, const std::string& ns) : name(name), ns(ns) {
			}

			const std::string& getName() const {
				return name;
			}

			const std::string& getNamespace() const {
				return ns;
			}

			bool operator==(const Attribute& o) const {
				return o.name == name && o.ns == ns;
			}

		private:
			std::string name;
			std::string ns;
	};
}
