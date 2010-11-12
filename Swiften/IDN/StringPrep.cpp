/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/IDN/StringPrep.h>

extern "C"
{
	#include <stringprep.h>
};

#include <vector>
#include <cassert>
#include <Swiften/Base/SafeAllocator.h>

using namespace Swift;

	namespace {
	static const int MAX_STRINGPREP_SIZE = 1024;

	const Stringprep_profile* getLibIDNProfile(StringPrep::Profile profile) {
		switch(profile) {
			case StringPrep::NamePrep: return stringprep_nameprep; break;
			case StringPrep::XMPPNodePrep: return stringprep_xmpp_nodeprep; break;
			case StringPrep::XMPPResourcePrep: return stringprep_xmpp_resourceprep; break;
			case StringPrep::SASLPrep: return stringprep_saslprep; break;
		}
		assert(false);
		return 0;
	}

	template<typename StringType, typename ContainerType>
	ContainerType getStringPrepared(const StringType& s, StringPrep::Profile profile) {
		ContainerType input(s.begin(), s.end());
		input.resize(MAX_STRINGPREP_SIZE);
		if (stringprep(&input[0], MAX_STRINGPREP_SIZE, static_cast<Stringprep_profile_flags>(0), getLibIDNProfile(profile)) == 0) {
			return input;
		}
		else {
			return ContainerType();
		}
	}
}

namespace Swift {

std::string StringPrep::getPrepared(const std::string& s, Profile profile) {
	std::vector<char> preparedData = getStringPrepared< std::string, std::vector<char> >(s, profile);
	if (preparedData.empty()) {
		throw std::exception();
	}
	return std::string(vecptr(preparedData));
}

SafeByteArray StringPrep::getPrepared(const SafeByteArray& s, Profile profile) {
	std::vector<char, SafeAllocator<char> > preparedData = getStringPrepared<SafeByteArray, std::vector<char, SafeAllocator<char> > >(s, profile);
	if (preparedData.empty()) {
		throw std::exception();
	}
	return createSafeByteArray(reinterpret_cast<const char*>(vecptr(preparedData)));
}

}
