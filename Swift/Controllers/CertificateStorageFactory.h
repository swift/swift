/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class CertificateStorage;
	class JID;

	class CertificateStorageFactory {
		public:
			virtual ~CertificateStorageFactory();

			virtual CertificateStorage* createCertificateStorage(const JID& profile) const = 0;
	};
}
