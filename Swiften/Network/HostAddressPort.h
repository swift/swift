#ifndef SWIFTEN_HostAddressPort_H
#define SWIFTEN_HostAddressPort_H

#include "Swiften/Network/HostAddress.h"

namespace Swift {
	class HostAddressPort {
		public:
			HostAddressPort(const HostAddress& address, int port) : address_(address), port_(port) {
			}

			const HostAddress& getAddress() const {
				return address_;
			}

			int getPort() const {
				return port_;
			}

		private:
			HostAddress address_;
			int port_;
	};
}

#endif
