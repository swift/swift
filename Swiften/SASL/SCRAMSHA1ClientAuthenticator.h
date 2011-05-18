/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <boost/optional.hpp>

#include <string>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/SASL/ClientAuthenticator.h>

namespace Swift {
	class SCRAMSHA1ClientAuthenticator : public ClientAuthenticator {
		public:
			SCRAMSHA1ClientAuthenticator(const std::string& nonce, bool useChannelBinding = false);

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
			boost::optional<ByteArray> tlsChannelBindingData;
	};
}
