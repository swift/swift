/*
 * Copyright (c) 2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/noncopyable.hpp>

#include <Swiften/Parser/XMLParserClient.h>
#include <Swiften/Parser/AttributeMap.h>

namespace Swift {
	class XMLParser;

	class BOSHParser : public XMLParserClient, boost::noncopyable {
		public:
			BOSHParser();
			~BOSHParser();

			bool parse(const std::string&);

			std::string getAttribute(const std::string& attribute, const std::string& ns = "") const {
				return boshBodyAttributes_.getAttribute(attribute, ns);
			}
		private:
			virtual void handleStartElement(
					const std::string& element,
					const std::string& ns,
					const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string& ns);
			virtual void handleCharacterData(const std::string& data);

		private:
			AttributeMap boshBodyAttributes_;
			XMLParser* xmlParser_;
			enum Level {
				BoshTopLevel = -1,
				TopLevel = 0,
				StreamLevel = 1,
				ElementLevel = 2
			};
			int level_;
			bool parseErrorOccurred_;
	};
}
