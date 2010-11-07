/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <openssl/ssl.h>

#include "Swiften/Base/String.h"
#include "Swiften/TLS/Certificate.h"

namespace Swift {
	class OpenSSLCertificate : public Certificate {
		public:
			OpenSSLCertificate(boost::shared_ptr<X509>);

			String getCommonName() const {
				return commonName;
			}

			std::vector<String> getSRVNames() const {
				return srvNames;
			}

			std::vector<String> getDNSNames() const {
				return dnsNames;
			}

			std::vector<String> getXMPPAddresses() const {
				return xmppAddresses;
			}

		private:
			void addSRVName(const String& name) {
				srvNames.push_back(name);
			}

			void addDNSName(const String& name) {
				dnsNames.push_back(name);
			}

			void addXMPPAddress(const String& addr) {
				xmppAddresses.push_back(addr);
			}

			void setCommonName(const String& commonName) {
				this->commonName = commonName;
			}

		private:
			boost::shared_ptr<X509> cert;
			String commonName;
			std::vector<String> dnsNames;
			std::vector<String> xmppAddresses;
			std::vector<String> srvNames;
	};
}
