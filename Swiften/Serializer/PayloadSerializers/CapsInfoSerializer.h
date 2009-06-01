#ifndef SWIFTEN_CapsInfoSerializer_H
#define SWIFTEN_CapsInfoSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/CapsInfo.h"

namespace Swift {
	class CapsInfoSerializer : public GenericPayloadSerializer<CapsInfo> {
		public:
			CapsInfoSerializer();

			virtual String serializePayload(boost::shared_ptr<CapsInfo>)  const;
	};
}

#endif
