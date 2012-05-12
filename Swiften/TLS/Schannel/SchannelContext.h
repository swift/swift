/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/TLS/TLSContext.h>
#include <Swiften/TLS/Schannel/SchannelUtil.h>
#include <Swiften/TLS/CertificateWithKey.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/TLS/TLSError.h>

#define SECURITY_WIN32
#include <Windows.h>
#include <Schannel.h>
#include <security.h>
#include <schnlsp.h>

#include <boost/noncopyable.hpp>

namespace Swift 
{	
	class CAPICertificate;
	class SchannelContext : public TLSContext, boost::noncopyable 
	{
	public:
		typedef boost::shared_ptr<SchannelContext> sp_t;

	public:
		SchannelContext();

		~SchannelContext();

		//
		// TLSContext
		//
		virtual void	connect();
		virtual bool	setClientCertificate(CertificateWithKey::ref cert);

		virtual void	handleDataFromNetwork(const SafeByteArray& data);
		virtual void	handleDataFromApplication(const SafeByteArray& data);

		virtual std::vector<Certificate::ref> getPeerCertificateChain() const;
		virtual CertificateVerificationError::ref getPeerCertificateVerificationError() const;

		virtual ByteArray getFinishMessage() const;

		virtual void setCheckCertificateRevocation(bool b);

	private:
		void			determineStreamSizes();
		void			continueHandshake(const SafeByteArray& data);
		void			indicateError(boost::shared_ptr<TLSError> error);
		//FIXME: Remove
		void indicateError() {indicateError(boost::make_shared<TLSError>());}
		void			handleCertError(SECURITY_STATUS status) ;

		void			sendDataOnNetwork(const void* pData, size_t dataSize);
		void			forwardDataToApplication(const void* pData, size_t dataSize);

		void			decryptAndProcessData(const SafeByteArray& data);
		void			encryptAndSendData(const SafeByteArray& data);

		void			appendNewData(const SafeByteArray& data);
		SECURITY_STATUS validateServerCertificate();

		void			handleCertificateCardRemoved();

	private:
		enum SchannelState
		{
			Start,
			Connecting,
			Connected,
			Error

		};

		SchannelState		m_state;
		boost::optional<CertificateVerificationError> m_verificationError;

		ULONG				m_secContext;
		ScopedCredHandle	m_credHandle;
		ScopedCtxtHandle	m_ctxtHandle;
		DWORD				m_ctxtFlags;
		SecPkgContext_StreamSizes m_streamSizes;

		std::vector<char>	m_receivedData;

		HCERTSTORE		m_my_cert_store;
		std::string		m_cert_store_name;
		std::string		m_cert_name;
////Not needed, most likely
		std::string		m_smartcard_reader;	//Can be empty string for non SmartCard certificates
		boost::shared_ptr<CAPICertificate> userCertificate;
		bool checkCertificateRevocation;
	};
}
