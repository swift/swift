/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubEventDisassociate.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class PayloadParserFactoryCollection;
	class PayloadParser;

	class SWIFTEN_API PubSubEventDisassociateParser : public GenericPayloadParser<PubSubEventDisassociate> {
		public:
			PubSubEventDisassociateParser(PayloadParserFactoryCollection* parsers);
			virtual ~PubSubEventDisassociateParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
			virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
			virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

		private:
			PayloadParserFactoryCollection* parsers;
			int level;
			boost::shared_ptr<PayloadParser> currentPayloadParser;
	};
}
