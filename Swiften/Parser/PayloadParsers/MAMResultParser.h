/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MAMResult.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class PayloadParserFactoryCollection;
	class ForwardedParser;

	class SWIFTEN_API MAMResultParser : public GenericPayloadParser<MAMResult> {
		public:
			MAMResultParser(PayloadParserFactoryCollection* factories);

			virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
			virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
			virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

			enum Level {
				TopLevel = 0, 
				PayloadLevel = 1
			};

		private:
			boost::shared_ptr<ForwardedParser> payloadParser_;
			PayloadParserFactoryCollection* factories_;
			int level_;
	};
}
