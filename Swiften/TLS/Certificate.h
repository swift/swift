/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <string>
#include <Swiften/Base/ByteArray.h>

namespace Swift {
	class Certificate {
		public:
			typedef boost::shared_ptr<Certificate> ref;

			virtual ~Certificate();

			/**
			 * Returns the textual representation of the full Subject
			 * name.
			 */
			virtual std::string getSubjectName() const = 0;

			virtual std::vector<std::string> getCommonNames() const = 0;
			virtual std::vector<std::string> getSRVNames() const = 0;
			virtual std::vector<std::string> getDNSNames() const = 0;
			virtual std::vector<std::string> getXMPPAddresses() const = 0;

			virtual ByteArray toDER() const = 0;

			virtual std::string getSHA1Fingerprint() const;

		protected:
			static const char* ID_ON_XMPPADDR_OID;
			static const char* ID_ON_DNSSRV_OID;

	};
}
