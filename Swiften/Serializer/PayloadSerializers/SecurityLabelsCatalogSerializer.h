#ifndef SWIFTEN_SecurityLabelsCatalogSerializer_H
#define SWIFTEN_SecurityLabelsCatalogSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/SecurityLabelsCatalog.h"

namespace Swift {
	class SecurityLabelsCatalogSerializer : public GenericPayloadSerializer<SecurityLabelsCatalog> {
		public:
			SecurityLabelsCatalogSerializer();

			virtual String serializePayload(boost::shared_ptr<SecurityLabelsCatalog> version)  const;
	};
}

#endif
