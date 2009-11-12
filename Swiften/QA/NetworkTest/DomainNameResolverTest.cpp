#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Base/String.h"
#include "Swiften/Network/PlatformDomainNameResolver.h"
#include "Swiften/Network/DomainNameResolveException.h"

using namespace Swift;

class DomainNameResolverTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(DomainNameResolverTest);
		CPPUNIT_TEST(testResolve_NoSRV);
		CPPUNIT_TEST(testResolve_SRV);
		CPPUNIT_TEST(testResolve_Invalid);
		//CPPUNIT_TEST(testResolve_IPv6);
		CPPUNIT_TEST(testResolve_International);
		CPPUNIT_TEST(testResolve_Localhost);
		CPPUNIT_TEST_SUITE_END();

	public:
		DomainNameResolverTest() {}

		void setUp() {
			resolver_ = new PlatformDomainNameResolver();
		}

		void tearDown() {
			delete resolver_;
		}

		void testResolve_NoSRV() {
			HostAddressPort result = resolver_->resolve("xmpp.test.swift.im")[0];

			CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.0"), result.getAddress().toString());
			CPPUNIT_ASSERT_EQUAL(5222, result.getPort());
		}

		void testResolve_SRV() {
			std::vector<HostAddressPort> result = resolver_->resolve("xmpp-srv.test.swift.im");

			CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(result.size()));
			CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.1"), result[0].getAddress().toString());
			CPPUNIT_ASSERT_EQUAL(5000, result[0].getPort());
			CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.3"), result[1].getAddress().toString());
			CPPUNIT_ASSERT_EQUAL(5000, result[1].getPort());
			CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.2"), result[2].getAddress().toString());
			CPPUNIT_ASSERT_EQUAL(5000, result[2].getPort());
		}

		void testResolve_Invalid() {
			CPPUNIT_ASSERT_THROW(resolver_->resolve("invalid.test.swift.im"), DomainNameResolveException);
		}

		void testResolve_IPv6() {
			HostAddressPort result = resolver_->resolve("xmpp-ipv6.test.swift.im")[0];
			CPPUNIT_ASSERT_EQUAL(std::string("0000:0000:0000:0000:0000:ffff:0a00:0104"), result.getAddress().toString());
			CPPUNIT_ASSERT_EQUAL(5222, result.getPort());
		}

		void testResolve_International() {
			HostAddressPort result = resolver_->resolve("tron\xc3\xa7on.test.swift.im")[0];
			CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.3"), result.getAddress().toString());
			CPPUNIT_ASSERT_EQUAL(5222, result.getPort());
		}

		void testResolve_Localhost() {
			HostAddressPort result = resolver_->resolve("localhost")[0];
			CPPUNIT_ASSERT_EQUAL(std::string("127.0.0.1"), result.getAddress().toString());
			CPPUNIT_ASSERT_EQUAL(5222, result.getPort());
		}

	private:
		PlatformDomainNameResolver* resolver_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(DomainNameResolverTest);
