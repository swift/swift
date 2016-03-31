/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/SASL/ClientAuthenticator.h>

namespace Swift {
    class IDNConverter;
    class CryptoProvider;

    class SWIFTEN_API SCRAMSHA1ClientAuthenticator : public ClientAuthenticator {
        public:
            SCRAMSHA1ClientAuthenticator(const std::string& nonce, bool useChannelBinding, IDNConverter*, CryptoProvider*);

            void setTLSChannelBindingData(const ByteArray& channelBindingData);

            virtual boost::optional<SafeByteArray> getResponse() const;
            virtual bool setChallenge(const boost::optional<ByteArray>&);

        private:
            ByteArray getInitialBareClientMessage() const;
            ByteArray getGS2Header() const;
            ByteArray getFinalMessageWithoutProof() const;

            static std::map<char, std::string> parseMap(const std::string&);

        private:
            enum Step {
                Initial,
                Proof,
                Final
            } step;
            std::string clientnonce;
            ByteArray initialServerMessage;
            ByteArray serverNonce;
            ByteArray authMessage;
            ByteArray saltedPassword;
            ByteArray serverSignature;
            bool useChannelBinding;
            IDNConverter* idnConverter;
            CryptoProvider* crypto;
            boost::optional<ByteArray> tlsChannelBindingData;
    };
}
