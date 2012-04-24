/*
 * Copyright (c) 2012 Isode Limited, London, England.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/TimerFactory.h>
#include <Swiften/TLS/CAPICertificate.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/Base/Log.h>

#include <boost/bind.hpp>
#include <boost/algorithm/string/predicate.hpp>

// Size of the SHA1 hash
#define SHA1_HASH_LEN 20


namespace Swift {

CAPICertificate::CAPICertificate(const std::string& capiUri, TimerFactory* timerFactory) : 
		valid_(false), 
		uri_(capiUri),
		certStoreHandle_(0),
		scardContext_(0),
		cardHandle_(0),
		certStore_(),
		certName_(),
		smartCardReaderName_(),
		timerFactory_(timerFactory),
		lastPollingResult_(true) {
	assert(timerFactory_);

	setUri(capiUri);
}

CAPICertificate::~CAPICertificate() {
	if (smartCardTimer_) {
		smartCardTimer_->stop();
		smartCardTimer_->onTick.disconnect(boost::bind(&CAPICertificate::handleSmartCardTimerTick, this));
		smartCardTimer_.reset();
	}

	if (certStoreHandle_) {
		CertCloseStore(certStoreHandle_, 0);
	}

	if (cardHandle_) {
		(void) SCardDisconnect(cardHandle_, SCARD_LEAVE_CARD);
	}

	if (scardContext_) {
		SCardReleaseContext(scardContext_);
	}
}

bool CAPICertificate::isNull() const {
	return uri_.empty() || !valid_;
}

const std::string& CAPICertificate::getCertStoreName() const {
	return certStore_;
}

const std::string& CAPICertificate::getCertName() const {
	return certName_;
}

const std::string& CAPICertificate::getSmartCardReaderName() const {
	return smartCardReaderName_;
}

PCCERT_CONTEXT findCertificateInStore (HCERTSTORE certStoreHandle, const std::string &certName) {
	if (!boost::iequals(certName.substr(0, 5), "sha1:")) {

		// Find client certificate. Note that this sample just searches for a
		// certificate that contains the user name somewhere in the subject name.
		return CertFindCertificateInStore(certStoreHandle, X509_ASN_ENCODING, /*dwFindFlags*/ 0, CERT_FIND_SUBJECT_STR_A, /* *pvFindPara*/certName.c_str(), /*pPrevCertContext*/ NULL);
	}


	std::string hexstring = certName.substr(5);
	ByteArray byteArray = Hexify::unhexify(hexstring);
	CRYPT_HASH_BLOB HashBlob;

	if (byteArray.size() != SHA1_HASH_LEN) {
		return NULL;
	}
	HashBlob.cbData = SHA1_HASH_LEN;
	HashBlob.pbData = static_cast<BYTE *>(vecptr(byteArray));

	// Find client certificate. Note that this sample just searches for a
	// certificate that contains the user name somewhere in the subject name.
	return CertFindCertificateInStore(certStoreHandle, X509_ASN_ENCODING, /* dwFindFlags */ 0, CERT_FIND_HASH, &HashBlob, /* pPrevCertContext */ NULL);

}


void CAPICertificate::setUri (const std::string& capiUri) {
	valid_ = false;

	/* Syntax: "certstore:" <cert_store> ":" <hash> ":" <hash_of_cert> */

	if (!boost::iequals(capiUri.substr(0, 10), "certstore:")) {
		return;
	}

	/* Substring of subject: uses "storename" */
	std::string capiIdentity = capiUri.substr(10);
	std::string newCertStoreName;
	size_t pos = capiIdentity.find_first_of (':');

	if (pos == std::string::npos) {
		/* Using the default certificate store */
		newCertStoreName = "MY";
		certName_ = capiIdentity;
	}
	else {
		newCertStoreName = capiIdentity.substr(0, pos);
		certName_ = capiIdentity.substr(pos + 1);
	}

	if (certStoreHandle_ != NULL) {
		if (newCertStoreName != certStore_) {
			CertCloseStore(certStoreHandle_, 0);
			certStoreHandle_ = NULL;
		}
	}

	if (certStoreHandle_ == NULL) {
		certStoreHandle_ = CertOpenSystemStore(0, newCertStoreName.c_str());
		if (!certStoreHandle_) {
			return;
		}
	}

	certStore_ = newCertStoreName;

	PCCERT_CONTEXT certContext = findCertificateInStore (certStoreHandle_, certName_);

	if (!certContext) {
		return;
	}


	/* Now verify that we can have access to the corresponding private key */

	DWORD len;
	CRYPT_KEY_PROV_INFO *pinfo;
	HCRYPTPROV hprov;
	HCRYPTKEY key;

	if (!CertGetCertificateContextProperty(certContext,
			CERT_KEY_PROV_INFO_PROP_ID,
			NULL,
			&len)) {
		CertFreeCertificateContext(certContext);
		return;
	}

	pinfo = static_cast<CRYPT_KEY_PROV_INFO *>(malloc(len));
	if (!pinfo) {
		CertFreeCertificateContext(certContext);
		return;
	}

	if (!CertGetCertificateContextProperty(certContext, CERT_KEY_PROV_INFO_PROP_ID, pinfo, &len)) {
		CertFreeCertificateContext(certContext);
		free(pinfo);
		return;
	}

	CertFreeCertificateContext(certContext);

	// Now verify if we have access to the private key
	if (!CryptAcquireContextW(&hprov, pinfo->pwszContainerName, pinfo->pwszProvName, pinfo->dwProvType, 0)) {
		free(pinfo);
		return;
	}


	char smartCardReader[1024];
	DWORD bufferLength = sizeof(smartCardReader);
	if (!CryptGetProvParam(hprov, PP_SMARTCARD_READER, (BYTE *)&smartCardReader, &bufferLength, 0)) {
		DWORD error = GetLastError();
		smartCardReaderName_ = "";
	}
	else {
		smartCardReaderName_ = smartCardReader;

		LONG result = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &scardContext_);
		if (SCARD_S_SUCCESS == result) {
			// Initiate monitoring for smartcard ejection
			smartCardTimer_ = timerFactory_->createTimer(SMARTCARD_EJECTION_CHECK_FREQUENCY_MILLISECONDS);
		}
		else {
			///Need to handle an error here
		}
	}

	if (!CryptGetUserKey(hprov, pinfo->dwKeySpec, &key)) {
		CryptReleaseContext(hprov, 0);
		free(pinfo);
		return;
	}

	CryptDestroyKey(key);
	CryptReleaseContext(hprov, 0);
	free(pinfo);

	if (smartCardTimer_) {
		smartCardTimer_->onTick.connect(boost::bind(&CAPICertificate::handleSmartCardTimerTick, this));
		smartCardTimer_->start();
	}

	valid_ = true;
}

