/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Component/ComponentHandshakeGenerator.h>

#include <Swiften/Base/String.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/StringCodecs/Hexify.h>

namespace Swift {

std::string ComponentHandshakeGenerator::getHandshake(const std::string& streamID, const std::string& secret, CryptoProvider* crypto) {
    std::string concatenatedString = streamID + secret;
    String::replaceAll(concatenatedString, '&', "&amp;");
    String::replaceAll(concatenatedString, '<', "&lt;");
    String::replaceAll(concatenatedString, '>', "&gt;");
    String::replaceAll(concatenatedString, '\'', "&apos;");
    String::replaceAll(concatenatedString, '"', "&quot;");
    return Hexify::hexify(crypto->getSHA1Hash(createByteArray(concatenatedString)));
}

}
