/*
 * Copyright (c) 2010-2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class CertificateWithKey {
		public:
			typedef boost::shared_ptr<CertificateWithKey> ref;
			CertificateWithKey() {}

			virtual ~CertificateWithKey() {}

			virtual bool isNull() const = 0;

	};
}
