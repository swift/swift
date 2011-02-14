/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_COMPRESSREQUESTSERIALIZER_H
#define SWIFTEN_COMPRESSREQUESTSERIALIZER_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Serializer/ElementSerializer.h"

namespace Swift {
	class CompressRequestSerializer : public ElementSerializer {
		public:
			CompressRequestSerializer();

			virtual std::string serialize(boost::shared_ptr<Element> element)  const;
			virtual bool canSerialize(boost::shared_ptr<Element> element) const;
	};
}

#endif
