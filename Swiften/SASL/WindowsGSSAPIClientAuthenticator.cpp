/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/SASL/WindowsGSSAPIClientAuthenticator.h>

#include <Swiften/Base/Log.h>
#include <Swiften/SASL/WindowsAuthentication.h>
#include <Swiften/SASL/WindowsServicePrincipalName.h>

#define SECURITY_LAYER_NONE 1

namespace Swift {

WindowsGSSAPIClientAuthenticator::WindowsGSSAPIClientAuthenticator(const std::string& hostname, const std::string& domainname, int port) : ClientAuthenticator("GSSAPI"), step_(BuildingSecurityContext), error_(false), haveCredentialsHandle_(false), haveContextHandle_(false), haveCompleteContext_(false) {
    WindowsServicePrincipalName servicePrincipalName(domainname);
    servicePrincipalName.setInstanceName(hostname);
    if ((port != -1) && (port != 5222)) {
        servicePrincipalName.setInstancePort(port);
    }
    servicePrincipalNameString_ = servicePrincipalName.toString();

    errorCode_ = acquireCredentialsHandle(&credentialsHandle_);
    if (isError()) {
        return;
    }
    else {
        haveCredentialsHandle_ = true;
    }

    buildSecurityContext(boost::optional<ByteArray>());
}

WindowsGSSAPIClientAuthenticator::~WindowsGSSAPIClientAuthenticator() {
    if (haveContextHandle_) {
        deleteSecurityContext(&contextHandle_);
    }

    if (haveCredentialsHandle_) {
        freeCredentialsHandle(&credentialsHandle_);
    }
}

boost::optional<SafeByteArray> WindowsGSSAPIClientAuthenticator::getResponse() const {
    SWIFT_LOG(debug) << "response_.size(): " << response_.size() << std::endl;
    return response_;
}

bool WindowsGSSAPIClientAuthenticator::setChallenge(const boost::optional<ByteArray>& challengeData) {
    /* Following http://tools.ietf.org/html/rfc4752, https://msdn.microsoft.com/en-us/library/windows/desktop/aa380496%28v=vs.85%29.aspx */

    if (step_ == BuildingSecurityContext) {
        buildSecurityContext(challengeData);
    }
    else if (step_ == SecurityLayerNegotiation) {
        if (!challengeData) {
            SWIFT_LOG(debug) << "Empty message received from the server" << std::endl;
            error_ = true;
            return false;
        }

        SafeByteArray challenge;
        errorCode_ = decryptMessage(&contextHandle_, challengeData.get(), challenge);
        if (isError()) {
            return false;
        }

        if (challenge.size() != 4) {
            SWIFT_LOG(debug) << "Token received from the server of incorrect length: " << challenge.size() << std::endl;
            error_ = true;
            return false;
        }

        unsigned char* challengePointer = vecptr(challenge);

        unsigned char serverSecurityLayer = challengePointer[0];
        if (serverSecurityLayer == 0) {
            SWIFT_LOG(debug) << "Server supports unknown security layer, assuming no security layer" << std::endl;
            serverSecurityLayer = SECURITY_LAYER_NONE;
        }
        else if (serverSecurityLayer == SECURITY_LAYER_NONE) {
            SWIFT_LOG(debug) << "Server supports no security layer" << std::endl;
        }
        else {
            SWIFT_LOG(debug) << "Server supports security layer" << std::endl;
        }

        unsigned int serverMaximumBuffer = (challengePointer[1] << 16) |
                        (challengePointer[2] << 8) |
                        (challengePointer[3] << 0);

        if ((serverSecurityLayer == SECURITY_LAYER_NONE) && (serverMaximumBuffer != 0)) {
            SWIFT_LOG(debug) << "Server supports no security layer but has maximum buffer size" << serverMaximumBuffer << std::endl;
            error_ = true;
            return false;
        }

        SafeByteArray message(4);

        /* Commenting this out as streamSizes was not obtained before
        if (message.size() > streamSizes_.cbMaximumMessage) {
            error_ = true;
            return false;
        } */

        unsigned char* messagePointer = vecptr(message);
        messagePointer[0] = SECURITY_LAYER_NONE;

        /* The next 3 bytes indicate the client's maximum size buffer which is set to 0 as we do not support a security layer */
        messagePointer[1] = 0;
        messagePointer[2] = 0;
        messagePointer[3] = 0;

        /* The authorization identity is omitted as it is the same as the authentication identity */

        errorCode_ = encryptMessage(&contextHandle_, sizes_, message, response_);
        if (isError()) {
            return false;
        }

        step_ = ServerAuthenticated;
    }

    if (isError()) {
        return false;
    }

    return true;
}

bool WindowsGSSAPIClientAuthenticator::isError() {
    if (error_) {
        return true;
    }

    if (!errorCode_) {
        return false;
    }

    return true;
}

void WindowsGSSAPIClientAuthenticator::buildSecurityContext(const boost::optional<ByteArray>& inputToken) {
    ULONG contextSupported;

    /* An XMPP server may not support Kerberos encryption or SASL security layer so not requesting integrity or confidentiality */
    errorCode_ = initializeSecurityContext(inputToken, servicePrincipalNameString_, &credentialsHandle_, haveContextHandle_, &contextHandle_, ISC_REQ_MUTUAL_AUTH, &contextSupported, &haveCompleteContext_, response_);
    if (isError()) {
        return;
    }

    haveContextHandle_ = true;

    if (!haveCompleteContext_) {
        return;
    }

    if (contextSupported & ISC_REQ_MUTUAL_AUTH == 0) {
        SWIFT_LOG(debug) << "Mutual authentication not supported" << std::endl;
        error_ = true;
        return;
    }

    errorCode_ = queryContextAttributes(&contextHandle_, SECPKG_ATTR_SIZES, &sizes_);
    if (isError()) {
        return;
    }

    /* Commenting this out as it gives the error code 0x80090302: The function requested is not supported
    errorCode_ = queryContextAttributes(&contextHandle_, SECPKG_ATTR_STREAM_SIZES, &streamSizes_);
    if (isError()) {
        return;
    }*/

    SecPkgContext_Names names;
    errorCode_ = queryContextAttributes(&contextHandle_, SECPKG_ATTR_NAMES, &names);
    if (isError()) {
        return;
    }

    userName_ = names.sUserName;
    SWIFT_LOG(debug) << "User name: " << userName_ << std::endl;

    std::size_t position = userName_.find("\\");
    clientName_ = userName_.substr(position + 1);
    SWIFT_LOG(debug) << "Client name: " << clientName_ << std::endl;

    serverName_ = userName_.substr(0, position);
    SWIFT_LOG(debug) << "Server name: " << serverName_ << std::endl;

    freeContextBuffer(names.sUserName);
    step_ = SecurityLayerNegotiation;
}

}
