/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include "Swiften/Elements/Storage.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class StorageParser : public GenericPayloadParser<Storage> {
		public:
			StorageParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			enum Level { 
				TopLevel = 0, 
				BookmarkLevel = 1,
				DetailLevel = 2
			};
			int level;
			String currentText;
			boost::optional<Storage::Conference> conference;
	};
}
