/*
 * Copyright (c) 2011 Soren Dreijer
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#define SECURITY_WIN32
#include <Windows.h>
#include <Schannel.h>
#include <security.h>
#include <schnlsp.h>

#include <boost/noncopyable.hpp>

namespace Swift 
{
	//
	// Convenience wrapper around the Schannel CredHandle struct.
	//
	class ScopedCredHandle
	{
	private:
		struct HandleContext
		{
			HandleContext()
			{
				ZeroMemory(&m_h, sizeof(m_h));
			}

			HandleContext(const CredHandle& h)
			{
				memcpy(&m_h, &h, sizeof(m_h));
			}

			~HandleContext()
			{
				::FreeCredentialsHandle(&m_h);
			}

			CredHandle m_h;
		};

	public:
		ScopedCredHandle()
		: m_pHandle( new HandleContext )
		{
		}

		explicit ScopedCredHandle(const CredHandle& h)
		: m_pHandle( new HandleContext(h) )
		{
		}

		// Copy constructor
		explicit ScopedCredHandle(const ScopedCredHandle& rhs)
		{
			m_pHandle = rhs.m_pHandle;			
		}

		~ScopedCredHandle()
		{
			m_pHandle.reset();
		}

		PCredHandle Reset()
		{
			CloseHandle();
			return &m_pHandle->m_h;
		}

		operator PCredHandle() const
		{
			return &m_pHandle->m_h;
		}

		ScopedCredHandle& operator=(const ScopedCredHandle& sh)
		{
			// Only update the internal handle if it's different
			if (&m_pHandle->m_h != &sh.m_pHandle->m_h)
			{
				m_pHandle = sh.m_pHandle;
			}

			return *this;
		}

		void CloseHandle()
		{
			m_pHandle.reset( new HandleContext );
		}

	private:
		boost::shared_ptr<HandleContext> m_pHandle;
	};

	//------------------------------------------------------------------------

	//
	// Convenience wrapper around the Schannel CtxtHandle struct.
	//
	class ScopedCtxtHandle
	{
	private:
		struct HandleContext
		{
			HandleContext()
			{
				ZeroMemory(&m_h, sizeof(m_h));
			}

			~HandleContext()
			{
				::DeleteSecurityContext(&m_h);
			}

			CtxtHandle m_h;
		};

	public:
		ScopedCtxtHandle()
		: m_pHandle( new HandleContext )
		{
		}

		explicit ScopedCtxtHandle(CredHandle h)
		: m_pHandle( new HandleContext )
		{
		}

		// Copy constructor
		explicit ScopedCtxtHandle(const ScopedCtxtHandle& rhs)
		{
			m_pHandle = rhs.m_pHandle;			
		}

		~ScopedCtxtHandle()
		{
			m_pHandle.reset();
		}

		PCredHandle Reset()
		{
			CloseHandle();
			return &m_pHandle->m_h;
		}

		operator PCredHandle() const
		{
			return &m_pHandle->m_h;
		}

		ScopedCtxtHandle& operator=(const ScopedCtxtHandle& sh)
		{
			// Only update the internal handle if it's different
			if (&m_pHandle->m_h != &sh.m_pHandle->m_h)
			{
				m_pHandle = sh.m_pHandle;
			}

			return *this;
		}

		void CloseHandle()
		{
			m_pHandle.reset( new HandleContext );
		}

	private:
		boost::shared_ptr<HandleContext> m_pHandle;
	};

	//------------------------------------------------------------------------

	//
	// Convenience wrapper around the Schannel ScopedSecBuffer struct.
	//
	class ScopedSecBuffer : boost::noncopyable
	{
	public:
		ScopedSecBuffer(PSecBuffer pSecBuffer)
		: m_pSecBuffer(pSecBuffer)
		{
		}
		
		~ScopedSecBuffer()
		{
			// Loop through all the output buffers and make sure we free them
			if (m_pSecBuffer->pvBuffer)
				FreeContextBuffer(m_pSecBuffer->pvBuffer);
		}

		PSecBuffer AsPtr()
		{
			return m_pSecBuffer;
		}

		PSecBuffer operator->()
		{
			return m_pSecBuffer;
		}

	private:
		PSecBuffer m_pSecBuffer;
	};

	//------------------------------------------------------------------------

	//
	// Convenience wrapper around the Schannel PCCERT_CONTEXT.
	//
	class ScopedCertContext
	{
	private:
		struct HandleContext
		{
			HandleContext()
			: m_pCertCtxt(NULL)
			{
			}

			HandleContext(PCCERT_CONTEXT pCert)
			: m_pCertCtxt(pCert)
			{
			}

			~HandleContext()
			{
				if (m_pCertCtxt)
					CertFreeCertificateContext(m_pCertCtxt);
			}

			PCCERT_CONTEXT m_pCertCtxt;
		};

	public:
		ScopedCertContext()
		: m_pHandle( new HandleContext )
		{
		}

		explicit ScopedCertContext(PCCERT_CONTEXT pCert)
		: m_pHandle( new HandleContext(pCert) )
		{
		}

		// Copy constructor
		ScopedCertContext(const ScopedCertContext& rhs)
		{
			m_pHandle = rhs.m_pHandle;			
		}

		~ScopedCertContext()
		{
			m_pHandle.reset();
		}

		PCCERT_CONTEXT* Reset()
		{
			FreeContext();
			return &m_pHandle->m_pCertCtxt;
		}

		operator PCCERT_CONTEXT() const
		{
			return m_pHandle->m_pCertCtxt;
		}

		PCCERT_CONTEXT* GetPointer() const
		{
			return &m_pHandle->m_pCertCtxt;
		}

		PCCERT_CONTEXT operator->() const
		{
			return m_pHandle->m_pCertCtxt;
		}

		ScopedCertContext& operator=(const ScopedCertContext& sh)
		{
			// Only update the internal handle if it's different
			if (&m_pHandle->m_pCertCtxt != &sh.m_pHandle->m_pCertCtxt)
			{
				m_pHandle = sh.m_pHandle;
			}

			return *this;
		}

		ScopedCertContext& operator=(PCCERT_CONTEXT pCertCtxt)
		{
			// Only update the internal handle if it's different
			if (m_pHandle && m_pHandle->m_pCertCtxt != pCertCtxt)
				m_pHandle.reset( new HandleContext(pCertCtxt) );

			return *this;
		}

		void FreeContext()
		{
			m_pHandle.reset( new HandleContext );
		}

	private:
		boost::shared_ptr<HandleContext> m_pHandle;		
	};

	//------------------------------------------------------------------------

	//
	// Convenience wrapper around the Schannel HCERTSTORE.
	//
	class ScopedCertStore : boost::noncopyable
	{
	public:
		ScopedCertStore(HCERTSTORE hCertStore)
		: m_hCertStore(hCertStore)
		{
		}
		
		~ScopedCertStore()
		{
			// Forcefully free all memory related to the store, i.e. we assume all CertContext's that have been opened via this
			// cert store have been closed at this point.
			if (m_hCertStore)
				CertCloseStore(m_hCertStore, CERT_CLOSE_STORE_FORCE_FLAG);
		}

		operator HCERTSTORE() const
		{
			return m_hCertStore;
		}

	private:
		HCERTSTORE m_hCertStore;
	};

	//------------------------------------------------------------------------

	//
	// Convenience wrapper around the Schannel CERT_CHAIN_CONTEXT.
	//
	class ScopedCertChainContext
	{
	private:
		struct HandleContext
		{
			HandleContext()
			: m_pCertChainCtxt(NULL)
			{
			}

			HandleContext(PCCERT_CHAIN_CONTEXT pCert)
			: m_pCertChainCtxt(pCert)
			{
			}

			~HandleContext()
			{
				if (m_pCertChainCtxt)
					CertFreeCertificateChain(m_pCertChainCtxt);
			}

			PCCERT_CHAIN_CONTEXT m_pCertChainCtxt;
		};

	public:
		ScopedCertChainContext()
		: m_pHandle( new HandleContext )
		{
		}

		explicit ScopedCertChainContext(PCCERT_CHAIN_CONTEXT pCert)
		: m_pHandle( new HandleContext(pCert) )
		{
		}

		// Copy constructor
		ScopedCertChainContext(const ScopedCertChainContext& rhs)
		{
			m_pHandle = rhs.m_pHandle;			
		}

		~ScopedCertChainContext()
		{
			m_pHandle.reset();
		}

		PCCERT_CHAIN_CONTEXT* Reset()
		{
			FreeContext();
			return &m_pHandle->m_pCertChainCtxt;
		}

		operator PCCERT_CHAIN_CONTEXT() const
		{
			return m_pHandle->m_pCertChainCtxt;
		}

		PCCERT_CHAIN_CONTEXT operator->() const
		{
			return m_pHandle->m_pCertChainCtxt;
		}

		ScopedCertChainContext& operator=(const ScopedCertChainContext& sh)
		{
			// Only update the internal handle if it's different
			if (&m_pHandle->m_pCertChainCtxt != &sh.m_pHandle->m_pCertChainCtxt)
			{
				m_pHandle = sh.m_pHandle;
			}

			return *this;
		}

		void FreeContext()
		{
			m_pHandle.reset( new HandleContext );
		}

	private:
		boost::shared_ptr<HandleContext> m_pHandle;		
	};
}
