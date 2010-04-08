/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include "Swiften/Elements/PrivateStorage.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class PayloadParserFactoryCollection;

	class PrivateStorageParser : public GenericPayloadParser<PrivateStorage> {
		public:
			PrivateStorageParser(PayloadParserFactoryCollection* factories);

		private:
			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			PayloadParserFactoryCollection* factories;
			int level;
			std::auto_ptr<PayloadParser> currentPayloadParser;
	};
}
