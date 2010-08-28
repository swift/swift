/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/SessionManagementEnabled.h"
#include "Swiften/Serializer/GenericElementSerializer.h"
#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {
	class SessionManagementEnabledSerializer : public GenericElementSerializer<SessionManagementEnabled> {
		public:
			SessionManagementEnabledSerializer() : GenericElementSerializer<SessionManagementEnabled>() {
			}

			virtual String serialize(boost::shared_ptr<Element>) const {
				return XMLElement("enabled", "urn:xmpp:sm:2").serialize();
			}
	};
}
