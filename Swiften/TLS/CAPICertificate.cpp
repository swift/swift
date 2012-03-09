/*
 * Copyright (c) 2012 Isode Limited, London, England.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */
#pragma once

#include <Swiften/Network/TimerFactory.h>
#include <Swiften/TLS/CAPICertificate.h>
#include <Swiften/StringCodecs/Hexify.h>

#include <boost/bind.hpp>
#include <boost/algorithm/string/predicate.hpp>

// Size of the SHA1 hash
#define SHA1_HASH_LEN                20


namespace Swift {

CAPICertificate::CAPICertificate(const std::string& capiUri, TimerFactory* timerFactory)
	: valid_(false),
	uri_(capiUri),
	certStoreHandle_(0),
	scardContext_(0),
	cardHandle_(0),
	certStore_(),
	certName_(),
	smartCardReaderName_(),
	timerFactory_(timerFactory) {

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
	PCCERT_CONTEXT pCertContext = NULL;

	if (!boost::iequals(certName.substr(0, 5), "sha1:")) {

		// Find client certificate. Note that this sample just searches for a
		// certificate that contains the user name somewhere in the subject name.
		pCertContext = CertFindCertificateInStore(certStoreHandle,
				X509_ASN_ENCODING,
				0,				// dwFindFlags
				CERT_FIND_SUBJECT_STR_A,
				certName.c_str(),		// *pvFindPara
				NULL );				// pPrevCertContext
		return pCertContext;
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
	pCertContext = CertFindCertificateInStore(certStoreHandle,
			X509_ASN_ENCODING,
			0,				// dwFindFlags
			CERT_FIND_HASH,
			&HashBlob,
			NULL );				// pPrevCertContext

	return pCertContext;
}


void CAPICertificate::setUri (const std::string& capiUri) {

	valid_ = false;

	/* Syntax: "certstore:" <cert_store> ":" <hash> ":" <hash_of_cert> */

	if (!boost::iequals(capiUri.substr(0, 10), "certstore:")) {
		return;
	}

	/* Substring of subject: uses "storename" */
	std::string capi_identity = capiUri.substr(10);
	std::string new_certStore_name;
	size_t pos = capi_identity.find_first_of (':');

	if (pos == std::string::npos) {
		/* Using the default certificate store */
		new_certStore_name = "MY";
		certName_ = capi_identity;
	}
	else {
		new_certStore_name = capi_identity.substr(0, pos);
		certName_ = capi_identity.substr(pos + 1);
	}

	PCCERT_CONTEXT pCertContext = NULL;

	if (certStoreHandle_ != NULL) {
		if (new_certStore_name != certStore_) {
			CertCloseStore(certStoreHandle_, 0);
			certStoreHandle_ = NULL;
		}
	}

	if (certStoreHandle_ == NULL) {
		certStoreHandle_ = CertOpenSystemStore(0, new_certStore_name.c_str());
		if (!certStoreHandle_) {
			return;
		}
	}

	certStore_ = new_certStore_name;

	pCertContext = findCertificateInStore (certStoreHandle_, certName_);

	if (!pCertContext) {
		return;
	}


	/* Now verify that we can have access to the corresponding private key */

	DWORD len;
	CRYPT_KEY_PROV_INFO *pinfo;
	HCRYPTPROV hprov;
	HCRYPTKEY key;

	if (!CertGetCertificateContextProperty(pCertContext,
			CERT_KEY_PROV_INFO_PROP_ID,
			NULL,
			&len)) {
		CertFreeCertificateContext(pCertContext);
		return;
	}

	pinfo = static_cast<CRYPT_KEY_PROV_INFO *>(malloc(len));
	if (!pinfo) {
		CertFreeCertificateContext(pCertContext);
		return;
	}

	if (!CertGetCertificateContextProperty(pCertContext,
			CERT_KEY_PROV_INFO_PROP_ID,
			pinfo,
			&len)) {
		CertFreeCertificateContext(pCertContext);
		free(pinfo);
		return;
	}

	CertFreeCertificateContext(pCertContext);

	// Now verify if we have access to the private key
	if (!CryptAcquireContextW(&hprov,
			pinfo->pwszContainerName,
			pinfo->pwszProvName,
			pinfo->dwProvType,
			0)) {
		free(pinfo);
		return;
	}


	char smartcard_reader[1024];
	DWORD buflen;

	buflen = sizeof(smartcard_reader);
	if (!CryptGetProvParam(hprov, PP_SMARTCARD_READER, (BYTE *)&smartcard_reader, &buflen, 0)) {
		DWORD error;

		error = GetLastError();
		smartCardReaderName_ = "";
	} else {
		LONG       lRet;

		smartCardReaderName_ = smartcard_reader;

		lRet = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &scardContext_);
		if (SCARD_S_SUCCESS == lRet) {
			// Initiate monitoring for smartcard ejection
			smartCardTimer_ = timerFactory_->createTimer(SMARTCARD_EJECTION_CHECK_FREQ);
		} else {
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

static void smartcard_check_status (SCARDCONTEXT  hContext,
				    const char * pReader,
				    SCARDHANDLE hCardHandle,     // Can be 0 on the first call
				    SCARDHANDLE * newCardHandle, // The handle returned
				    DWORD * pdwState) {
	LONG            lReturn;
	DWORD           dwAP;
	char            szReader[200];
	DWORD           cch = sizeof(szReader);
	BYTE            bAttr[32];
	DWORD           cByte = 32;

	if (hCardHandle == 0) {
		lReturn = SCardConnect(hContext,
					pReader,
					SCARD_SHARE_SHARED,
					SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
					&hCardHandle,
					&dwAP);
		if ( SCARD_S_SUCCESS != lReturn ) {
			hCardHandle = 0;
			if (SCARD_E_NO_SMARTCARD == lReturn || SCARD_W_REMOVED_CARD == lReturn) {
				*pdwState = SCARD_ABSENT;
			} else {
				*pdwState = SCARD_UNKNOWN;
			}
			goto done;
		}
	}

	lReturn = SCardStatus(hCardHandle,
				szReader,	// Unfortunately we can't use NULL here
				&cch,
				pdwState,
				NULL,
				(LPBYTE)&bAttr,
				&cByte);

	if ( SCARD_S_SUCCESS != lReturn ) {
		if (SCARD_E_NO_SMARTCARD == lReturn || SCARD_W_REMOVED_CARD == lReturn) {
			*pdwState = SCARD_ABSENT;
		} else {
			*pdwState = SCARD_UNKNOWN;
		}
	}

done:
	if (newCardHandle == NULL) {
		(void) SCardDisconnect(hCardHandle, SCARD_LEAVE_CARD);
		hCardHandle = 0;
	} else {
		*newCardHandle = hCardHandle;
	}
}

bool CAPICertificate::checkIfSmartCardPresent () {

	DWORD dwState;

	if (!smartCardReaderName_.empty()) {
		smartcard_check_status (scardContext_,
					smartCardReaderName_.c_str(),
					cardHandle_,
					&cardHandle_,
					&dwState);
////DEBUG
		switch ( dwState ) {
		case SCARD_ABSENT:
		    printf("Card absent.\n");
		    break;
		case SCARD_PRESENT:
		    printf("Card present.\n");
		    break;
		case SCARD_SWALLOWED:
		    printf("Card swallowed.\n");
		    break;
		case SCARD_POWERED:
		    printf("Card has power.\n");
		    break;
		case SCARD_NEGOTIABLE:
		    printf("Card reset and waiting PTS negotiation.\n");
		    break;
		case SCARD_SPECIFIC:
		    printf("Card has specific communication protocols set.\n");
		    break;
		default:
		    printf("Unknown or unexpected card state.\n");
		    break;
		}



		switch ( dwState ) {
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
	} else {
		return false;
	}
}

void CAPICertificate::handleSmartCardTimerTick() {

	if (checkIfSmartCardPresent() == false) {
		smartCardTimer_->stop();
		onCertificateCardRemoved();
	} else {
		smartCardTimer_->start();
	}
}

}
