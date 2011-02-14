/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_PrioritySerializer_H
#define SWIFTEN_PrioritySerializer_H

#include <boost/lexical_cast.hpp>

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/Priority.h"

namespace Swift {
	class PrioritySerializer : public GenericPayloadSerializer<Priority> {
		public:
			PrioritySerializer() : GenericPayloadSerializer<Priority>() {}

			virtual std::string serializePayload(boost::shared_ptr<Priority> priority)  const {
				return "<priority>" + boost::lexical_cast<std::string>(priority->getPriority()) + "</priority>";
			}
	};
}

#endif
