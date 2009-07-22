#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Network/HostAddress.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

namespace Swift {
	class LinkLocalService {
		public:
			LinkLocalService();

			const String& getName() const {
				return name;
			}

			void setName(const String& n) {
				name = n;
			}

			const String& getHostname() const {
				return hostname;
			}

			void setHostname(const String& h) {
				hostname = h;
			}

			const HostAddress& getAddress() const {
				return address;
			}

			void setAddress(const HostAddress& a) {
				address = a;
			}

			const LinkLocalServiceInfo& getInfo() const {
				return info;
			}

			void setInfo(const LinkLocalServiceInfo& i) {
				info = i;
			}

		private:
			String name;
			String hostname;
			LinkLocalServiceInfo info;
			HostAddress address;
	};
}
