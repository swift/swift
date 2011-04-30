/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Elements/Storage.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class StorageParser : public GenericPayloadParser<Storage> {
		public:
			StorageParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			enum Level { 
				TopLevel = 0, 
				BookmarkLevel = 1,
				DetailLevel = 2
			};
			int level;
			std::string currentText;
			boost::optional<Storage::Room> room;
			boost::optional<Storage::URL> url;
	};
}
