#ifndef SWIFTEN_PrioritySerializer_H
#define SWIFTEN_PrioritySerializer_H

#include <boost/lexical_cast.hpp>

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/Priority.h"

namespace Swift {
	class PrioritySerializer : public GenericPayloadSerializer<Priority> {
		public:
			PrioritySerializer() : GenericPayloadSerializer<Priority>() {}

			virtual String serializePayload(boost::shared_ptr<Priority> priority)  const {
				return "<priority>" + boost::lexical_cast<std::string>(priority->getPriority()) + "</priority>";
			}
	};
}

#endif
