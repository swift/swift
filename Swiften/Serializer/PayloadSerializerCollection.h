/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class PayloadSerializer;
	

	class PayloadSerializerCollection {
		public:
			PayloadSerializerCollection();

			void addSerializer(PayloadSerializer* factory);
			void removeSerializer(PayloadSerializer* factory);
			PayloadSerializer* getPayloadSerializer(boost::shared_ptr<Payload>) const;

		private:
			std::vector<PayloadSerializer*> serializers_;
	};
}
