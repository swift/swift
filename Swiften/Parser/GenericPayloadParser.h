/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef GENERICPAYLOADPARSER_H
#define GENERICPAYLOADPARSER_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Parser/PayloadParser.h"

namespace Swift {
	class String;
	class FormParser;

	template<typename PAYLOAD_TYPE>
	class GenericPayloadParser : public PayloadParser {
		public:
			GenericPayloadParser() : PayloadParser() {
				payload_ = boost::shared_ptr<PAYLOAD_TYPE>(new PAYLOAD_TYPE());
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

#endif
