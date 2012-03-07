/*
 * Copyright (c) 2012 Isode Limited, London, England.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */
#pragma once

#include <Swiften/TLS/CAPICertificate.h>
#include <Swiften/StringCodecs/Hexify.h>

#include <boost/algorithm/string/predicate.hpp>

// Size of the SHA1 hash
#define SHA1_HASH_LEN                20


namespace Swift {
CAPICertificate::CAPICertificate(const std::string& capiUri)
			  		 : valid_(false), uri_(capiUri), certStoreHandle_(0), certStore_(), certName_() {
	setUri(capiUri);
}

CAPICertificate::~CAPICertificate() {
	if (certStoreHandle_) {
		CertCloseStore(certStoreHandle_, 0);
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

	if (!CryptGetUserKey(hprov, pinfo->dwKeySpec, &key)) {
		CryptReleaseContext(hprov, 0);
		free(pinfo);
		return;
	}

	CryptDestroyKey(key);
	CryptReleaseContext(hprov, 0);
	free(pinfo);

	valid_ = true;
}

}
