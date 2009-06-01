#ifndef SWIFTEN_HOSTADDRESS
#define SWIFTEN_HOSTADDRESS

#include <string>
#include <vector>

namespace Swift {
	class HostAddress {
		public:
      HostAddress();
			HostAddress(const unsigned char* address, int length);

			const std::vector<unsigned char>& getRawAddress() const {
				return address_;
			}

			std::string toString() const;

		private:
			std::vector<unsigned char> address_;
	};
}

#endif
