/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Elements/AuthChallenge.h>
#include <Swiften/Serializer/AuthChallengeSerializer.h>

using namespace Swift;

class AuthChallengeSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(AuthChallengeSerializerTest);
        CPPUNIT_TEST(testSerialize);
        CPPUNIT_TEST(testSerialize_NoMessage);
        CPPUNIT_TEST(testSerialize_EmptyMessage);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testSerialize() {
            AuthChallengeSerializer testling;
            std::shared_ptr<AuthChallenge> authChallenge(new AuthChallenge());
            authChallenge->setValue(createByteArray("foo"));

            CPPUNIT_ASSERT_EQUAL(createSafeByteArray(
                "<challenge xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
                    "Zm9v"
                "</challenge>"), testling.serialize(authChallenge));
        }

        void testSerialize_NoMessage() {
            AuthChallengeSerializer testling;
            std::shared_ptr<AuthChallenge> authChallenge(new AuthChallenge());

            CPPUNIT_ASSERT_EQUAL(createSafeByteArray(
                "<challenge xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
                "</challenge>"), testling.serialize(authChallenge));
        }

        void testSerialize_EmptyMessage() {
            AuthChallengeSerializer testling;
            std::shared_ptr<AuthChallenge> authChallenge(new AuthChallenge());
            authChallenge->setValue(std::vector<unsigned char>());

            CPPUNIT_ASSERT_EQUAL(createSafeByteArray(
                "<challenge xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
                    "="
                "</challenge>"), testling.serialize(authChallenge));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AuthChallengeSerializerTest);
