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
			CertificateWithKey() {}

			virtual ~CertificateWithKey() {}

			virtual bool isNull() const = 0;

			virtual bool isPrivateKeyExportable() const = 0;

			virtual const std::string& getCertStoreName() const = 0;

			virtual const std::string& getCertName() const = 0;

			virtual const ByteArray& getData() const = 0;

			virtual void setData(const ByteArray& data) = 0;

			virtual const SafeByteArray& getPassword() const = 0;
	};
}
