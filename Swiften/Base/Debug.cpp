/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/Debug.h>

#include <iostream>
#include <memory>

#include <Swiften/Client/ClientError.h>
#include <Swiften/Client/ClientSession.h>
#include <Swiften/Serializer/PayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Serializer/XMPPSerializer.h>

std::ostream& operator<<(std::ostream& os, const Swift::ClientError& error) {
    os << "ClientError(";
    switch(error.getType()) {
        case Swift::ClientError::UnknownError:
            os << "UnknownError";
            break;
        case Swift::ClientError::DomainNameResolveError:
            os << "DomainNameResolveError";
            break;
        case Swift::ClientError::ConnectionError:
            os << "ConnectionError";
            break;
        case Swift::ClientError::ConnectionReadError:
            os << "ConnectionReadError";
            break;
        case Swift::ClientError::ConnectionWriteError:
            os << "ConnectionWriteError";
            break;
        case Swift::ClientError::XMLError:
            os << "XMLError";
            break;
        case Swift::ClientError::AuthenticationFailedError:
            os << "AuthenticationFailedError";
            break;
        case Swift::ClientError::CompressionFailedError:
            os << "CompressionFailedError";
            break;
        case Swift::ClientError::ServerVerificationFailedError:
            os << "ServerVerificationFailedError";
            break;
        case Swift::ClientError::NoSupportedAuthMechanismsError:
            os << "NoSupportedAuthMechanismsError";
            break;
        case Swift::ClientError::UnexpectedElementError:
            os << "UnexpectedElementError";
            break;
        case Swift::ClientError::ResourceBindError:
            os << "ResourceBindError";
            break;
        case Swift::ClientError::SessionStartError:
            os << "SessionStartError";
            break;
        case Swift::ClientError::StreamError:
            os << "StreamError";
            break;
        case Swift::ClientError::TLSError:
            os << "TLSError";
            break;
        case Swift::ClientError::ClientCertificateLoadError:
            os << "ClientCertificateLoadError";
            break;
        case Swift::ClientError::ClientCertificateError:
            os << "ClientCertificateError";
            break;
        case Swift::ClientError::CertificateCardRemoved:
            os << "CertificateCardRemoved";
            break;
        case Swift::ClientError::UnknownCertificateError:
            os << "UnknownCertificateError";
            break;
        case Swift::ClientError::CertificateExpiredError:
            os << "CertificateExpiredError";
            break;
        case Swift::ClientError::CertificateNotYetValidError:
            os << "CertificateNotYetValidError";
            break;
        case Swift::ClientError::CertificateSelfSignedError:
            os << "CertificateSelfSignedError";
            break;
        case Swift::ClientError::CertificateRejectedError:
            os << "CertificateRejectedError";
            break;
        case Swift::ClientError::CertificateUntrustedError:
            os << "CertificateUntrustedError";
            break;
        case Swift::ClientError::InvalidCertificatePurposeError:
            os << "InvalidCertificatePurposeError";
            break;
        case Swift::ClientError::CertificatePathLengthExceededError:
            os << "CertificatePathLengthExceededError";
            break;
        case Swift::ClientError::InvalidCertificateSignatureError:
            os << "InvalidCertificateSignatureError";
            break;
        case Swift::ClientError::InvalidCAError:
            os << "InvalidCAError";
            break;
        case Swift::ClientError::InvalidServerIdentityError:
            os << "InvalidServerIdentityError";
            break;
        case Swift::ClientError::RevokedError:
            os << "RevokedError";
            break;
        case Swift::ClientError::RevocationCheckFailedError:
            os << "RevocationCheckFailedError";
            break;
    }
    os << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, Swift::Element* ele) {
    using namespace Swift;

    std::shared_ptr<Element> element = std::shared_ptr<Element>(ele);

    std::shared_ptr<Payload> payload = std::dynamic_pointer_cast<Payload>(element);
    if (payload) {
         FullPayloadSerializerCollection payloadSerializerCollection;
         PayloadSerializer *serializer = payloadSerializerCollection.getPayloadSerializer(payload);
         os << "Payload(" << serializer->serialize(payload) << ")";
         return os;
    }
    std::shared_ptr<ToplevelElement> topLevelElement = std::dynamic_pointer_cast<ToplevelElement>(element);
    if (topLevelElement) {
        FullPayloadSerializerCollection payloadSerializerCollection;
        XMPPSerializer xmppSerializer(&payloadSerializerCollection, ClientStreamType, false);
        SafeByteArray serialized = xmppSerializer.serializeElement(topLevelElement);
        os << "TopLevelElement(" << safeByteArrayToString(serialized) << ")";
        return os;
    }
    os << "Element(Unknown)";
    return os;
}

std::ostream& operator<<(std::ostream& os, Swift::ClientSession::State state) {
    using CS = Swift::ClientSession;
    switch (state) {
        case CS::State::Initial:
            os << "ClientSession::State::Initial";
            break;
        case CS::State::WaitingForStreamStart:
            os << "ClientSession::State::WaitingForStreamStart";
            break;
        case CS::State::Negotiating:
            os << "ClientSession::State::Negotiating";
            break;
        case CS::State::Compressing:
            os << "ClientSession::State::Compressing";
            break;
        case CS::State::WaitingForEncrypt:
            os << "ClientSession::State::WaitingForEncrypt";
            break;
        case CS::State::Encrypting:
            os << "ClientSession::State::Encrypting";
            break;
        case CS::State::WaitingForCredentials:
            os << "ClientSession::State::WaitingForCredentials";
            break;
        case CS::State::Authenticating:
            os << "ClientSession::State::Authenticating";
            break;
        case CS::State::EnablingSessionManagement:
            os << "ClientSession::State::EnablingSessionManagement";
            break;
        case CS::State::BindingResource:
            os << "ClientSession::State::BindingResource";
            break;
        case CS::State::StartingSession:
            os << "ClientSession::State::StartingSession";
            break;
        case CS::State::Initialized:
            os << "ClientSession::State::Initialized";
            break;
        case CS::State::Finishing:
            os << "ClientSession::State::Finishing";
            break;
        case CS::State::Finished:
            os << "ClientSession::State::Finished";
            break;
    }
    return os;
}
