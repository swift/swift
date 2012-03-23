/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/Error.h>

namespace Swift {
	class TLSError : public Error {
		public:
			typedef boost::shared_ptr<TLSError> ref;

			enum Type {
				UnknownError,
				CertificateCardRemoved
			};

			TLSError(Type type = UnknownError) : type(type) {}

			Type getType() const { 
				return type; 
			}

		private:
			Type type;
	};
}
