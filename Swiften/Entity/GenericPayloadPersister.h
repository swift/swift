/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Entity/PayloadPersister.h>
#include <Swiften/Parser/GenericPayloadParserFactory.h>

namespace Swift {
	template<typename PAYLOAD, typename PARSER, typename SERIALIZER>
	class GenericPayloadPersister : public PayloadPersister {
		public:
			GenericPayloadPersister() {
			}

		public:
			boost::shared_ptr<PAYLOAD> loadPayloadGeneric(const boost::filesystem::path& path) {
				return boost::dynamic_pointer_cast<PAYLOAD>(loadPayload(path));
			}

		protected:
			virtual const PayloadSerializer* getSerializer() const {
				return &serializer;
			}

			virtual PayloadParser* createParser() const {
				return new PARSER();
			}

		private:
			SERIALIZER serializer;
	};
}
