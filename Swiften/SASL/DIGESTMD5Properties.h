/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <boost/optional.hpp>

#include <string>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class DIGESTMD5Properties {
		public:
			DIGESTMD5Properties();
			
			boost::optional<std::string> getValue(const std::string& key) const;

			void setValue(const std::string& key, const std::string& value);

			ByteArray serialize() const;

			static DIGESTMD5Properties parse(const ByteArray&);

		private:
			static bool isQuoted(const std::string& property);

		private:
			typedef std::multimap<std::string, ByteArray> DIGESTMD5PropertiesMap;
			DIGESTMD5PropertiesMap properties;
	};
}
