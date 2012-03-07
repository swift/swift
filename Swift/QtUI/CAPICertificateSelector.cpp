/*
 * Copyright (c) 2012 Isode Limited, London, England.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <string>

#include "CAPICertificateSelector.h"

#define SECURITY_WIN32
#include <Windows.h>
#include <WinCrypt.h>
#include <cryptuiapi.h>

#include <Swiften/StringCodecs/Hexify.h>
#include <boost/algorithm/string.hpp>

namespace Swift {

#define cert_dlg_title L"TLS Client Certificate Selection"
#define cert_dlg_prompt L"Select a certificate to use for authentication"
/////Hmm, maybe we should not exlude the "location" column
#define exclude_columns	 CRYPTUI_SELECT_LOCATION_COLUMN \
			|CRYPTUI_SELECT_INTENDEDUSE_COLUMN

// Size of the SHA1 hash
#define SHA1_HASH_LEN                20

static std::string getCertUri(PCCERT_CONTEXT cert, const char * cert_store_name) {
	DWORD cbHash = SHA1_HASH_LEN;
	BYTE aHash[SHA1_HASH_LEN];
	std::string ret("certstore:");

	ret += cert_store_name;
	ret += ":sha1:";

	if (CertGetCertificateContextProperty(cert,
		 CERT_HASH_PROP_ID,
		 aHash,
		 &cbHash) == FALSE ) {
		return "";
	}

	ByteArray byteArray = createByteArray((char *)(&aHash[0]), cbHash);
	ret += Hexify::hexify(byteArray);

	return ret;
}

std::string selectCAPICertificate() {

	const char * cert_store_name = "MY";
	PCCERT_CONTEXT cert;
	DWORD store_flags;
	HCERTSTORE hstore;
	HWND hwnd;

	store_flags = CERT_STORE_OPEN_EXISTING_FLAG |
		      CERT_STORE_READONLY_FLAG |
		      CERT_SYSTEM_STORE_CURRENT_USER;

	hstore = CertOpenStore(CERT_STORE_PROV_SYSTEM_A, 0, 0, store_flags, cert_store_name);
	if (!hstore) {
		return "";
	}


////Does this handle need to be freed as well?
	hwnd = GetForegroundWindow();
	if (!hwnd) {
		hwnd = GetActiveWindow();
	}

	/* Call Windows dialog to select a suitable certificate */
	cert = CryptUIDlgSelectCertificateFromStore(hstore,
						  hwnd,
						  cert_dlg_title,
						  cert_dlg_prompt,
						  exclude_columns,
						  0,
						  NULL);

	if (hstore) {
		CertCloseStore(hstore, 0);
	}

	if (cert) {
		std::string ret = getCertUri(cert, cert_store_name);

		CertFreeCertificateContext(cert);

		return ret;
	} else {
		return "";
	}
}

bool isCAPIURI(std::string uri) {
	return (boost::iequals(uri.substr(0, 10), "certstore:"));
}

}
