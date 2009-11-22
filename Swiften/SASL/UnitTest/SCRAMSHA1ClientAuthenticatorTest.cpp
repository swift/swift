#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/SASL/SCRAMSHA1ClientAuthenticator.h"
#include "Swiften/Base/ByteArray.h"

using namespace Swift;

class SCRAMSHA1ClientAuthenticatorTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SCRAMSHA1ClientAuthenticatorTest);
		CPPUNIT_TEST(testGetInitialResponse);
		CPPUNIT_TEST(testGetInitialResponse_UsernameHasSpecialChars);
		CPPUNIT_TEST(testGetFinalResponse);
		CPPUNIT_TEST(testSetChallenge);
		CPPUNIT_TEST(testSetChallenge_InvalidClientNonce);
		CPPUNIT_TEST(testSetChallenge_OnlyClientNonce);
		CPPUNIT_TEST(testSetFinalChallenge);
		CPPUNIT_TEST(testSetFinalChallenge_InvalidChallenge);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
		}

		void testGetInitialResponse() {
			SCRAMSHA1ClientAuthenticator testling("abcdefghABCDEFGH");
			testling.setCredentials("user", "pass", "");

			ByteArray response = testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(String("n,,n=user,r=abcdefghABCDEFGH"), testling.getResponse().toString());
		}

		void testGetInitialResponse_UsernameHasSpecialChars() {
			SCRAMSHA1ClientAuthenticator testling("abcdefghABCDEFGH");
			testling.setCredentials(",us=,er=", "pass", "");

			ByteArray response = testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(String("n,,n==2Cus=3D=2Cer=3D,r=abcdefghABCDEFGH"), testling.getResponse().toString());
		}

		void testGetFinalResponse() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", "pass", "");
			testling.setChallenge(ByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=4096"));

			ByteArray response = testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(String("c=biwsCg==,r=abcdefghABCDEFGH,p=bVzb1EAf2hXw5Z+QIMYPTy5TOsU="), testling.getResponse().toString());
		}

		void testSetFinalChallenge() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", "pass", "");
			testling.setChallenge(ByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=4096"));
			bool result = testling.setChallenge(ByteArray("v=xWI/+vT8aAm0hKpqKIqctIDwtQE="));

			CPPUNIT_ASSERT(result);
		}

		void testSetChallenge() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", "pass", "");

			bool result = testling.setChallenge(ByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=4096"));

			CPPUNIT_ASSERT(result);
		}

		void testSetChallenge_InvalidClientNonce() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", "pass", "");

			bool result = testling.setChallenge(ByteArray("r=abcdefgiABCDEFGH,s=MTIzNDU2NzgK,i=4096"));

			CPPUNIT_ASSERT(!result);
		}

		void testSetChallenge_OnlyClientNonce() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", "pass", "");

			bool result = testling.setChallenge(ByteArray("r=abcdefgh,s=MTIzNDU2NzgK,i=4096"));

			CPPUNIT_ASSERT(!result);
		}

		void testSetFinalChallenge_InvalidChallenge() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", "pass", "");
			testling.setChallenge(ByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=4096"));
			bool result = testling.setChallenge(ByteArray("v=e26kI69ICb6zosapLLxrER/631A="));

			CPPUNIT_ASSERT(!result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SCRAMSHA1ClientAuthenticatorTest);
