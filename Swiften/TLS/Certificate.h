/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"

namespace Swift {
	class Certificate {
		public:
			typedef boost::shared_ptr<Certificate> ref;

			const String& getCommonName() const {
				return commonName;
			}

			void setCommonName(const String& commonName) {
				this->commonName = commonName;
			}

			const std::vector<String>& getSRVNames() const {
				return srvNames;
			}

			void addSRVName(const String& name) {
				srvNames.push_back(name);
			}

			const std::vector<String>& getDNSNames() const {
				return dnsNames;
			}

			void addDNSName(const String& name) {
				dnsNames.push_back(name);
			}

			const std::vector<String>& getXMPPAddresses() const {
				return xmppAddresses;
			}

			void addXMPPAddress(const String& addr) {
				xmppAddresses.push_back(addr);
			}

		private:
			String commonName;
			std::vector<String> dnsNames;
			std::vector<String> xmppAddresses;
			std::vector<String> srvNames;
	};
}
