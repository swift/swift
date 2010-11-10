/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/TLS/Certificate.h"

namespace Swift {
	class SimpleCertificate : public Certificate {
		public:
			typedef boost::shared_ptr<SimpleCertificate> ref;

			void setSubjectName(const String& name) {
				subjectName = name;
			}

			String getSubjectName() const {
				return subjectName;
			}

			std::vector<String> getCommonNames() const {
				return commonNames;
			}

			void addCommonName(const String& name) {
				commonNames.push_back(name);
			}

			void addSRVName(const String& name) {
				srvNames.push_back(name);
			}

			void addDNSName(const String& name) {
				dnsNames.push_back(name);
			}

			void addXMPPAddress(const String& addr) {
				xmppAddresses.push_back(addr);
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

			ByteArray toDER() const {
				return ByteArray();
			}

		private:
			void parse();

		private:
			String subjectName;
			std::vector<String> commonNames;
			std::vector<String> dnsNames;
			std::vector<String> xmppAddresses;
			std::vector<String> srvNames;
	};
}
