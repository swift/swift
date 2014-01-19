/*
 * Copyright (c) 2014 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericPayloadParser.h>
#include <Swiften/Elements/UserTune.h>

namespace Swift {
	class SWIFTEN_API UserTuneParser : public GenericPayloadParser<UserTune> {
		public:
			UserTuneParser();
			virtual ~UserTuneParser();

			virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
			virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
			virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

		private:
			int level;
			std::string currentText;
	};
}
