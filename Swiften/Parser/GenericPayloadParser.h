/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/PayloadParser.h>

namespace Swift {
	
	class FormParser;

	/**
	 * A generic payload parser for payloads of the given type.
	 *
	 * This class provides getPayloadInternal() for retrieving the actual
	 * payload.
	 */
	template<typename PAYLOAD_TYPE>
	class SWIFTEN_API GenericPayloadParser : public PayloadParser {
		public:
			GenericPayloadParser() : PayloadParser() {
				payload_ = boost::make_shared<PAYLOAD_TYPE>();
			}

			virtual boost::shared_ptr<Payload> getPayload() const {
				return payload_;
			}

			virtual boost::shared_ptr<PAYLOAD_TYPE> getPayloadInternal() const {
				return payload_;
			}

		private:
			boost::shared_ptr<PAYLOAD_TYPE> payload_;
	};
}
