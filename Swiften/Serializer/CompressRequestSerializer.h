/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Serializer/ElementSerializer.h>

namespace Swift {
	class CompressRequestSerializer : public ElementSerializer {
		public:
			CompressRequestSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<Element> element)  const;
			virtual bool canSerialize(boost::shared_ptr<Element> element) const;
	};
}
