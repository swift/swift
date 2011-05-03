/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Component/ComponentHandshakeGenerator.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/Base/String.h>

namespace Swift {

std::string ComponentHandshakeGenerator::getHandshake(const std::string& streamID, const std::string& secret) {
	std::string concatenatedString = streamID + secret;
	String::replaceAll(concatenatedString, '&', "&amp;");
	String::replaceAll(concatenatedString, '<', "&lt;");
	String::replaceAll(concatenatedString, '>', "&gt;");
	String::replaceAll(concatenatedString, '\'', "&apos;");
	String::replaceAll(concatenatedString, '"', "&quot;");
	return Hexify::hexify(SHA1::getHash(createByteArray(concatenatedString)));
}

}
