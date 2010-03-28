#ifndef SWIFTEN_SecurityLabelSerializer_H
#define SWIFTEN_SecurityLabelSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/SecurityLabel.h"

namespace Swift {
	class SecurityLabelSerializer : public GenericPayloadSerializer<SecurityLabel> {
		public:
			SecurityLabelSerializer();

			virtual String serializePayload(boost::shared_ptr<SecurityLabel> version)  const;
	};
}

#endif
