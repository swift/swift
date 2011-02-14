/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_StatusSerializer_H
#define SWIFTEN_StatusSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Serializer/XML/XMLElement.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"
#include "Swiften/Elements/Status.h"

namespace Swift {
	class StatusSerializer : public GenericPayloadSerializer<Status> {
		public:
			StatusSerializer() : GenericPayloadSerializer<Status>() {}

			virtual std::string serializePayload(boost::shared_ptr<Status> status)  const {
				XMLElement element("status");
				element.addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(status->getText())));
				return element.serialize();
			}
	};
}

#endif
