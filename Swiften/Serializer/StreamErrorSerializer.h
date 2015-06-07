/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Elements/StreamError.h>

namespace Swift {
	class SWIFTEN_API StreamErrorSerializer : public GenericElementSerializer<StreamError> {
		public:
			StreamErrorSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement> error) const;
	};
}
