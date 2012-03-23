/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "Swiften/TLS/Schannel/SchannelContext.h"
#include "Swiften/TLS/Schannel/SchannelCertificate.h"
#include <Swiften/TLS/CAPICertificate.h>
#include <WinHTTP.h> // For SECURITY_FLAG_IGNORE_CERT_CN_INVALID

namespace Swift {

//------------------------------------------------------------------------

SchannelContext::SchannelContext() 
: m_state(Start)
, m_secContext(0)
, m_my_cert_store(NULL)
, m_cert_store_name("MY")
, m_cert_name()
{
	m_ctxtFlags = ISC_REQ_ALLOCATE_MEMORY | 
				  ISC_REQ_CONFIDENTIALITY |
				  ISC_REQ_EXTENDED_ERROR  |
				  ISC_REQ_INTEGRITY		  |
				  ISC_REQ_REPLAY_DETECT   |	
				  ISC_REQ_SEQUENCE_DETECT |
				  ISC_REQ_USE_SUPPLIED_CREDS |
				  ISC_REQ_STREAM;

	ZeroMemory(&m_streamSizes, sizeof(m_streamSizes));
}

//------------------------------------------------------------------------

SchannelContext::~SchannelContext()
{
	if (m_my_cert_store) CertCloseStore(m_my_cert_store, 0);
}

//------------------------------------------------------------------------

void SchannelContext::determineStreamSizes()
{
	QueryContextAttributes(m_ctxtHandle, SECPKG_ATTR_STREAM_SIZES, &m_streamSizes);
}

//------------------------------------------------------------------------

void SchannelContext::connect() 
{
	ScopedCertContext pCertContext;

	m_state = Connecting;

	// If a user name is specified, then attempt to find a client
	// certificate. Otherwise, just create a NULL credential.
	if (!m_cert_name.empty())
	{
		if (m_my_cert_store == NULL)
		{
			m_my_cert_store = CertOpenSystemStore(0, m_cert_store_name.c_str());
			if (!m_my_cert_store)
			{
/////			printf( "**** Error 0x%x returned by CertOpenSystemStore\n", GetLastError() );
				indicateError();
				return;
			}
		}

		pCertContext = findCertificateInStore( m_my_cert_store, m_cert_name );
		if (pCertContext == NULL)
		{
/////		printf("**** Error 0x%x returned by CertFindCertificateInStore\n", GetLastError());
			indicateError();
			return;
		}
	}

	// We use an empty list for client certificates
	PCCERT_CONTEXT clientCerts[1] = {0};

	SCHANNEL_CRED sc = {0};
	sc.dwVersion = SCHANNEL_CRED_VERSION;

/////SSL3?
	sc.grbitEnabledProtocols = SP_PROT_SSL3_CLIENT | SP_PROT_TLS1_CLIENT | SP_PROT_TLS1_1_CLIENT | SP_PROT_TLS1_2_CLIENT;
	sc.dwFlags = SCH_CRED_MANUAL_CRED_VALIDATION;

	if (pCertContext)
	{
		sc.cCreds = 1;
		sc.paCred = pCertContext.GetPointer();
		sc.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS;
	}
	else
	{
		sc.cCreds = 0; // Let Crypto API find the appropriate certificate for us
		sc.paCred = clientCerts;
		sc.dwFlags |= SCH_CRED_USE_DEFAULT_CREDS;
	}

	// Swiften performs the server name check for us
	sc.dwFlags |= SCH_CRED_NO_SERVERNAME_CHECK;

	SECURITY_STATUS status = AcquireCredentialsHandle(
		NULL,
		UNISP_NAME,
		SECPKG_CRED_OUTBOUND,
		NULL,
		&sc,
		NULL,
		NULL,
		m_credHandle.Reset(),
		NULL);
	
	if (status != SEC_E_OK) 
	{
		// We failed to obtain the credentials handle
		indicateError();
		return;
	}

	SecBuffer outBuffers[2];

	// We let Schannel allocate the output buffer for us
	outBuffers[0].pvBuffer   = NULL;
	outBuffers[0].cbBuffer   = 0;
	outBuffers[0].BufferType = SECBUFFER_TOKEN;

	// Contains alert data if an alert is generated
	outBuffers[1].pvBuffer   = NULL;
	outBuffers[1].cbBuffer   = 0;
	outBuffers[1].BufferType = SECBUFFER_ALERT;

	// Make sure the output buffers are freed
	ScopedSecBuffer scopedOutputData(&outBuffers[0]);
	ScopedSecBuffer scopedOutputAlertData(&outBuffers[1]);

	SecBufferDesc outBufferDesc = {0};
	outBufferDesc.cBuffers   = 2;
	outBufferDesc.pBuffers   = outBuffers;
	outBufferDesc.ulVersion  = SECBUFFER_VERSION;

	// Create the initial security context
	status = InitializeSecurityContext(
		m_credHandle,
		NULL,
		NULL,
		m_ctxtFlags,
		0,
		0,
		NULL,
		0,
		m_ctxtHandle.Reset(),
		&outBufferDesc,
		&m_secContext,
		NULL);

	if (status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED) 
	{
		// We failed to initialize the security context
		handleCertError(status);
		indicateError();
		return;
	}

	// Start the handshake
	sendDataOnNetwork(outBuffers[0].pvBuffer, outBuffers[0].cbBuffer);

	if (status == SEC_E_OK) 
	{
		status = validateServerCertificate();
		if (status != SEC_E_OK)
			handleCertError(status);

		m_state = Connected;
		determineStreamSizes();

		onConnected();
	}
}

//------------------------------------------------------------------------

SECURITY_STATUS SchannelContext::validateServerCertificate()
{
	SchannelCertificate::ref pServerCert = boost::dynamic_pointer_cast<SchannelCertificate>( getPeerCertificate() );
	if (!pServerCert)
		return SEC_E_WRONG_PRINCIPAL;

	const LPSTR usage[] = 
	{
		szOID_PKIX_KP_SERVER_AUTH,
		szOID_SERVER_GATED_CRYPTO,
		szOID_SGC_NETSCAPE
	};

	CERT_CHAIN_PARA chainParams = {0};
	chainParams.cbSize = sizeof(chainParams);
	chainParams.RequestedUsage.dwType = USAGE_MATCH_TYPE_OR;
	chainParams.RequestedUsage.Usage.cUsageIdentifier = ARRAYSIZE(usage);
	chainParams.RequestedUsage.Usage.rgpszUsageIdentifier = const_cast<LPSTR*>(usage);

	DWORD chainFlags = CERT_CHAIN_CACHE_END_CERT | CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT;

	ScopedCertChainContext pChainContext;

	BOOL success = CertGetCertificateChain(
		NULL, // Use the chain engine for the current user (assumes a user is logged in)
		pServerCert->getCertContext(),
		NULL,
		NULL,
		&chainParams,
		chainFlags,
		NULL,
		pChainContext.Reset());

	if (!success)
		return GetLastError();

	SSL_EXTRA_CERT_CHAIN_POLICY_PARA sslChainPolicy = {0};
	sslChainPolicy.cbSize = sizeof(sslChainPolicy);
	sslChainPolicy.dwAuthType = AUTHTYPE_SERVER;
	sslChainPolicy.fdwChecks = SECURITY_FLAG_IGNORE_CERT_CN_INVALID; // Swiften checks the server name for us. Is this the correct way to disable server name checking?
	sslChainPolicy.pwszServerName = NULL;

	CERT_CHAIN_POLICY_PARA certChainPolicy = {0};
	certChainPolicy.cbSize = sizeof(certChainPolicy);
	certChainPolicy.dwFlags = CERT_CHAIN_POLICY_IGNORE_INVALID_NAME_FLAG; // Swiften checks the server name for us. Is this the correct way to disable server name checking?
	certChainPolicy.pvExtraPolicyPara = &sslChainPolicy;

	CERT_CHAIN_POLICY_STATUS certChainPolicyStatus = {0};
	certChainPolicyStatus.cbSize = sizeof(certChainPolicyStatus);

	// Verify the chain
	if (!CertVerifyCertificateChainPolicy(
		CERT_CHAIN_POLICY_SSL,
		pChainContext,
		&certChainPolicy,
		&certChainPolicyStatus))
	{
		return GetLastError();
	}

	if (certChainPolicyStatus.dwError != S_OK)
		return certChainPolicyStatus.dwError;

	return S_OK;
}

//------------------------------------------------------------------------

void SchannelContext::appendNewData(const SafeByteArray& data)
{
	size_t originalSize = m_receivedData.size();
	m_receivedData.resize( originalSize + data.size() );
	memcpy( &m_receivedData[0] + originalSize, &data[0], data.size() );
}

//------------------------------------------------------------------------

void SchannelContext::continueHandshake(const SafeByteArray& data) 
{
	appendNewData(data);

	while (!m_receivedData.empty())
	{
		SecBuffer inBuffers[2];

		// Provide Schannel with the remote host's handshake data
		inBuffers[0].pvBuffer	 = (char*)(&m_receivedData[0]);
		inBuffers[0].cbBuffer	 = (unsigned long)m_receivedData.size();
		inBuffers[0].BufferType  = SECBUFFER_TOKEN;

		inBuffers[1].pvBuffer   = NULL;
		inBuffers[1].cbBuffer   = 0;
		inBuffers[1].BufferType = SECBUFFER_EMPTY;

		SecBufferDesc inBufferDesc = {0};
		inBufferDesc.cBuffers   = 2;
		inBufferDesc.pBuffers   = inBuffers;
		inBufferDesc.ulVersion  = SECBUFFER_VERSION;

		SecBuffer outBuffers[2];

		// We let Schannel allocate the output buffer for us
		outBuffers[0].pvBuffer   = NULL;
		outBuffers[0].cbBuffer   = 0;
		outBuffers[0].BufferType = SECBUFFER_TOKEN;

		// Contains alert data if an alert is generated
		outBuffers[1].pvBuffer   = NULL;
		outBuffers[1].cbBuffer   = 0;
		outBuffers[1].BufferType = SECBUFFER_ALERT;

		// Make sure the output buffers are freed
		ScopedSecBuffer scopedOutputData(&outBuffers[0]);
		ScopedSecBuffer scopedOutputAlertData(&outBuffers[1]);

		SecBufferDesc outBufferDesc = {0};
		outBufferDesc.cBuffers   = 2;
		outBufferDesc.pBuffers   = outBuffers;
		outBufferDesc.ulVersion  = SECBUFFER_VERSION;

		SECURITY_STATUS status = InitializeSecurityContext(
			m_credHandle,
			m_ctxtHandle,
			NULL,
			m_ctxtFlags,
			0,
			0,
			&inBufferDesc,
			0,
			NULL,
			&outBufferDesc,
			&m_secContext,
			NULL);

		if (status == SEC_E_INCOMPLETE_MESSAGE)	
		{
			// Wait for more data to arrive
			break;
		}
		else if (status == SEC_I_CONTINUE_NEEDED) 
		{
			SecBuffer* pDataBuffer = &outBuffers[0];
			SecBuffer* pExtraBuffer = &inBuffers[1];
			
			if (pDataBuffer && pDataBuffer->cbBuffer > 0 && pDataBuffer->pvBuffer != NULL)
				sendDataOnNetwork(pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);

			if (pExtraBuffer->BufferType == SECBUFFER_EXTRA)
				m_receivedData.erase(m_receivedData.begin(), m_receivedData.end() - pExtraBuffer->cbBuffer);
			else
				m_receivedData.clear();

			break;
		}
		else if (status == SEC_E_OK) 
		{
			status = validateServerCertificate();
			if (status != SEC_E_OK)
				handleCertError(status);

			SecBuffer* pExtraBuffer = &inBuffers[1];
			
			if (pExtraBuffer && pExtraBuffer->cbBuffer > 0)
				m_receivedData.erase(m_receivedData.begin(), m_receivedData.end() - pExtraBuffer->cbBuffer);
			else
				m_receivedData.clear();

			m_state = Connected;
			determineStreamSizes();

			onConnected();
		} 
		else 
		{
			// We failed to initialize the security context
			handleCertError(status);
			indicateError();
			return;
		}
	}
}

//------------------------------------------------------------------------

void SchannelContext::handleCertError(SECURITY_STATUS status) 
{
	if (status == SEC_E_UNTRUSTED_ROOT			|| 
		status == CERT_E_UNTRUSTEDROOT			||
		status == CRYPT_E_ISSUER_SERIALNUMBER	|| 
		status == CRYPT_E_SIGNER_NOT_FOUND		||
		status == CRYPT_E_NO_TRUSTED_SIGNER)
	{
		m_verificationError = CertificateVerificationError::Untrusted;
	}
	else if (status == SEC_E_CERT_EXPIRED || 
			 status == CERT_E_EXPIRED)
	{
		m_verificationError = CertificateVerificationError::Expired;
	}
	else if (status == CRYPT_E_SELF_SIGNED)
	{
		m_verificationError = CertificateVerificationError::SelfSigned;
	}
	else if (status == CRYPT_E_HASH_VALUE	||
			 status == TRUST_E_CERT_SIGNATURE)
	{
		m_verificationError = CertificateVerificationError::InvalidSignature;
	}
	else if (status == CRYPT_E_REVOKED)
	{
		m_verificationError = CertificateVerificationError::Revoked;
	}
	else if (status == CRYPT_E_NO_REVOCATION_CHECK ||
			 status == CRYPT_E_REVOCATION_OFFLINE)
	{
		m_verificationError = CertificateVerificationError::RevocationCheckFailed;
	}
	else
	{
		m_verificationError = CertificateVerificationError::UnknownError;
	}
}

//------------------------------------------------------------------------

void SchannelContext::sendDataOnNetwork(const void* pData, size_t dataSize) 
{
	if (dataSize > 0 && pData) 
	{
		SafeByteArray byteArray(dataSize);
		memcpy(&byteArray[0], pData, dataSize);

		onDataForNetwork(byteArray);
	}
}

//------------------------------------------------------------------------

void SchannelContext::forwardDataToApplication(const void* pData, size_t dataSize) 
{
	SafeByteArray byteArray(dataSize);
	memcpy(&byteArray[0], pData, dataSize);

	onDataForApplication(byteArray);
}

//------------------------------------------------------------------------

void SchannelContext::handleDataFromApplication(const SafeByteArray& data) 
{
	// Don't attempt to send data until we're fully connected
	if (m_state == Connecting)
		return;

	// Encrypt the data
	encryptAndSendData(data);
}

//------------------------------------------------------------------------

void SchannelContext::handleDataFromNetwork(const SafeByteArray& data) 
{
	switch (m_state)
	{
	case Connecting:
		{
			// We're still establishing the connection, so continue the handshake
			continueHandshake(data);
		}
		break;

	case Connected:
		{
			// Decrypt the data
			decryptAndProcessData(data);
		}
		break;

	default:
		return;
	}
}

//------------------------------------------------------------------------

void SchannelContext::indicateError() 
{
	m_state = Error;
	m_receivedData.clear();
	onError(boost::make_shared<TLSError>());
}

//------------------------------------------------------------------------

void SchannelContext::decryptAndProcessData(const SafeByteArray& data) 
{
	SecBuffer inBuffers[4]	= {0};

	appendNewData(data);
	
	while (!m_receivedData.empty())
	{
		//
		// MSDN: 
		//   When using the Schannel SSP with contexts that are not connection oriented, on input, 
		//   the structure must contain four SecBuffer structures. Exactly one buffer must be of type 
		//   SECBUFFER_DATA and contain an encrypted message, which is decrypted in place. The remaining 
		//   buffers are used for output and must be of type SECBUFFER_EMPTY. For connection-oriented 
		//   contexts, a SECBUFFER_DATA type buffer must be supplied, as noted for nonconnection-oriented 
		//   contexts. Additionally, a second SECBUFFER_TOKEN type buffer that contains a security token 
		//   must also be supplied.
		//
		inBuffers[0].pvBuffer	 = (char*)(&m_receivedData[0]);
		inBuffers[0].cbBuffer	 = (unsigned long)m_receivedData.size();
		inBuffers[0].BufferType  = SECBUFFER_DATA;

		inBuffers[1].BufferType  = SECBUFFER_EMPTY;
		inBuffers[2].BufferType  = SECBUFFER_EMPTY;
		inBuffers[3].BufferType  = SECBUFFER_EMPTY;

		SecBufferDesc inBufferDesc = {0};
		inBufferDesc.cBuffers      = 4;
		inBufferDesc.pBuffers      = inBuffers;
		inBufferDesc.ulVersion     = SECBUFFER_VERSION;

		size_t inData = m_receivedData.size();
		SECURITY_STATUS status = DecryptMessage(m_ctxtHandle, &inBufferDesc, 0, NULL);

		if (status == SEC_E_INCOMPLETE_MESSAGE) 
		{
			// Wait for more data to arrive
			break;
		} 
		else if (status == SEC_I_RENEGOTIATE) 
		{
			// TODO: Handle renegotiation scenarios
			indicateError();
			break;
		} 
		else if (status == SEC_I_CONTEXT_EXPIRED) 
		{
			indicateError();
			break;
		} 
		else if (status != SEC_E_OK) 
		{
			indicateError();
			break;
		}

		SecBuffer* pDataBuffer = NULL;
		SecBuffer* pExtraBuffer = NULL;
		for (int i = 0; i < 4; ++i) 
		{
			if (inBuffers[i].BufferType == SECBUFFER_DATA)
				pDataBuffer = &inBuffers[i];

			else if (inBuffers[i].BufferType == SECBUFFER_EXTRA)
				pExtraBuffer = &inBuffers[i];
		}

		if (pDataBuffer && pDataBuffer->cbBuffer > 0 && pDataBuffer->pvBuffer != NULL)
			forwardDataToApplication(pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);

		// If there is extra data left over from the decryption operation, we call DecryptMessage() again
		if (pExtraBuffer) 
		{
			m_receivedData.erase(m_receivedData.begin(), m_receivedData.end() - pExtraBuffer->cbBuffer);
		} 
		else 
		{
			// We're done
			m_receivedData.erase(m_receivedData.begin(), m_receivedData.begin() + inData);
		}
	}
}

//------------------------------------------------------------------------

void SchannelContext::encryptAndSendData(const SafeByteArray& data) 
{	
	if (m_streamSizes.cbMaximumMessage == 0)
		return;

	SecBuffer outBuffers[4]	= {0};

	// Calculate the largest required size of the send buffer
	size_t messageBufferSize = (data.size() > m_streamSizes.cbMaximumMessage) 
							 ? m_streamSizes.cbMaximumMessage 
							 : data.size();

	// Allocate a packet for the encrypted data
	SafeByteArray sendBuffer;
	sendBuffer.resize(m_streamSizes.cbHeader + messageBufferSize + m_streamSizes.cbTrailer);

	size_t bytesSent = 0;
	do 
	{
		size_t bytesLeftToSend = data.size() - bytesSent;

		// Calculate how much of the send buffer we'll be using for this chunk
		size_t bytesToSend = (bytesLeftToSend > m_streamSizes.cbMaximumMessage) 
						   ? m_streamSizes.cbMaximumMessage 
						   : bytesLeftToSend;
		
		// Copy the plain text data into the send buffer
		memcpy(&sendBuffer[0] + m_streamSizes.cbHeader, &data[0] + bytesSent, bytesToSend);

		outBuffers[0].pvBuffer	 = &sendBuffer[0];
		outBuffers[0].cbBuffer	 = m_streamSizes.cbHeader;
		outBuffers[0].BufferType = SECBUFFER_STREAM_HEADER;

		outBuffers[1].pvBuffer	 = &sendBuffer[0] + m_streamSizes.cbHeader;
		outBuffers[1].cbBuffer	 = (unsigned long)bytesToSend;
		outBuffers[1].BufferType = SECBUFFER_DATA;

		outBuffers[2].pvBuffer	 = &sendBuffer[0] + m_streamSizes.cbHeader + bytesToSend;
		outBuffers[2].cbBuffer	 = m_streamSizes.cbTrailer;
		outBuffers[2].BufferType = SECBUFFER_STREAM_TRAILER;

		outBuffers[3].pvBuffer   = 0;
		outBuffers[3].cbBuffer   = 0;
		outBuffers[3].BufferType = SECBUFFER_EMPTY;

		SecBufferDesc outBufferDesc = {0};
		outBufferDesc.cBuffers   = 4;
		outBufferDesc.pBuffers   = outBuffers;
		outBufferDesc.ulVersion  = SECBUFFER_VERSION;

		SECURITY_STATUS status = EncryptMessage(m_ctxtHandle, 0, &outBufferDesc, 0);
		if (status != SEC_E_OK) 
		{
			indicateError();
			return;
		}

		sendDataOnNetwork(&sendBuffer[0], outBuffers[0].cbBuffer + outBuffers[1].cbBuffer + outBuffers[2].cbBuffer);
		bytesSent += bytesToSend;

	} while (bytesSent < data.size());
}

//------------------------------------------------------------------------

bool SchannelContext::setClientCertificate(CertificateWithKey::ref certificate)
{
	boost::shared_ptr<CAPICertificate> capiCertificate = boost::dynamic_pointer_cast<CAPICertificate>(certificate);
	if (!capiCertificate || capiCertificate->isNull()) {
		return false;
	}

	// We assume that the Certificate Store Name/Certificate Name
	// are valid at this point
	m_cert_store_name = capiCertificate->getCertStoreName();
	m_cert_name = capiCertificate->getCertName();
	return true;
}

//------------------------------------------------------------------------

Certificate::ref SchannelContext::getPeerCertificate() const 
{
	SchannelCertificate::ref pCertificate;

	ScopedCertContext pServerCert;
	SECURITY_STATUS status = QueryContextAttributes(m_ctxtHandle, SECPKG_ATTR_REMOTE_CERT_CONTEXT, pServerCert.Reset());
	if (status != SEC_E_OK)
		return pCertificate;

	pCertificate.reset( new SchannelCertificate(pServerCert) );
	return pCertificate;
}

//------------------------------------------------------------------------

CertificateVerificationError::ref SchannelContext::getPeerCertificateVerificationError() const 
{
	boost::shared_ptr<CertificateVerificationError> pCertError;

	if (m_verificationError)
		pCertError.reset( new CertificateVerificationError(*m_verificationError) );
	
	return pCertError;
}

//------------------------------------------------------------------------

ByteArray SchannelContext::getFinishMessage() const
{
	// TODO: Implement

	ByteArray emptyArray;
	return emptyArray;
}

//------------------------------------------------------------------------

}
