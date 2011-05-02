/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
	class ErrorSerializer : public GenericPayloadSerializer<ErrorPayload> {
		public:
			ErrorSerializer();

			virtual std::string serializePayload(boost::shared_ptr<ErrorPayload> error)  const;
	};
}
