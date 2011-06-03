/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/SASL/SCRAMSHA1ClientAuthenticator.h>
#include <Swiften/Base/ByteArray.h>

using namespace Swift;

class SCRAMSHA1ClientAuthenticatorTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SCRAMSHA1ClientAuthenticatorTest);
		CPPUNIT_TEST(testGetInitialResponse);
		CPPUNIT_TEST(testGetInitialResponse_UsernameHasSpecialChars);
		CPPUNIT_TEST(testGetInitialResponse_WithAuthorizationID);
		CPPUNIT_TEST(testGetInitialResponse_WithAuthorizationIDWithSpecialChars);
		CPPUNIT_TEST(testGetInitialResponse_WithoutChannelBindingWithTLSChannelBindingData);
		CPPUNIT_TEST(testGetInitialResponse_WithChannelBindingWithTLSChannelBindingData);
		CPPUNIT_TEST(testGetFinalResponse);
		CPPUNIT_TEST(testGetFinalResponse_WithoutChannelBindingWithTLSChannelBindingData);
		CPPUNIT_TEST(testGetFinalResponse_WithChannelBindingWithTLSChannelBindingData);
		CPPUNIT_TEST(testSetChallenge);
		CPPUNIT_TEST(testSetChallenge_InvalidClientNonce);
		CPPUNIT_TEST(testSetChallenge_OnlyClientNonce);
		CPPUNIT_TEST(testSetChallenge_InvalidIterations);
		CPPUNIT_TEST(testSetChallenge_ZeroIterations);
		CPPUNIT_TEST(testSetChallenge_NegativeIterations);
		CPPUNIT_TEST(testSetChallenge_MissingIterations);
		CPPUNIT_TEST(testSetFinalChallenge);
		CPPUNIT_TEST(testSetFinalChallenge_InvalidChallenge);
		CPPUNIT_TEST(testGetResponseAfterFinalChallenge);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
		}

		void testGetInitialResponse() {
			SCRAMSHA1ClientAuthenticator testling("abcdefghABCDEFGH");
			testling.setCredentials("user", createSafeByteArray("pass"), "");

			SafeByteArray response = *testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray("n,,n=user,r=abcdefghABCDEFGH"), response);
		}

		void testGetInitialResponse_UsernameHasSpecialChars() {
			SCRAMSHA1ClientAuthenticator testling("abcdefghABCDEFGH");
			testling.setCredentials(",us=,er=", createSafeByteArray("pass"), "");

			SafeByteArray response = *testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray("n,,n==2Cus=3D=2Cer=3D,r=abcdefghABCDEFGH"), response);
		}

		void testGetInitialResponse_WithAuthorizationID() {
			SCRAMSHA1ClientAuthenticator testling("abcdefghABCDEFGH");
			testling.setCredentials("user", createSafeByteArray("pass"), "auth");

			SafeByteArray response = *testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray("n,a=auth,n=user,r=abcdefghABCDEFGH"), response);
		}

		void testGetInitialResponse_WithAuthorizationIDWithSpecialChars() {
			SCRAMSHA1ClientAuthenticator testling("abcdefghABCDEFGH");
			testling.setCredentials("user", createSafeByteArray("pass"), "a=u,th");

			SafeByteArray response = *testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray("n,a=a=3Du=2Cth,n=user,r=abcdefghABCDEFGH"), response);
		}

		void testGetInitialResponse_WithoutChannelBindingWithTLSChannelBindingData() {
			SCRAMSHA1ClientAuthenticator testling("abcdefghABCDEFGH", false);
			testling.setTLSChannelBindingData(createByteArray("xyza"));
			testling.setCredentials("user", createSafeByteArray("pass"), "");

			SafeByteArray response = *testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray("y,,n=user,r=abcdefghABCDEFGH"), response);
		}

		void testGetInitialResponse_WithChannelBindingWithTLSChannelBindingData() {
			SCRAMSHA1ClientAuthenticator testling("abcdefghABCDEFGH", true);
			testling.setTLSChannelBindingData(createByteArray("xyza"));
			testling.setCredentials("user", createSafeByteArray("pass"), "");

			SafeByteArray response = *testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray("p=tls-unique,,n=user,r=abcdefghABCDEFGH"), response);
		}

		void testGetFinalResponse() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", createSafeByteArray("pass"), "");
			testling.setChallenge(createByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=4096"));

			SafeByteArray response = *testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray("c=biws,r=abcdefghABCDEFGH,p=CZbjGDpIteIJwQNBgO0P8pKkMGY="), response);
		}

		void testGetFinalResponse_WithoutChannelBindingWithTLSChannelBindingData() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh", false);
			testling.setCredentials("user", createSafeByteArray("pass"), "");
			testling.setTLSChannelBindingData(createByteArray("xyza"));
			testling.setChallenge(createByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=4096"));

			SafeByteArray response = *testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray("c=eSws,r=abcdefghABCDEFGH,p=JNpsiFEcxZvNZ1+FFBBqrYvYxMk="), response);
		}

		void testGetFinalResponse_WithChannelBindingWithTLSChannelBindingData() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh", true);
			testling.setCredentials("user", createSafeByteArray("pass"), "");
			testling.setTLSChannelBindingData(createByteArray("xyza"));
			testling.setChallenge(createByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=4096"));

			SafeByteArray response = *testling.getResponse();

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray("c=cD10bHMtdW5pcXVlLCx4eXph,r=abcdefghABCDEFGH,p=i6Rghite81P1ype8XxaVAa5l7v0="), response);
		}

		void testSetFinalChallenge() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", createSafeByteArray("pass"), "");
			testling.setChallenge(createByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=4096"));

			bool result = testling.setChallenge(createByteArray("v=Dd+Q20knZs9jeeK0pi1Mx1Se+yo="));

			CPPUNIT_ASSERT(result);
		}

		void testSetChallenge() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", createSafeByteArray("pass"), "");

			bool result = testling.setChallenge(createByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=4096"));

			CPPUNIT_ASSERT(result);
		}

		void testSetChallenge_InvalidClientNonce() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", createSafeByteArray("pass"), "");

			bool result = testling.setChallenge(createByteArray("r=abcdefgiABCDEFGH,s=MTIzNDU2NzgK,i=4096"));

			CPPUNIT_ASSERT(!result);
		}

		void testSetChallenge_OnlyClientNonce() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", createSafeByteArray("pass"), "");

			bool result = testling.setChallenge(createByteArray("r=abcdefgh,s=MTIzNDU2NzgK,i=4096"));

			CPPUNIT_ASSERT(!result);
		}

		void testSetChallenge_InvalidIterations() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", createSafeByteArray("pass"), "");

			bool result = testling.setChallenge(createByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=bla"));

			CPPUNIT_ASSERT(!result);
		}

		void testSetChallenge_MissingIterations() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", createSafeByteArray("pass"), "");

			bool result = testling.setChallenge(createByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK"));

			CPPUNIT_ASSERT(!result);
		}

		void testSetChallenge_ZeroIterations() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", createSafeByteArray("pass"), "");

			bool result = testling.setChallenge(createByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=0"));

			CPPUNIT_ASSERT(!result);
		}

		void testSetChallenge_NegativeIterations() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", createSafeByteArray("pass"), "");

			bool result = testling.setChallenge(createByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=-1"));

			CPPUNIT_ASSERT(!result);
		}

		void testSetFinalChallenge_InvalidChallenge() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", createSafeByteArray("pass"), "");
			testling.setChallenge(createByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=4096"));
			bool result = testling.setChallenge(createByteArray("v=e26kI69ICb6zosapLLxrER/631A="));

			CPPUNIT_ASSERT(!result);
		}

		void testGetResponseAfterFinalChallenge() {
			SCRAMSHA1ClientAuthenticator testling("abcdefgh");
			testling.setCredentials("user", createSafeByteArray("pass"), "");
			testling.setChallenge(createByteArray("r=abcdefghABCDEFGH,s=MTIzNDU2NzgK,i=4096"));
			testling.setChallenge(createByteArray("v=Dd+Q20knZs9jeeK0pi1Mx1Se+yo="));

			CPPUNIT_ASSERT(!testling.getResponse());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SCRAMSHA1ClientAuthenticatorTest);
