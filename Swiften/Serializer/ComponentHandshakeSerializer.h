/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/ComponentHandshake.h>
#include <Swiften/Serializer/GenericElementSerializer.h>

namespace Swift {
	class ComponentHandshakeSerializer : public GenericElementSerializer<ComponentHandshake> {
		public:
			ComponentHandshakeSerializer();

			virtual SafeByteArray serialize(boost::shared_ptr<Element> element)  const;
	};
}
