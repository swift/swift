/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Serializer/ElementSerializer.h>

namespace Swift {
	class CompressRequestSerializer : public ElementSerializer {
		public:
			CompressRequestSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement> element)  const;
			virtual bool canSerialize(boost::shared_ptr<ToplevelElement> element) const;
	};
}
