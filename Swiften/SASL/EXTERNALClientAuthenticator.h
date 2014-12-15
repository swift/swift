/*
 * Copyright (c) 2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/SASL/ClientAuthenticator.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class EXTERNALClientAuthenticator : public ClientAuthenticator {
		public:
			EXTERNALClientAuthenticator();

			virtual boost::optional<SafeByteArray> getResponse() const;
			virtual bool setChallenge(const boost::optional<ByteArray>&);

		private:
			bool finished;
	};
}
