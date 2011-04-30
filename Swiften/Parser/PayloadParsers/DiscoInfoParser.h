/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
	class DiscoInfoParser : public GenericPayloadParser<DiscoInfo> {
		public:
			DiscoInfoParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
			virtual void handleEndElement(const std::string& element, const std::string&);
			virtual void handleCharacterData(const std::string& data);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1
			};
			int level_;
			FormParser* formParser_;
	};
}
