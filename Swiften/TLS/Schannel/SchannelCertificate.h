/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/TLS/Certificate.h"
#include "Swiften/TLS/Schannel/SchannelUtil.h"

namespace Swift 
{
	class SchannelCertificate : public Certificate 
	{
	public:
		typedef boost::shared_ptr<SchannelCertificate> ref;

	public:
		SchannelCertificate(const ScopedCertContext& certCtxt);
		SchannelCertificate(const ByteArray& der);

		std::string getSubjectName() const 
		{
			return m_subjectName;
		}

		std::vector<std::string> getCommonNames() const 
		{
			return m_commonNames;
		}

		std::vector<std::string> getSRVNames() const 
		{
			return m_srvNames;
		}

		std::vector<std::string> getDNSNames() const 
		{
			return m_dnsNames;
		}

		std::vector<std::string> getXMPPAddresses() const 
		{
			return m_xmppAddresses;
		}

		ScopedCertContext getCertContext() const
		{
			return m_cert;
		}

		ByteArray toDER() const;
		
	private:
		void parse();
		std::string wstrToStr(const std::wstring& wstr);

		void addSRVName(const std::string& name) 
		{
			m_srvNames.push_back(name);
		}

		void addDNSName(const std::string& name) 
		{
			m_dnsNames.push_back(name);
		}

		void addXMPPAddress(const std::string& addr) 
		{
			m_xmppAddresses.push_back(addr);
		}

	private:
		ScopedCertContext		 m_cert;

		std::string				 m_subjectName;
		std::vector<std::string> m_commonNames;
		std::vector<std::string> m_dnsNames;
		std::vector<std::string> m_xmppAddresses;
		std::vector<std::string> m_srvNames;
	};
}
