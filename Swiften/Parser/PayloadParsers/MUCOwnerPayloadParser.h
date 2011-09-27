/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Elements/MUCOwnerPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class PayloadParserFactoryCollection;

	class MUCOwnerPayloadParser : public GenericPayloadParser<MUCOwnerPayload> {
		public:
		MUCOwnerPayloadParser(PayloadParserFactoryCollection* factories);

		private:
			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			PayloadParserFactoryCollection* factories;
			int level;
			boost::shared_ptr<PayloadParser> currentPayloadParser;
	};
}
