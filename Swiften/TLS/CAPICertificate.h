/*
 * Copyright (c) 2012 Isode Limited, London, England.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/TLS/CertificateWithKey.h>
#include <Swiften/Network/Timer.h>

#define SECURITY_WIN32
#include <Windows.h>
#include <WinCrypt.h>
#include <Winscard.h>

#define SMARTCARD_EJECTION_CHECK_FREQUENCY_MILLISECONDS	1000

namespace Swift {
	class TimerFactory;

	class SWIFTEN_API CAPICertificate : public Swift::CertificateWithKey {
		public:
			CAPICertificate(const std::string& capiUri, TimerFactory* timerFactory);

			virtual ~CAPICertificate();

			virtual bool isNull() const;

			const std::string& getCertStoreName() const;

			const std::string& getCertName() const;

			const std::string& getSmartCardReaderName() const;

		public:
			boost::signal<void ()> onCertificateCardRemoved;

		private:
			void setUri (const std::string& capiUri);

			void handleSmartCardTimerTick();

			bool checkIfSmartCardPresent();

		private:
			bool valid_;
			std::string uri_;

			HCERTSTORE certStoreHandle_;
			SCARDCONTEXT  scardContext_;
			SCARDHANDLE   cardHandle_;

			/* Parsed components of the uri_ */
			std::string certStore_;
			std::string certName_;
			std::string smartCardReaderName_;
			boost::shared_ptr<Timer> smartCardTimer_;
			TimerFactory* timerFactory_;

			bool lastPollingResult_;
	};

PCCERT_CONTEXT findCertificateInStore (HCERTSTORE certStoreHandle, const std::string &certName);

}
