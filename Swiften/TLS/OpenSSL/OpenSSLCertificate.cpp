/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/TLS/OpenSSL/OpenSSLCertificate.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Log.h>

#undef X509_NAME // Windows.h defines this, and  for some reason, it doesn't get undeffed properly in x509.h
#include <openssl/x509v3.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

OpenSSLCertificate::OpenSSLCertificate(boost::shared_ptr<X509> cert) : cert(cert) {
	parse();
}


OpenSSLCertificate::OpenSSLCertificate(const ByteArray& der) {
#if OPENSSL_VERSION_NUMBER <= 0x009070cfL
	unsigned char* p = const_cast<unsigned char*>(vecptr(der));
#else
	const unsigned char* p = vecptr(der);
#endif
	cert = boost::shared_ptr<X509>(d2i_X509(NULL, &p, der.size()), X509_free);
	if (!cert) {
		SWIFT_LOG(warning) << "Error creating certificate from DER data" << std::endl;
	}
	parse();
}

ByteArray OpenSSLCertificate::toDER() const {
	ByteArray result;
	if (!cert) {
		return result;
	}
	result.resize(i2d_X509(cert.get(), NULL));
	unsigned char* p = vecptr(result);
	i2d_X509(cert.get(), &p);
	return result;
}

void OpenSSLCertificate::parse() {
	if (!cert) {
		return;
	}
	// Subject name
	X509_NAME* subjectName = X509_get_subject_name(cert.get());
	if (subjectName) {
		// Subject name
		ByteArray subjectNameData;
		subjectNameData.resize(256);
		X509_NAME_oneline(X509_get_subject_name(cert.get()), reinterpret_cast<char*>(vecptr(subjectNameData)), subjectNameData.size());
		this->subjectName = byteArrayToString(subjectNameData);

		// Common name
		int cnLoc = X509_NAME_get_index_by_NID(subjectName, NID_commonName, -1);
		while (cnLoc != -1) {
			X509_NAME_ENTRY* cnEntry = X509_NAME_get_entry(subjectName, cnLoc);
			ASN1_STRING* cnData = X509_NAME_ENTRY_get_data(cnEntry);
			commonNames.push_back(byteArrayToString(createByteArray(reinterpret_cast<const char*>(cnData->data), cnData->length)));
			cnLoc = X509_NAME_get_index_by_NID(subjectName, NID_commonName, cnLoc);
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
					addXMPPAddress(byteArrayToString(createByteArray(reinterpret_cast<const char*>(ASN1_STRING_data(xmppAddrValue)), ASN1_STRING_length(xmppAddrValue))));
				}
				else if (OBJ_cmp(otherName->type_id, dnsSRVObject.get()) == 0) {
					// SRVName
					if (otherName->value->type != V_ASN1_IA5STRING) {
						continue;
					}
					ASN1_IA5STRING* srvNameValue = otherName->value->value.ia5string;
					addSRVName(byteArrayToString(createByteArray(reinterpret_cast<const char*>(ASN1_STRING_data(srvNameValue)), ASN1_STRING_length(srvNameValue))));
				}
			}
			else if (generalName->type == GEN_DNS) {
				// DNSName
				addDNSName(byteArrayToString(createByteArray(ASN1_STRING_data(generalName->d.dNSName), ASN1_STRING_length(generalName->d.dNSName))));
			}
		}
	}
}

}
