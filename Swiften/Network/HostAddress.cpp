#include "Swiften/Network/HostAddress.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <sstream>
#include <iomanip>

#include "Swiften/Base/foreach.h"
#include "Swiften/Base/String.h"

namespace Swift {

HostAddress::HostAddress() {
  for (int i = 0; i < 4; ++i) {
    address_.push_back(0);
  }
}

HostAddress::HostAddress(const String& address) {
	std::vector<String> components = address.split('.');
	assert(components.size() == 4);
	foreach(const String& component, components) {
		address_.push_back(boost::lexical_cast<int>(component.getUTF8String()));
	}
}

HostAddress::HostAddress(const unsigned char* address, int length) {
	assert(length == 4 || length == 16);
	address_.reserve(length);
	for (int i = 0; i < length; ++i) {
		address_.push_back(address[i]);
	}
}

std::string HostAddress::toString() const {
	if (address_.size() == 4) {
		std::ostringstream result;
		for (size_t i = 0; i < address_.size() - 1; ++i) {
			result << boost::numeric_cast<unsigned int>(address_[i]) << ".";
		}
		result << boost::numeric_cast<unsigned int>(address_[address_.size()	- 1]);
		return result.str();
	}
	else if (address_.size() == 16) {
		std::ostringstream result;
		result << std::hex;
		result.fill('0');
		for (size_t i = 0; i < (address_.size() / 2) - 1; ++i) {
			result << std::setw(2) << boost::numeric_cast<unsigned int>(address_[2*i]) << std::setw(2) << boost::numeric_cast<unsigned int>(address_[(2*i)+1]) << ":";
		}
		result << std::setw(2) << boost::numeric_cast<unsigned int>(address_[address_.size() - 2]) << std::setw(2) << boost::numeric_cast<unsigned int>(address_[address_.size() - 1]);
		return result.str();
	}
	else {
		assert(false);
    return "";
	}
}

}
