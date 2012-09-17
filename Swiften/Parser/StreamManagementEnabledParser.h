/*
 * Copyright (c) 2010-2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Base/API.h>

namespace Swift {
	class SWIFTEN_API StreamManagementEnabledParser : public GenericElementParser<StreamManagementEnabled> {
		public:
			StreamManagementEnabledParser();
			~StreamManagementEnabledParser();

			virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&);
			virtual void handleEndElement(const std::string&, const std::string&);

		private:
			enum Level { 
				TopLevel = 0
			};
			int level;
	};
}
