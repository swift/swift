/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

namespace Swift {
	class Payload;
	class PayloadSerializer;
	class PayloadParser;

	class PayloadPersister {
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
