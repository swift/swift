/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/ComponentHandshake.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
	class ComponentHandshakeSerializer : public GenericElementSerializer<ComponentHandshake> {
		public:
			ComponentHandshakeSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement> element)  const;
	};
}