static void smartcard_check_status (SCARDCONTEXT hContext,
		const char* pReader,
		SCARDHANDLE hCardHandle, /* Can be 0 on the first call */
		SCARDHANDLE* newCardHandle, /* The handle returned */
		DWORD* pdwState) {
	if (hCardHandle == 0) {
		DWORD dwAP;
		LONG result = SCardConnect(hContext, pReader, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCardHandle, &dwAP);
		if (SCARD_S_SUCCESS != result) {
			hCardHandle = 0;
			if (SCARD_E_NO_SMARTCARD == result || SCARD_W_REMOVED_CARD == result) {
				*pdwState = SCARD_ABSENT;
			}
			else {
				*pdwState = SCARD_UNKNOWN;
			}

			if (newCardHandle == NULL) {
				(void) SCardDisconnect(hCardHandle, SCARD_LEAVE_CARD);
				hCardHandle = 0;
			}
			else {
				*newCardHandle = hCardHandle;
			}
		}
	}

	char szReader[200];
	DWORD cch = sizeof(szReader);
	BYTE bAttr[32];
	DWORD cByte = 32;
	LONG result = SCardStatus(hCardHandle, /* Unfortunately we can't use NULL here */ szReader, &cch, pdwState, NULL, (LPBYTE)&bAttr, &cByte);

	if (SCARD_S_SUCCESS != result) {
		if (SCARD_E_NO_SMARTCARD == result || SCARD_W_REMOVED_CARD == result) {
			*pdwState = SCARD_ABSENT;
		}
		else {
			*pdwState = SCARD_UNKNOWN;
		}
	}

	if (newCardHandle == NULL) {
		(void) SCardDisconnect(hCardHandle, SCARD_LEAVE_CARD);
		hCardHandle = 0;
	}
	else {
		*newCardHandle = hCardHandle;
	}
}

bool CAPICertificate::checkIfSmartCardPresent () {
	if (!smartCardReaderName_.empty()) {
		DWORD dwState;
		smartcard_check_status(scardContext_, smartCardReaderName_.c_str(), cardHandle_, &cardHandle_, &dwState);

		switch (dwState) {
			case SCARD_ABSENT:
				SWIFT_LOG(debug) << "Card absent." << std::endl;
				break;
			case SCARD_PRESENT:
				SWIFT_LOG(debug) << "Card present." << std::endl;
				break;
			case SCARD_SWALLOWED:
				SWIFT_LOG(debug) << "Card swallowed." << std::endl;
				break;
			case SCARD_POWERED:
				SWIFT_LOG(debug) << "Card has power." << std::endl;
				break;
			case SCARD_NEGOTIABLE:
				SWIFT_LOG(debug) << "Card reset and waiting PTS negotiation." << std::endl;
				break;
			case SCARD_SPECIFIC:
				SWIFT_LOG(debug) << "Card has specific communication protocols set." << std::endl;
				break;
			default:
				SWIFT_LOG(debug) << "Unknown or unexpected card state." << std::endl;
				break;
		}



		switch (dwState) {
			case SCARD_ABSENT:
				return false;

			case SCARD_PRESENT:
			case SCARD_SWALLOWED:
			case SCARD_POWERED:
			case SCARD_NEGOTIABLE:
			case SCARD_SPECIFIC:
				return true;

			default:
				return false;
		}
	}
	else {
		return false;
	}
}

void CAPICertificate::handleSmartCardTimerTick() {
	bool poll = checkIfSmartCardPresent();
	if (lastPollingResult_ && !poll) {
		onCertificateCardRemoved();
	} 
	lastPollingResult_ = poll;
	smartCardTimer_->start();
}

}
