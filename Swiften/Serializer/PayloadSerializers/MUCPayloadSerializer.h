/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_MUCPayloadSerializer_H
#define SWIFTEN_MUCPayloadSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/MUCPayload.h"

namespace Swift {
	class MUCPayloadSerializer : public GenericPayloadSerializer<MUCPayload> {
		public:
			MUCPayloadSerializer();

			virtual String serializePayload(boost::shared_ptr<MUCPayload> version)  const;
	};
}

#endif
