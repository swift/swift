/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericPayloadParser.h>
#include <Swiften/Elements/MAMFin.h>

namespace Swift {
	class ResultSetParser;

	class SWIFTEN_API MAMFinParser : public GenericPayloadParser<MAMFin> {
		public:
			MAMFinParser();

			virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
			virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
			virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

			enum Level {
				TopLevel = 0, 
				PayloadLevel = 1
			};

		private:
			boost::shared_ptr<ResultSetParser> resultSetParser_;
			int level_;
	};
}
