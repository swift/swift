/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Serializer/ElementSerializer.h>

namespace Swift {
	template<typename T>
	class GenericElementSerializer : public ElementSerializer {
		public:
			virtual SafeByteArray serialize(boost::shared_ptr<Element> element) const = 0;

			virtual bool canSerialize(boost::shared_ptr<Element> element) const {
				return boost::dynamic_pointer_cast<T>(element);
			}
	};
}
