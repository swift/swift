/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/TLS/OpenSSL/OpenSSLCertificate.h"

#include "Swiften/Base/ByteArray.h"

#undef X509_NAME // Windows.h defines this, and  for some reason, it doesn't get undeffed properly in x509.h
#include <openssl/x509v3.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

OpenSSLCertificate::OpenSSLCertificate(boost::shared_ptr<X509> cert) : cert(cert) {
	// Common name
	X509_NAME* subjectName = X509_get_subject_name(cert.get());
	if (subjectName) {
		int cnLoc = X509_NAME_get_index_by_NID(subjectName, NID_commonName, -1);
		if (cnLoc != -1) {
			X509_NAME_ENTRY* cnEntry = X509_NAME_get_entry(subjectName, cnLoc);
			ASN1_STRING* cnData = X509_NAME_ENTRY_get_data(cnEntry);
			setCommonName(ByteArray(cnData->data, cnData->length).toString());
		}
	}

	// subjectAltNames
	int subjectAltNameLoc = X509_get_ext_by_NID(cert.get(), NID_subject_alt_name, -1);
	if(subjectAltNameLoc != -1) {
		X509_EXTENSION* extension = X509_get_ext(cert.get(), subjectAltNameLoc);
		boost::shared_ptr<GENERAL_NAMES> generalNames(reinterpret_cast<GENERAL_NAMES*>(X509V3_EXT_d2i(extension)), GENERAL_NAMES_free);
		boost::shared_ptr<ASN1_OBJECT> xmppAddrObject(OBJ_txt2obj(ID_ON_XMPPADDR_OID, 1), ASN1_OBJECT_free);
		boost::shared_ptr<ASN1_OBJECT> dnsSRVObject(OBJ_txt2obj(ID_ON_DNSSRV_OID, 1), ASN1_OBJECT_free);
		for (int i = 0; i < sk_GENERAL_NAME_num(generalNames.get()); ++i) {
			GENERAL_NAME* generalName = sk_GENERAL_NAME_value(generalNames.get(), i);
			if (generalName->type == GEN_OTHERNAME) {
				OTHERNAME* otherName = generalName->d.otherName;
				if (OBJ_cmp(otherName->type_id, xmppAddrObject.get()) == 0) {
					// XmppAddr
					if (otherName->value->type != V_ASN1_UTF8STRING) {
						continue;
					}
					ASN1_UTF8STRING* xmppAddrValue = otherName->value->value.utf8string;
					addXMPPAddress(ByteArray(ASN1_STRING_data(xmppAddrValue), ASN1_STRING_length(xmppAddrValue)).toString());
				}
				else if (OBJ_cmp(otherName->type_id, dnsSRVObject.get()) == 0) {
					// SRVName
					if (otherName->value->type != V_ASN1_IA5STRING) {
						continue;
					}
					ASN1_IA5STRING* srvNameValue = otherName->value->value.ia5string;
					addSRVName(ByteArray(ASN1_STRING_data(srvNameValue), ASN1_STRING_length(srvNameValue)).toString());
				}
			}
			else if (generalName->type == GEN_DNS) {
				// DNSName
				addDNSName(ByteArray(ASN1_STRING_data(generalName->d.dNSName), ASN1_STRING_length(generalName->d.dNSName)).toString());
			}
		}
	}
}

}
