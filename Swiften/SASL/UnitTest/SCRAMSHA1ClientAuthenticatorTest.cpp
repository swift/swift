#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/SASL/SCRAMSHA1ClientAuthenticator.h"
#include "Swiften/Base/ByteArray.h"

using namespace Swift;

class SCRAMSHA1ClientAuthenticatorTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SCRAMSHA1ClientAuthenticatorTest);
		CPPUNIT_TEST(testGetInitialResponse);
		CPPUNIT_TEST(testGetInitialResponse_UsernameHasSpecialChars);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			nonce = String("abcdefghABCDEFGH");
		}

		void testGetInitialResponse() {
			SCRAMSHA1ClientAuthenticator testling(nonce);
			testling.setCredentials("user", "pass", "");

			ByteArray response = testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(String("n,,n=user,r=abcdefghABCDEFGH"), testling.getResponse().toString());
		}

		void testGetInitialResponse_UsernameHasSpecialChars() {
			SCRAMSHA1ClientAuthenticator testling(nonce);
			testling.setCredentials(",us=,er=", "pass", "");

			ByteArray response = testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(String("n,,n==2Cus=3D=2Cer=3D,r=abcdefghABCDEFGH"), testling.getResponse().toString());
		}

	private:
		String nonce;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SCRAMSHA1ClientAuthenticatorTest);
