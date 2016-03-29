/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
	class Payload;
	class PayloadSerializer;
	class PayloadParser;

	class SWIFTEN_API PayloadPersister {
		public: 
			PayloadPersister();
			virtual ~PayloadPersister();

			void savePayload(boost::shared_ptr<Payload>, const boost::filesystem::path&);
			boost::shared_ptr<Payload> loadPayload(const boost::filesystem::path&);

		protected:

			virtual const PayloadSerializer* getSerializer() const = 0;
			virtual PayloadParser* createParser() const = 0;
	};
}
