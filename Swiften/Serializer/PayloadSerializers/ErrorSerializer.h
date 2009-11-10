#ifndef SWIFTEN_ErrorSerializer_H
#define SWIFTEN_ErrorSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/ErrorPayload.h"

namespace Swift {
	class ErrorSerializer : public GenericPayloadSerializer<ErrorPayload> {
		public:
			ErrorSerializer();

			virtual String serializePayload(boost::shared_ptr<ErrorPayload> error)  const;
	};
}

#endif
