/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericPayloadParser.h>
#include <Swiften/Elements/UserLocation.h>

namespace Swift {
	class SWIFTEN_API UserLocationParser : public GenericPayloadParser<UserLocation> {
		public:
			UserLocationParser();
			virtual ~UserLocationParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
			virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
			virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

		private:
			int level;
			std::string currentText;
	};
}
