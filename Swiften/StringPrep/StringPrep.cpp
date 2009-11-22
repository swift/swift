#include "Swiften/StringPrep/StringPrep.h"

#include <stringprep.h>
#include <vector>

namespace Swift {

static const int MAX_STRINGPREP_SIZE = 1024;

const Stringprep_profile* getLibIDNProfile(StringPrep::Profile profile) {
	switch(profile) {
		case StringPrep::NamePrep: return stringprep_nameprep; break;
		case StringPrep::XMPPNodePrep: return stringprep_xmpp_nodeprep; break;
		case StringPrep::XMPPResourcePrep: return stringprep_xmpp_resourceprep; break;
		case StringPrep::SASLPrep: return stringprep_saslprep; break;
	}
}

String StringPrep::getPrepared(const String& s, Profile profile) {
	
	std::vector<char> input(s.getUTF8String().begin(), s.getUTF8String().end());
	input.resize(MAX_STRINGPREP_SIZE);
	if (stringprep(&input[0], MAX_STRINGPREP_SIZE, static_cast<Stringprep_profile_flags>(0), getLibIDNProfile(profile)) == 0) {
		return String(&input[0]);
	}
	else {
		return "";
	}
}

}
