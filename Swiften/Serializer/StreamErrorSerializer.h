/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Elements/StreamError.h>

namespace Swift {
	class StreamErrorSerializer : public GenericElementSerializer<StreamError> {
		public:
			StreamErrorSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement> error) const;
	};
}
