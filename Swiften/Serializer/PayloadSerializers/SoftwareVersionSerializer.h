/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_SoftwareVersionSerializer_H
#define SWIFTEN_SoftwareVersionSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/SoftwareVersion.h"

namespace Swift {
	class SoftwareVersionSerializer : public GenericPayloadSerializer<SoftwareVersion> {
		public:
			SoftwareVersionSerializer();

			virtual std::string serializePayload(boost::shared_ptr<SoftwareVersion> version)  const;
	};
}

#endif
