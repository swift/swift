/*
 * Copyright (c) 2012 Isode Limited, London, England.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/TLS/CertificateWithKey.h>

#include <boost/algorithm/string/predicate.hpp>

#define SECURITY_WIN32
#include <WinCrypt.h>

namespace Swift {
	class CAPICertificate : public Swift::CertificateWithKey {
		public:
			CAPICertificate(const std::string& capiUri)
			    : valid_(false), uri_(capiUri), cert_store_handle_(0), cert_store_(NULL), cert_name_(NULL) {
				setUri(capiUri);
			}

			virtual ~CAPICertificate() {
				if (cert_store_handle_ != NULL)
				{
					CertCloseStore(cert_store_handle_, 0);
				}
			}

			virtual bool isNull() const {
				return uri_.empty() || !valid_;
			}

			virtual bool isPrivateKeyExportable() const {
				/* We can check with CAPI, but for now the answer is "no" */
				return false;
			}

			virtual const std::string& getCertStoreName() const {
			    return cert_store_;
			}

			virtual const std::string& getCertName() const {
			    return cert_name_;
			}

			const ByteArray& getData() const {
////Might need to throw an exception here, or really generate PKCS12 blob from CAPI data?
				assert(0);
			}

			void setData(const ByteArray& data) {
				assert(0);
			}

			const SafeByteArray& getPassword() const {
/////Can't pass NULL to createSafeByteArray!
/////Should this throw an exception instead?
				return createSafeByteArray("");
			}

		protected:
			void setUri (const std::string& capiUri) {

				valid_ = false;

				/* Syntax: "certstore:" [<cert_store> ":"] <cert_id> */

				if (!boost::iequals(capiUri.substr(0, 10), "certstore:")) {
					return;
				}

				/* Substring of subject: uses "storename" */
				std::string capi_identity = capiUri.substr(10);
				std::string new_cert_store_name;
				size_t pos = capi_identity.find_first_of (':');

				if (pos == std::string::npos) {
					/* Using the default certificate store */
					new_cert_store_name = "MY";
					cert_name_ = capi_identity;
				} else {
					new_cert_store_name = capi_identity.substr(0, pos);
					cert_name_ = capi_identity.substr(pos + 1);
				}

				PCCERT_CONTEXT pCertContext = NULL;

				if (cert_store_handle_ != NULL)
				{
					if (new_cert_store_name != cert_store_) {
						CertCloseStore(cert_store_handle_, 0);
						cert_store_handle_ = NULL;
					}
				}

				if (cert_store_handle_ == NULL)
				{
					cert_store_handle_ = CertOpenSystemStore(0, cert_store_.c_str());
					if (!cert_store_handle_)
					{
						return;
					}
				}

				cert_store_ = new_cert_store_name;

				/* NB: This might have to change, depending on how we locate certificates */

				// Find client certificate. Note that this sample just searches for a 
				// certificate that contains the user name somewhere in the subject name.
				pCertContext = CertFindCertificateInStore(cert_store_handle_,
					X509_ASN_ENCODING,
					0,				// dwFindFlags
					CERT_FIND_SUBJECT_STR_A,
					cert_name_.c_str(),		// *pvFindPara
					NULL );				// pPrevCertContext

				if (pCertContext == NULL)
				{
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
								       &len))
				{
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
								       &len))
				{
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
							  0))
				{
					free(pinfo);
					return;
				}

				if (!CryptGetUserKey(hprov, pinfo->dwKeySpec, &key))
				{
					CryptReleaseContext(hprov, 0);
					free(pinfo);
					return;
				}

				CryptDestroyKey(key);
				CryptReleaseContext(hprov, 0);
				free(pinfo);

				valid_ = true;
			}

		private:
			bool valid_;
			std::string uri_;

			HCERTSTORE cert_store_handle_;

			/* Parsed components of the uri_ */
			std::string cert_store_;
			std::string cert_name_;
	};
}
