/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_CompressFailureSerializer_H
#define SWIFTEN_CompressFailureSerializer_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/CompressFailure.h"
#include "Swiften/Serializer/GenericElementSerializer.h"
#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {
	class CompressFailureSerializer : public GenericElementSerializer<CompressFailure> {
		public:
			CompressFailureSerializer() : GenericElementSerializer<CompressFailure>() {
			}

			virtual std::string serialize(boost::shared_ptr<Element>) const {
				return XMLElement("failure", "http://jabber.org/protocol/compress").serialize();
			}
	};
}

#endif
