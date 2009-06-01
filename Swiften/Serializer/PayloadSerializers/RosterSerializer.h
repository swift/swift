#ifndef SWIFTEN_RosterSerializer_H
#define SWIFTEN_RosterSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/RosterPayload.h"

namespace Swift {
	class RosterSerializer : public GenericPayloadSerializer<RosterPayload> {
		public:
			RosterSerializer();

			virtual String serializePayload(boost::shared_ptr<RosterPayload>)  const;
	};
}

#endif
