/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class Payload;

	class PayloadSerializer {
		public:
			virtual ~PayloadSerializer();

			virtual bool canSerialize(boost::shared_ptr<Payload>) const = 0;
			virtual std::string serialize(boost::shared_ptr<Payload>) const = 0;
	};
}
