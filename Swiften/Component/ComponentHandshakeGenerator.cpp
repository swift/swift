/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Component/ComponentHandshakeGenerator.h"
#include "Swiften/StringCodecs/Hexify.h"
#include "Swiften/StringCodecs/SHA1.h"

namespace Swift {

String ComponentHandshakeGenerator::getHandshake(const String& streamID, const String& secret) {
	String concatenatedString = streamID + secret;
	concatenatedString.replaceAll('&', "&amp;");
	concatenatedString.replaceAll('<', "&lt;");
	concatenatedString.replaceAll('>', "&gt;");
	concatenatedString.replaceAll('\'', "&apos;");
	concatenatedString.replaceAll('"', "&quot;");
	return Hexify::hexify(SHA1::getHash(ByteArray(concatenatedString)));
}

}
