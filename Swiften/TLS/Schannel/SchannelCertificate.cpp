/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "Swiften/TLS/Schannel/SchannelCertificate.h"
#include "Swiften/Base/ByteArray.h"

#define SECURITY_WIN32
#include <Windows.h>
#include <Schannel.h>
#include <security.h>
#include <schnlsp.h>
#include <Wincrypt.h>

using std::vector;

namespace Swift {

//------------------------------------------------------------------------

SchannelCertificate::SchannelCertificate(const ScopedCertContext& certCtxt) 
: m_cert(certCtxt) 
{
	parse();
}

//------------------------------------------------------------------------

SchannelCertificate::SchannelCertificate(const ByteArray& der)
{
	if (!der.empty())
	{
		// Convert the DER encoded certificate to a PCERT_CONTEXT
		CERT_BLOB certBlob = {0};
		certBlob.cbData = der.size();
		certBlob.pbData = (BYTE*)&der[0];

		if (!CryptQueryObject(
			CERT_QUERY_OBJECT_BLOB,
			&certBlob,
			CERT_QUERY_CONTENT_FLAG_CERT,
			CERT_QUERY_FORMAT_FLAG_ALL,
			0,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			(const void**)m_cert.Reset()))
		{
			// TODO: Because Swiften isn't exception safe, we have no way to indicate failure
		}
	}
}

//------------------------------------------------------------------------

ByteArray SchannelCertificate::toDER() const 
{
 	ByteArray result;

	// Serialize the certificate. The CERT_CONTEXT is already DER encoded.
	result.resize(m_cert->cbCertEncoded);
	memcpy(&result[0], m_cert->pbCertEncoded, result.size());
	
 	return result;
}

//------------------------------------------------------------------------

std::string SchannelCertificate::wstrToStr(const std::wstring& wstr)
{
	if (wstr.empty())
		return "";

	// First request the size of the required UTF-8 buffer
	int numRequiredBytes = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	if (!numRequiredBytes)
		return "";

	// Allocate memory for the UTF-8 string
	std::vector<char> utf8Str(numRequiredBytes);

	int numConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), &utf8Str[0], numRequiredBytes, NULL, NULL);
	if (!numConverted)
		return "";

	std::string str(&utf8Str[0], numConverted);
	return str;
}

//------------------------------------------------------------------------

void SchannelCertificate::parse() 
{
	//
	// Subject name
	//
	DWORD requiredSize = CertNameToStr(X509_ASN_ENCODING, &m_cert->pCertInfo->Subject, CERT_OID_NAME_STR, NULL, 0);
	if (requiredSize > 1)
	{
		vector<char> rawSubjectName(requiredSize);
		CertNameToStr(X509_ASN_ENCODING, &m_cert->pCertInfo->Subject, CERT_OID_NAME_STR, &rawSubjectName[0], rawSubjectName.size());
		m_subjectName = std::string(&rawSubjectName[0]);
	}

	//
	// Common name
	//
	// Note: We only pull out one common name from the cert.
	requiredSize = CertGetNameString(m_cert, CERT_NAME_ATTR_TYPE, 0, szOID_COMMON_NAME, NULL, 0);
	if (requiredSize > 1)
	{
		vector<char> rawCommonName(requiredSize);
		requiredSize = CertGetNameString(m_cert, CERT_NAME_ATTR_TYPE, 0, szOID_COMMON_NAME, &rawCommonName[0], rawCommonName.size());
		m_commonNames.push_back( std::string(&rawCommonName[0]) );
	}
	
	//
	// Subject alternative names
	//
	PCERT_EXTENSION pExtensions = CertFindExtension(szOID_SUBJECT_ALT_NAME2, m_cert->pCertInfo->cExtension, m_cert->pCertInfo->rgExtension);
	if (pExtensions)
	{
		CRYPT_DECODE_PARA decodePara = {0};
		decodePara.cbSize = sizeof(decodePara);

		CERT_ALT_NAME_INFO* pAltNameInfo = NULL;
		DWORD altNameInfoSize = 0;

		BOOL status = CryptDecodeObjectEx(
			X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
			szOID_SUBJECT_ALT_NAME2,
			pExtensions->Value.pbData,
			pExtensions->Value.cbData,
			CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
			&decodePara,
			&pAltNameInfo,
			&altNameInfoSize);

		if (status && pAltNameInfo) 
		{
			for (int i = 0; i < pAltNameInfo->cAltEntry; i++) 
			{
				if (pAltNameInfo->rgAltEntry[i].dwAltNameChoice == CERT_ALT_NAME_DNS_NAME)
					addDNSName( wstrToStr( pAltNameInfo->rgAltEntry[i].pwszDNSName ) );
			}
		}
	}

	// 	if (pExtensions)
	// 	{
	// 		vector<wchar_t> subjectAlt
	// 		CryptDecodeObject(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, szOID_SUBJECT_ALT_NAME, pExtensions->Value->pbData, pExtensions->Value->cbData, )
	// 	}
	// 
	// 	// subjectAltNames
	// 	int subjectAltNameLoc = X509_get_ext_by_NID(cert.get(), NID_subject_alt_name, -1);
	// 	if(subjectAltNameLoc != -1) {
	// 		X509_EXTENSION* extension = X509_get_ext(cert.get(), subjectAltNameLoc);
	// 		boost::shared_ptr<GENERAL_NAMES> generalNames(reinterpret_cast<GENERAL_NAMES*>(X509V3_EXT_d2i(extension)), GENERAL_NAMES_free);
	// 		boost::shared_ptr<ASN1_OBJECT> xmppAddrObject(OBJ_txt2obj(ID_ON_XMPPADDR_OID, 1), ASN1_OBJECT_free);
	// 		boost::shared_ptr<ASN1_OBJECT> dnsSRVObject(OBJ_txt2obj(ID_ON_DNSSRV_OID, 1), ASN1_OBJECT_free);
	// 		for (int i = 0; i < sk_GENERAL_NAME_num(generalNames.get()); ++i) {
	// 			GENERAL_NAME* generalName = sk_GENERAL_NAME_value(generalNames.get(), i);
	// 			if (generalName->type == GEN_OTHERNAME) {
	// 				OTHERNAME* otherName = generalName->d.otherName;
	// 				if (OBJ_cmp(otherName->type_id, xmppAddrObject.get()) == 0) {
	// 					// XmppAddr
	// 					if (otherName->value->type != V_ASN1_UTF8STRING) {
	// 						continue;
	// 					}
	// 					ASN1_UTF8STRING* xmppAddrValue = otherName->value->value.utf8string;
	// 					addXMPPAddress(ByteArray(ASN1_STRING_data(xmppAddrValue), ASN1_STRING_length(xmppAddrValue)).toString());
	// 				}
	// 				else if (OBJ_cmp(otherName->type_id, dnsSRVObject.get()) == 0) {
	// 					// SRVName
	// 					if (otherName->value->type != V_ASN1_IA5STRING) {
	// 						continue;
	// 					}
	// 					ASN1_IA5STRING* srvNameValue = otherName->value->value.ia5string;
	// 					addSRVName(ByteArray(ASN1_STRING_data(srvNameValue), ASN1_STRING_length(srvNameValue)).toString());
	// 				}
	// 			}
	// 			else if (generalName->type == GEN_DNS) {
	// 				// DNSName
	// 				addDNSName(ByteArray(ASN1_STRING_data(generalName->d.dNSName), ASN1_STRING_length(generalName->d.dNSName)).toString());
	// 			}
	// 		}
	// 	}
}

//------------------------------------------------------------------------

}
