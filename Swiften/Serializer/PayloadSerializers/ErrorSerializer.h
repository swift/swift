#ifndef SWIFTEN_ErrorSerializer_H
#define SWIFTEN_ErrorSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/Error.h"

namespace Swift {
	class ErrorSerializer : public GenericPayloadSerializer<Error> {
		public:
			ErrorSerializer();

			virtual String serializePayload(boost::shared_ptr<Error> error)  const;
	};
}

#endif
