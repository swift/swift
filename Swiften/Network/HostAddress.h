#pragma once

#include <string>
#include <vector>

namespace Swift {
	class String;

	class HostAddress {
		public:
			HostAddress();
			HostAddress(const String&);
			HostAddress(const unsigned char* address, int length);

			const std::vector<unsigned char>& getRawAddress() const {
				return address_;
			}

			std::string toString() const;

			bool operator==(const HostAddress& o) const {
				return address_ == o.address_;
			}

		private:
			std::vector<unsigned char> address_;
	};
}
