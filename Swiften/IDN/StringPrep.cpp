/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/IDN/StringPrep.h"

#include <stringprep.h>
#include <vector>
#include <cassert>

namespace Swift {

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

std::string StringPrep::getPrepared(const std::string& s, Profile profile) {
	
	std::vector<char> input(s.begin(), s.end());
	input.resize(MAX_STRINGPREP_SIZE);
	if (stringprep(&input[0], MAX_STRINGPREP_SIZE, static_cast<Stringprep_profile_flags>(0), getLibIDNProfile(profile)) == 0) {
		return std::string(&input[0]);
	}
	else {
		return "";
	}
}

}
