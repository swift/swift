/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
	template<typename T>
	class ResponseSink {
		public:
			ResponseSink() : responseReceived(false) {
			}

			bool hasResponse() const {
				return responseReceived;
			}

			boost::shared_ptr<T> getResponsePayload() const {
				return payload;
			}

			ErrorPayload::ref getResponseError() const {
				return error;
			}

			void operator()(boost::shared_ptr<T> payload, ErrorPayload::ref error) {
				this->payload = payload;
				this->error = error;
				this->responseReceived = true;
			}

			void operator()(ErrorPayload::ref error) {
				this->error = error;
				this->responseReceived = true;
			}

		private:
			bool responseReceived;
			boost::shared_ptr<T> payload;
			ErrorPayload::ref error;
	};
}
