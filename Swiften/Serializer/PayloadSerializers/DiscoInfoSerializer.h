#ifndef SWIFTEN_DiscoInfoSerializer_H
#define SWIFTEN_DiscoInfoSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/DiscoInfo.h"

namespace Swift {
	class DiscoInfoSerializer : public GenericPayloadSerializer<DiscoInfo> {
		public:
			DiscoInfoSerializer();

			virtual String serializePayload(boost::shared_ptr<DiscoInfo>)  const;
	};
}

#endif
