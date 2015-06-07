/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CompressFailure.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	class SWIFTEN_API CompressFailureSerializer : public GenericElementSerializer<CompressFailure> {
		public:
			CompressFailureSerializer() : GenericElementSerializer<CompressFailure>() {
			}

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement>) const {
				return createSafeByteArray(XMLElement("failure", "http://jabber.org/protocol/compress").serialize());
			}
	};
}
