/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_PAYLOADSERIALIZER_H
#define SWIFTEN_PAYLOADSERIALIZER_H

#include <boost/shared_ptr.hpp>

#include <string>
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class PayloadSerializer {
		public:
			virtual ~PayloadSerializer();

			virtual bool canSerialize(boost::shared_ptr<Payload>) const = 0;
			virtual std::string serialize(boost::shared_ptr<Payload>) const = 0;
	};
}

#endif
