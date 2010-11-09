/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class Certificate {
		public:
			typedef boost::shared_ptr<Certificate> ref;

			virtual ~Certificate();

			virtual String getSubjectName() const = 0;

			virtual std::vector<String> getCommonNames() const = 0;
			virtual std::vector<String> getSRVNames() const = 0;
			virtual std::vector<String> getDNSNames() const = 0;
			virtual std::vector<String> getXMPPAddresses() const = 0;

			virtual ByteArray toDER() const = 0;

		protected:
			static const char* ID_ON_XMPPADDR_OID;
			static const char* ID_ON_DNSSRV_OID;

	};
}
