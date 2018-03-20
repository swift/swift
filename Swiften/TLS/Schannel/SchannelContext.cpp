/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/Schannel/SchannelContext.h>

#include <boost/bind.hpp>

#include <WinHTTP.h> /* For SECURITY_FLAG_IGNORE_CERT_CN_INVALID */

#include <Swiften/Base/Log.h>
#include <Swiften/TLS/CAPICertificate.h>
#include <Swiften/TLS/Schannel/SchannelCertificate.h>

namespace Swift {

//------------------------------------------------------------------------

SchannelContext::SchannelContext(bool tls1_0Workaround) : state_(Start), secContext_(0), myCertStore_(NULL), certStoreName_("MY"), certName_(), smartCardReader_(), checkCertificateRevocation_(true), tls1_0Workaround_(tls1_0Workaround), disconnectOnCardRemoval_(true) {
    contextFlags_ = ISC_REQ_ALLOCATE_MEMORY |
                ISC_REQ_CONFIDENTIALITY |
                ISC_REQ_EXTENDED_ERROR |
                ISC_REQ_INTEGRITY |
                ISC_REQ_REPLAY_DETECT |
                ISC_REQ_SEQUENCE_DETECT |
                ISC_REQ_USE_SUPPLIED_CREDS |
                ISC_REQ_STREAM;

    ZeroMemory(&streamSizes_, sizeof(streamSizes_));
}

//------------------------------------------------------------------------

SchannelContext::~SchannelContext() {
    SWIFT_LOG(debug) << "Destroying SchannelContext" << std::endl;
    if (myCertStore_) {
        if (CertCloseStore(myCertStore_, 0) == FALSE) {
            SWIFT_LOG(debug) << "Failed to close the certificate store" << std::endl;
        }
    }
}

//------------------------------------------------------------------------

void SchannelContext::determineStreamSizes() {
    if (QueryContextAttributes(contextHandle_, SECPKG_ATTR_STREAM_SIZES, &streamSizes_) != SEC_E_OK) {
        SWIFT_LOG(debug) << "QueryContextAttributes failed to determinate the stream size" << std::endl;
    }
}

//------------------------------------------------------------------------

void SchannelContext::connect() {
    ScopedCertContext pCertContext;

    state_ = Connecting;

    // If a user name is specified, then attempt to find a client
    // certificate. Otherwise, just create a NULL credential.
    if (!certName_.empty()) {
        if (myCertStore_ == NULL) {
            myCertStore_ = CertOpenSystemStore(0, certStoreName_.c_str());
            if (!myCertStore_) {
                indicateError(std::make_shared<TLSError>(TLSError::UnknownError));
                return;
            }
        }

        pCertContext = findCertificateInStore( myCertStore_, certName_ );
        if (pCertContext == NULL) {
            indicateError(std::make_shared<TLSError>(TLSError::UnknownError));
            return;
        }
    }

    // We use an empty list for client certificates
    PCCERT_CONTEXT clientCerts[1] = {0};

    SCHANNEL_CRED sc = {0};
    sc.dwVersion = SCHANNEL_CRED_VERSION;

    if (tls1_0Workaround_) {
        sc.grbitEnabledProtocols = SP_PROT_TLS1_CLIENT;
    }
    else {
        sc.grbitEnabledProtocols = /*SP_PROT_SSL3_CLIENT | */SP_PROT_TLS1_CLIENT | SP_PROT_TLS1_1_CLIENT | SP_PROT_TLS1_2_CLIENT;
    }

    sc.dwFlags = SCH_CRED_MANUAL_CRED_VALIDATION;

    if (pCertContext) {
        sc.cCreds = 1;
        sc.paCred = pCertContext.GetPointer();
        sc.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS;
    }
    else {
        sc.cCreds = 0;
        sc.paCred = clientCerts;
        sc.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS;
    }

    // Swiften performs the server name check for us
    sc.dwFlags |= SCH_CRED_NO_SERVERNAME_CHECK;

    SECURITY_STATUS status = AcquireCredentialsHandle(
        NULL,
        UNISP_NAME,
        SECPKG_CRED_OUTBOUND,
        NULL,
        &sc,
        NULL,
        NULL,
        credHandle_.Reset(),
        NULL);

    if (status != SEC_E_OK) {
        // We failed to obtain the credentials handle
        indicateError(std::make_shared<TLSError>(TLSError::UnknownError));
        return;
    }

    SecBuffer outBuffers[2];

    // We let Schannel allocate the output buffer for us
    outBuffers[0].pvBuffer   = NULL;
    outBuffers[0].cbBuffer   = 0;
    outBuffers[0].BufferType = SECBUFFER_TOKEN;

    // Contains alert data if an alert is generated
    outBuffers[1].pvBuffer   = NULL;
    outBuffers[1].cbBuffer   = 0;
    outBuffers[1].BufferType = SECBUFFER_ALERT;

    // Make sure the output buffers are freed
    ScopedSecBuffer scopedOutputData(&outBuffers[0]);
    ScopedSecBuffer scopedOutputAlertData(&outBuffers[1]);

    SecBufferDesc outBufferDesc = {0};
    outBufferDesc.cBuffers   = 2;
    outBufferDesc.pBuffers   = outBuffers;
    outBufferDesc.ulVersion  = SECBUFFER_VERSION;

    // Create the initial security context
    status = InitializeSecurityContext(
        credHandle_,
        NULL,
        NULL,
        contextFlags_,
        0,
        0,
        NULL,
        0,
        contextHandle_.Reset(),
        &outBufferDesc,
        &secContext_,
        NULL);

    if (status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) {
        // We failed to initialize the security context
        handleCertError(status);
        indicateError(std::make_shared<TLSError>(TLSError::UnknownError));
        return;
    }

    // Start the handshake
    sendDataOnNetwork(outBuffers[0].pvBuffer, outBuffers[0].cbBuffer);

    if (status == SEC_E_OK) {
        status = validateServerCertificate();
        if (status != SEC_E_OK) {
            handleCertError(status);
        }

        state_ = Connected;
        determineStreamSizes();

        onConnected();
    }
}

//------------------------------------------------------------------------

SECURITY_STATUS SchannelContext::validateServerCertificate() {
    SchannelCertificate::ref pServerCert = std::dynamic_pointer_cast<SchannelCertificate>( getPeerCertificate() );
    if (!pServerCert) {
        return SEC_E_WRONG_PRINCIPAL;
    }

    const LPSTR usage[] =
    {
        szOID_PKIX_KP_SERVER_AUTH,
        szOID_SERVER_GATED_CRYPTO,
        szOID_SGC_NETSCAPE
    };

    CERT_CHAIN_PARA chainParams = {0};
    chainParams.cbSize = sizeof(chainParams);
    chainParams.RequestedUsage.dwType = USAGE_MATCH_TYPE_OR;
    chainParams.RequestedUsage.Usage.cUsageIdentifier = ARRAYSIZE(usage);
    chainParams.RequestedUsage.Usage.rgpszUsageIdentifier = const_cast<LPSTR*>(usage);

    DWORD chainFlags = CERT_CHAIN_CACHE_END_CERT;
    if (checkCertificateRevocation_) {
        chainFlags |= CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT;
    }

    ScopedCertChainContext pChainContext;

    BOOL success = CertGetCertificateChain(
        NULL, // Use the chain engine for the current user (assumes a user is logged in)
        pServerCert->getCertContext(),
        NULL,
        pServerCert->getCertContext()->hCertStore,
        &chainParams,
        chainFlags,
        NULL,
        pChainContext.Reset());

    if (!success) {
        return GetLastError();
    }

    SSL_EXTRA_CERT_CHAIN_POLICY_PARA sslChainPolicy = {0};
    sslChainPolicy.cbSize = sizeof(sslChainPolicy);
    sslChainPolicy.dwAuthType = AUTHTYPE_SERVER;
    sslChainPolicy.fdwChecks = SECURITY_FLAG_IGNORE_CERT_CN_INVALID; // Swiften checks the server name for us. Is this the correct way to disable server name checking?
    sslChainPolicy.pwszServerName = NULL;

    CERT_CHAIN_POLICY_PARA certChainPolicy = {0};
    certChainPolicy.cbSize = sizeof(certChainPolicy);
    certChainPolicy.dwFlags = CERT_CHAIN_POLICY_IGNORE_INVALID_NAME_FLAG; // Swiften checks the server name for us. Is this the correct way to disable server name checking?
    certChainPolicy.pvExtraPolicyPara = &sslChainPolicy;

    CERT_CHAIN_POLICY_STATUS certChainPolicyStatus = {0};
    certChainPolicyStatus.cbSize = sizeof(certChainPolicyStatus);

    // Verify the chain
    if (!CertVerifyCertificateChainPolicy(
        CERT_CHAIN_POLICY_SSL,
        pChainContext,
        &certChainPolicy,
        &certChainPolicyStatus)) {
        return GetLastError();
    }

    if (certChainPolicyStatus.dwError != S_OK) {
        return certChainPolicyStatus.dwError;
    }

    return S_OK;
}

//------------------------------------------------------------------------

void SchannelContext::appendNewData(const SafeByteArray& data) {
    size_t originalSize = receivedData_.size();
    receivedData_.resize(originalSize + data.size());
    memcpy(&receivedData_[0] + originalSize, &data[0], data.size());
}

//------------------------------------------------------------------------

void SchannelContext::continueHandshake(const SafeByteArray& data) {
    appendNewData(data);

    while (!receivedData_.empty()) {
        SecBuffer inBuffers[2];

        // Provide Schannel with the remote host's handshake data
        inBuffers[0].pvBuffer = static_cast<char*>(&receivedData_[0]);
        inBuffers[0].cbBuffer = static_cast<unsigned long>(receivedData_.size());
        inBuffers[0].BufferType  = SECBUFFER_TOKEN;

        inBuffers[1].pvBuffer   = NULL;
        inBuffers[1].cbBuffer   = 0;
        inBuffers[1].BufferType = SECBUFFER_EMPTY;

        SecBufferDesc inBufferDesc = {0};
        inBufferDesc.cBuffers   = 2;
        inBufferDesc.pBuffers   = inBuffers;
        inBufferDesc.ulVersion  = SECBUFFER_VERSION;

        SecBuffer outBuffers[2];

        // We let Schannel allocate the output buffer for us
        outBuffers[0].pvBuffer   = NULL;
        outBuffers[0].cbBuffer   = 0;
        outBuffers[0].BufferType = SECBUFFER_TOKEN;

        // Contains alert data if an alert is generated
        outBuffers[1].pvBuffer   = NULL;
        outBuffers[1].cbBuffer   = 0;
        outBuffers[1].BufferType = SECBUFFER_ALERT;

        // Make sure the output buffers are freed
        ScopedSecBuffer scopedOutputData(&outBuffers[0]);
        ScopedSecBuffer scopedOutputAlertData(&outBuffers[1]);

        SecBufferDesc outBufferDesc = {0};
        outBufferDesc.cBuffers   = 2;
        outBufferDesc.pBuffers   = outBuffers;
        outBufferDesc.ulVersion  = SECBUFFER_VERSION;

        SECURITY_STATUS status = InitializeSecurityContext(
            credHandle_,
            contextHandle_,
            NULL,
            contextFlags_,
            0,
            0,
            &inBufferDesc,
            0,
            NULL,
            &outBufferDesc,
            &secContext_,
            NULL);

        if (status == SEC_E_INCOMPLETE_MESSAGE)    {
            // Wait for more data to arrive
            break;
        }
        else if (status == SEC_I_CONTINUE_NEEDED) {
            SecBuffer* pDataBuffer = &outBuffers[0];
            SecBuffer* pExtraBuffer = &inBuffers[1];

            if (pDataBuffer && pDataBuffer->cbBuffer > 0 && pDataBuffer->pvBuffer != NULL) {
                sendDataOnNetwork(pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);
            }

            if (pExtraBuffer->BufferType == SECBUFFER_EXTRA) {
                receivedData_.erase(receivedData_.begin(), receivedData_.end() - pExtraBuffer->cbBuffer);
            }
            else {
                receivedData_.clear();
            }

            break;
        }
        else if (status == SEC_E_OK)  {
            status = validateServerCertificate();
            if (status != SEC_E_OK) {
                handleCertError(status);
            }

            SecBuffer* pExtraBuffer = &inBuffers[1];

            if (pExtraBuffer && pExtraBuffer->cbBuffer > 0) {
                receivedData_.erase(receivedData_.begin(), receivedData_.end() - pExtraBuffer->cbBuffer);
            }
            else {
                receivedData_.clear();
            }

            state_ = Connected;
            determineStreamSizes();

            onConnected();
        }
        else {
            // We failed to initialize the security context
            handleCertError(status);
            indicateError(std::make_shared<TLSError>(TLSError::UnknownError));
            return;
        }
    }
}

//------------------------------------------------------------------------

void SchannelContext::handleCertError(SECURITY_STATUS status)
{
    if (status == SEC_E_UNTRUSTED_ROOT            ||
        status == CERT_E_UNTRUSTEDROOT            ||
        status == CRYPT_E_ISSUER_SERIALNUMBER    ||
        status == CRYPT_E_SIGNER_NOT_FOUND        ||
        status == CRYPT_E_NO_TRUSTED_SIGNER) {
        verificationError_ = CertificateVerificationError::Untrusted;
    }
    else if (status == SEC_E_CERT_EXPIRED ||
             status == CERT_E_EXPIRED) {
        verificationError_ = CertificateVerificationError::Expired;
    }
    else if (status == CRYPT_E_SELF_SIGNED) {
        verificationError_ = CertificateVerificationError::SelfSigned;
    }
    else if (status == CRYPT_E_HASH_VALUE    ||
             status == TRUST_E_CERT_SIGNATURE) {
        verificationError_ = CertificateVerificationError::InvalidSignature;
    }
    else if (status == CRYPT_E_REVOKED) {
        verificationError_ = CertificateVerificationError::Revoked;
    }
    else if (status == CRYPT_E_NO_REVOCATION_CHECK ||
             status == CRYPT_E_REVOCATION_OFFLINE) {
        verificationError_ = CertificateVerificationError::RevocationCheckFailed;
    }
    else if (status == CERT_E_WRONG_USAGE) {
        verificationError_ = CertificateVerificationError::InvalidPurpose;
    }
    else {
        verificationError_ = CertificateVerificationError::UnknownError;
    }
}

//------------------------------------------------------------------------

void SchannelContext::sendDataOnNetwork(const void* pData, size_t dataSize) {
    if (dataSize > 0 && pData) {
        SafeByteArray byteArray(dataSize);
        memcpy(&byteArray[0], pData, dataSize);

        onDataForNetwork(byteArray);
    }
}

//------------------------------------------------------------------------

void SchannelContext::forwardDataToApplication(const void* pData, size_t dataSize) {
    SafeByteArray byteArray(dataSize);
    memcpy(&byteArray[0], pData, dataSize);

    onDataForApplication(byteArray);
}

//------------------------------------------------------------------------

void SchannelContext::handleDataFromApplication(const SafeByteArray& data) {
    // Don't attempt to send data until we're fully connected
    if (state_ == Connecting) {
        return;
    }

    // Encrypt the data
    encryptAndSendData(data);
}

//------------------------------------------------------------------------

void SchannelContext::handleDataFromNetwork(const SafeByteArray& data) {
    switch (state_) {
    case Connecting:
        {
            // We're still establishing the connection, so continue the handshake
            continueHandshake(data);
        }
        break;

    case Connected:
        {
            // Decrypt the data
            decryptAndProcessData(data);
        }
        break;

    default:
        return;
    }
}

//------------------------------------------------------------------------

void SchannelContext::indicateError(std::shared_ptr<TLSError> error) {
    state_ = Error;
    receivedData_.clear();
    onError(error);
}

//------------------------------------------------------------------------

void SchannelContext::decryptAndProcessData(const SafeByteArray& data) {
    SecBuffer inBuffers[4]    = {0};

    appendNewData(data);

    while (!receivedData_.empty()) {
        //
        // MSDN:
        //   When using the Schannel SSP with contexts that are not connection oriented, on input,
        //   the structure must contain four SecBuffer structures. Exactly one buffer must be of type
        //   SECBUFFER_DATA and contain an encrypted message, which is decrypted in place. The remaining
        //   buffers are used for output and must be of type SECBUFFER_EMPTY. For connection-oriented
        //   contexts, a SECBUFFER_DATA type buffer must be supplied, as noted for nonconnection-oriented
        //   contexts. Additionally, a second SECBUFFER_TOKEN type buffer that contains a security token
        //   must also be supplied.
        //
        inBuffers[0].pvBuffer = static_cast<char*>(&receivedData_[0]);
        inBuffers[0].cbBuffer = static_cast<unsigned long>(receivedData_.size());
        inBuffers[0].BufferType  = SECBUFFER_DATA;

        inBuffers[1].BufferType  = SECBUFFER_EMPTY;
        inBuffers[2].BufferType  = SECBUFFER_EMPTY;
        inBuffers[3].BufferType  = SECBUFFER_EMPTY;

        SecBufferDesc inBufferDesc = {0};
        inBufferDesc.cBuffers      = 4;
        inBufferDesc.pBuffers      = inBuffers;
        inBufferDesc.ulVersion     = SECBUFFER_VERSION;

        size_t inData = receivedData_.size();
        SECURITY_STATUS status = DecryptMessage(contextHandle_, &inBufferDesc, 0, NULL);

        if (status == SEC_E_INCOMPLETE_MESSAGE) {
            // Wait for more data to arrive
            break;
        }
        else if (status == SEC_I_RENEGOTIATE) {
            // TODO: Handle renegotiation scenarios
            indicateError(std::make_shared<TLSError>(TLSError::UnknownError));
            break;
        }
        else if (status == SEC_I_CONTEXT_EXPIRED) {
            indicateError(std::make_shared<TLSError>(TLSError::UnknownError));
            break;
        }
        else if (status != SEC_E_OK) {
            indicateError(std::make_shared<TLSError>(TLSError::UnknownError));
            break;
        }

        SecBuffer* pDataBuffer = NULL;
        SecBuffer* pExtraBuffer = NULL;
        for (int i = 0; i < 4; ++i) {
            if (inBuffers[i].BufferType == SECBUFFER_DATA) {
                pDataBuffer = &inBuffers[i];
            }
            else if (inBuffers[i].BufferType == SECBUFFER_EXTRA) {
                pExtraBuffer = &inBuffers[i];
            }
        }

        if (pDataBuffer && pDataBuffer->cbBuffer > 0 && pDataBuffer->pvBuffer != NULL) {
            forwardDataToApplication(pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);
        }

        // If there is extra data left over from the decryption operation, we call DecryptMessage() again
        if (pExtraBuffer) {
            receivedData_.erase(receivedData_.begin(), receivedData_.end() - pExtraBuffer->cbBuffer);
        }
        else {
            // We're done
            receivedData_.erase(receivedData_.begin(), receivedData_.begin() + inData);
        }
    }
}

//------------------------------------------------------------------------

void SchannelContext::encryptAndSendData(const SafeByteArray& data) {
    if (streamSizes_.cbMaximumMessage == 0) {
        return;
    }

    SecBuffer outBuffers[4]    = {0};

    // Calculate the largest required size of the send buffer
    size_t messageBufferSize = (data.size() > streamSizes_.cbMaximumMessage)
                             ? streamSizes_.cbMaximumMessage
                             : data.size();

    // Allocate a packet for the encrypted data
    SafeByteArray sendBuffer;
    sendBuffer.resize(streamSizes_.cbHeader + messageBufferSize + streamSizes_.cbTrailer);

    size_t bytesSent = 0;
    do {
        size_t bytesLeftToSend = data.size() - bytesSent;

        // Calculate how much of the send buffer we'll be using for this chunk
        size_t bytesToSend = (bytesLeftToSend > streamSizes_.cbMaximumMessage)
                           ? streamSizes_.cbMaximumMessage
                           : bytesLeftToSend;

        // Copy the plain text data into the send buffer
        memcpy(&sendBuffer[0] + streamSizes_.cbHeader, &data[0] + bytesSent, bytesToSend);

        outBuffers[0].pvBuffer     = &sendBuffer[0];
        outBuffers[0].cbBuffer     = streamSizes_.cbHeader;
        outBuffers[0].BufferType = SECBUFFER_STREAM_HEADER;

        outBuffers[1].pvBuffer     = &sendBuffer[0] + streamSizes_.cbHeader;
        outBuffers[1].cbBuffer     = static_cast<unsigned long>(bytesToSend);
        outBuffers[1].BufferType = SECBUFFER_DATA;

        outBuffers[2].pvBuffer     = &sendBuffer[0] + streamSizes_.cbHeader + bytesToSend;
        outBuffers[2].cbBuffer     = streamSizes_.cbTrailer;
        outBuffers[2].BufferType = SECBUFFER_STREAM_TRAILER;

        outBuffers[3].pvBuffer   = 0;
        outBuffers[3].cbBuffer   = 0;
        outBuffers[3].BufferType = SECBUFFER_EMPTY;

        SecBufferDesc outBufferDesc = {0};
        outBufferDesc.cBuffers   = 4;
        outBufferDesc.pBuffers   = outBuffers;
        outBufferDesc.ulVersion  = SECBUFFER_VERSION;

        SECURITY_STATUS status = EncryptMessage(contextHandle_, 0, &outBufferDesc, 0);
        if (status != SEC_E_OK) {
            indicateError(std::make_shared<TLSError>(TLSError::UnknownError));
            return;
        }

        sendDataOnNetwork(&sendBuffer[0], outBuffers[0].cbBuffer + outBuffers[1].cbBuffer + outBuffers[2].cbBuffer);
        bytesSent += bytesToSend;

    } while (bytesSent < data.size());
}

//------------------------------------------------------------------------

bool SchannelContext::setClientCertificate(CertificateWithKey::ref certificate) {
    std::shared_ptr<CAPICertificate> capiCertificate = std::dynamic_pointer_cast<CAPICertificate>(certificate);
    if (!capiCertificate || capiCertificate->isNull()) {
        return false;
    }

    userCertificate_ = capiCertificate;

    // We assume that the Certificate Store Name/Certificate Name
    // are valid at this point
    certStoreName_ = capiCertificate->getCertStoreName();
    certName_ = capiCertificate->getCertName();
////At the moment this is only useful for logging:
    smartCardReader_ = capiCertificate->getSmartCardReaderName();

    capiCertificate->onCertificateCardRemoved.connect(boost::bind(&SchannelContext::handleCertificateCardRemoved, this));

    return true;
}

//------------------------------------------------------------------------
void SchannelContext::handleCertificateCardRemoved() {
    if (disconnectOnCardRemoval_) {
        indicateError(std::make_shared<TLSError>(TLSError::CertificateCardRemoved));
    }
}

//------------------------------------------------------------------------

std::vector<Certificate::ref> SchannelContext::getPeerCertificateChain() const {
    std::vector<Certificate::ref> certificateChain;
    ScopedCertContext pServerCert;
    ScopedCertContext pIssuerCert;
    ScopedCertContext pCurrentCert;
    SECURITY_STATUS status = QueryContextAttributes(contextHandle_, SECPKG_ATTR_REMOTE_CERT_CONTEXT, pServerCert.Reset());

    if (status != SEC_E_OK) {
        SWIFT_LOG(debug) << "Error while Querying the Certificate Chain" << std::endl;
        return certificateChain;
    }
    certificateChain.push_back(std::make_shared<SchannelCertificate>(pServerCert));

    pCurrentCert = pServerCert;
    while(pCurrentCert.GetPointer()) {
        DWORD dwVerificationFlags = 0;
        pIssuerCert = CertGetIssuerCertificateFromStore(pServerCert->hCertStore, pCurrentCert, NULL, &dwVerificationFlags );
        if (!(*pIssuerCert.GetPointer())) {
            break;
        }
        certificateChain.push_back(std::make_shared<SchannelCertificate>(pIssuerCert));

        pCurrentCert = pIssuerCert;
        pIssuerCert = NULL;
    }
    return certificateChain;
}

//------------------------------------------------------------------------

CertificateVerificationError::ref SchannelContext::getPeerCertificateVerificationError() const {
    return verificationError_ ? std::make_shared<CertificateVerificationError>(*verificationError_) : CertificateVerificationError::ref();
}

//------------------------------------------------------------------------

ByteArray SchannelContext::getFinishMessage() const {
    SecPkgContext_Bindings bindings;
    int ret = QueryContextAttributes(contextHandle_, SECPKG_ATTR_UNIQUE_BINDINGS, &bindings);
    if (ret == SEC_E_OK) {
        return createByteArray(((unsigned char*) bindings.Bindings) + bindings.Bindings->dwApplicationDataOffset + 11 /* tls-unique:*/, bindings.Bindings->cbApplicationDataLength - 11);
    }
    else {
        SWIFT_LOG(debug) << "Error while retrieving Finish Message" << std::endl;
    }

    return ByteArray();
}

//------------------------------------------------------------------------

void SchannelContext::setCheckCertificateRevocation(bool b) {
    checkCertificateRevocation_ = b;
}

void SchannelContext::setDisconnectOnCardRemoval(bool b) {
    disconnectOnCardRemoval_ = b;
}


}
