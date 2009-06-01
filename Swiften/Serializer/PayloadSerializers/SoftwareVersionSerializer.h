#ifndef SWIFTEN_SoftwareVersionSerializer_H
#define SWIFTEN_SoftwareVersionSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/SoftwareVersion.h"

namespace Swift {
	class SoftwareVersionSerializer : public GenericPayloadSerializer<SoftwareVersion> {
		public:
			SoftwareVersionSerializer();

			virtual String serializePayload(boost::shared_ptr<SoftwareVersion> version)  const;
	};
}

#endif
