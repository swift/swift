/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Serializer/ElementSerializer.h>

namespace Swift {
	template<typename T>
	class GenericElementSerializer : public ElementSerializer {
		public:
			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement> element) const = 0;

			virtual bool canSerialize(boost::shared_ptr<ToplevelElement> element) const {
				return !!boost::dynamic_pointer_cast<T>(element);
			}
	};
}
