/*
 * Copyright (c) 2012 Isode Limited, London, England.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <string>

#define SECURITY_WIN32
#include <Windows.h>
#include <WinCrypt.h>
#include <cryptuiapi.h>

#include "CAPICertificateSelector.h"

namespace Swift {

#define cert_dlg_title L"TLS Client Certificate Selection"
#define cert_dlg_prompt L"Select a certificate to use for authentication"
/////Hmm, maybe we should not exlude the "location" column
#define exclude_columns	 CRYPTUI_SELECT_LOCATION_COLUMN \
			|CRYPTUI_SELECT_INTENDEDUSE_COLUMN



static std::string getCertUri(PCCERT_CONTEXT cert, const char * cert_store_name) {
	DWORD required_size;
	char * comma;
	char * p_in;
	char * p_out;
	char * subject_name;
	std::string ret = std::string("certstore:") + cert_store_name + ":";

	required_size = CertNameToStrA(cert->dwCertEncodingType,
				&cert->pCertInfo->Subject,
				/* Discard attribute names: */
				CERT_SIMPLE_NAME_STR | CERT_NAME_STR_REVERSE_FLAG,
				NULL,
				0);

	subject_name = static_cast<char *>(malloc(required_size+1));

	if (!CertNameToStrA(cert->dwCertEncodingType,
			    &cert->pCertInfo->Subject,
			    /* Discard attribute names: */
			    CERT_SIMPLE_NAME_STR | CERT_NAME_STR_REVERSE_FLAG,
			    subject_name,
			    required_size)) {
		return "";
	}

	/* Now search for the "," (ignoring escapes)
	    and truncate the rest of the string */
	if (subject_name[0] == '"') {
		for (comma = subject_name + 1; comma[0]; comma++) {
			if (comma[0] == '"') {
				comma++;
				if (comma[0] != '"') {
					break;
				}
			}
		}
	} else {
		comma = strchr(subject_name, ',');
	}

	if (comma != NULL) {
		*comma = '\0';
	}

	/* We now need to unescape the returned RDN */
	if (subject_name[0] == '"') {
		for (p_in = subject_name + 1, p_out = subject_name; p_in[0]; p_in++, p_out++) {
			if (p_in[0] == '"') {
				p_in++;
			}

			p_out[0] = p_in[0];
		}
		p_out[0] = '\0';
	}

	ret += subject_name;
	free(subject_name);

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


}
