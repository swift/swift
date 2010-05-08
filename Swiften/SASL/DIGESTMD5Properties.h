/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <boost/optional.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class DIGESTMD5Properties {
		public:
			DIGESTMD5Properties();
			
			boost::optional<String> getValue(const String& key) const;

			void setValue(const String& key, const String& value);

			ByteArray serialize() const;

			static DIGESTMD5Properties parse(const ByteArray&);

		private:
			static bool isQuoted(const String& property);

		private:
			typedef std::multimap<String, ByteArray> DIGESTMD5PropertiesMap;
			DIGESTMD5PropertiesMap properties;
	};
}
