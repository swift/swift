/*
 * Copyright (c) 2012 Isode Limited, London, England.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/TLS/CertificateWithKey.h>

#define SECURITY_WIN32
#include <Windows.h>
#include <WinCrypt.h>

namespace Swift {
	class CAPICertificate : public Swift::CertificateWithKey {
		public:
			CAPICertificate(const std::string& capiUri);

			virtual ~CAPICertificate();

			virtual bool isNull() const;

			const std::string& getCertStoreName() const;

			const std::string& getCertName() const;

		private:
			void setUri (const std::string& capiUri);

		private:
			bool valid_;
			std::string uri_;

			HCERTSTORE certStoreHandle_;

			/* Parsed components of the uri_ */
			std::string certStore_;
			std::string certName_;
	};

PCCERT_CONTEXT findCertificateInStore (HCERTSTORE certStoreHandle, const std::string &certName);

}
