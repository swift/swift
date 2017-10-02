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

#pragma once

#include <boost/signals2.hpp>

#include <Swiften/TLS/TLSContext.h>
#include <Swiften/TLS/Schannel/SchannelUtil.h>
#include <Swiften/TLS/CertificateWithKey.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/TLS/TLSError.h>

#define SECURITY_WIN32
#include <Windows.h>
#include <Schannel.h>
#include <security.h>
#include <schnlsp.h>

#include <boost/noncopyable.hpp>

namespace Swift
{
    class CAPICertificate;
    class SchannelContext : public TLSContext, boost::noncopyable
    {
    public:
        typedef std::shared_ptr<SchannelContext> sp_t;

    public:
        SchannelContext(bool tls1_0Workaround);

        virtual ~SchannelContext();

        //
        // TLSContext
        //
        virtual void    connect();
        virtual bool    setClientCertificate(CertificateWithKey::ref cert);

        virtual void    handleDataFromNetwork(const SafeByteArray& data);
        virtual void    handleDataFromApplication(const SafeByteArray& data);

        virtual std::vector<Certificate::ref> getPeerCertificateChain() const;
        virtual CertificateVerificationError::ref getPeerCertificateVerificationError() const;

        virtual ByteArray getFinishMessage() const;

        virtual void setCheckCertificateRevocation(bool b);

        virtual void setDisconnectOnCardRemoval(bool b);

    private:
        void            determineStreamSizes();
        void            continueHandshake(const SafeByteArray& data);
        void            indicateError(std::shared_ptr<TLSError> error);
        //FIXME: Remove
        void indicateError() {indicateError(std::make_shared<TLSError>());}
        void            handleCertError(SECURITY_STATUS status) ;

        void            sendDataOnNetwork(const void* pData, size_t dataSize);
        void            forwardDataToApplication(const void* pData, size_t dataSize);

        void            decryptAndProcessData(const SafeByteArray& data);
        void            encryptAndSendData(const SafeByteArray& data);

        void            appendNewData(const SafeByteArray& data);
        SECURITY_STATUS validateServerCertificate();

        void            handleCertificateCardRemoved();

    private:
        enum SchannelState
        {
            Start,
            Connecting,
            Connected,
            Error

        };

        SchannelState        state_;
        boost::optional<CertificateVerificationError> verificationError_;

        ULONG                secContext_;
        ScopedCredHandle    credHandle_;
        ScopedCtxtHandle    contextHandle_;
        DWORD                contextFlags_;
        SecPkgContext_StreamSizes streamSizes_;

        std::vector<char>    receivedData_;

        HCERTSTORE        myCertStore_;
        std::string        certStoreName_;
        std::string        certName_;
////Not needed, most likely
        std::string        smartCardReader_;    //Can be empty string for non SmartCard certificates
        std::shared_ptr<CAPICertificate> userCertificate_;
        bool checkCertificateRevocation_;
        bool tls1_0Workaround_;
        bool disconnectOnCardRemoval_;
    };
}
