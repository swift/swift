/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <vector>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Elements/Body.h>
#include <Swiften/Presence/PayloadAddingPresenceSender.h>
#include <Swiften/Presence/StanzaChannelPresenceSender.h>

using namespace Swift;

class PayloadAddingPresenceSenderTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(PayloadAddingPresenceSenderTest);
        CPPUNIT_TEST(testSetPayloadAddsPayloadOnPresenceSend);
        CPPUNIT_TEST(testSetNullPayloadDoesNotAddPayloadOnPresenceSend);
        CPPUNIT_TEST(testSendPresenceDoesNotAlterOriginalPayload);
        CPPUNIT_TEST(testSetPayloadAfterInitialPresenceResendsPresence);
        CPPUNIT_TEST(testSetPayloadAfterUnavailablePresenceDoesNotResendPresence);
        CPPUNIT_TEST(testSetPayloadAfterResetDoesNotResendPresence);
        CPPUNIT_TEST(testSendDirectedPresenceIsNotResent);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            stanzaChannel = new DummyStanzaChannel();
            presenceSender = new StanzaChannelPresenceSender(stanzaChannel);
        }

        void tearDown() {
            delete presenceSender;
            delete stanzaChannel;
        }

        void testSetPayloadAddsPayloadOnPresenceSend() {
            std::shared_ptr<PayloadAddingPresenceSender> testling(createSender());

            testling->setPayload(MyPayload::create("foo"));
            testling->sendPresence(Presence::create("bar"));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("bar"), stanzaChannel->getStanzaAtIndex<Presence>(0)->getStatus());
            CPPUNIT_ASSERT(stanzaChannel->getStanzaAtIndex<Presence>(0)->getPayload<MyPayload>());
        }

        void testSetNullPayloadDoesNotAddPayloadOnPresenceSend() {
            std::shared_ptr<PayloadAddingPresenceSender> testling(createSender());

            testling->setPayload(MyPayload::ref());
            testling->sendPresence(Presence::create("bar"));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("bar"), stanzaChannel->getStanzaAtIndex<Presence>(0)->getStatus());
            CPPUNIT_ASSERT(!stanzaChannel->getStanzaAtIndex<Presence>(0)->getPayload<MyPayload>());
        }

        void testSendPresenceDoesNotAlterOriginalPayload() {
            std::shared_ptr<PayloadAddingPresenceSender> testling(createSender());

            testling->setPayload(MyPayload::create("foo"));
            Presence::ref presence(Presence::create("bar"));
            testling->sendPresence(presence);

            CPPUNIT_ASSERT(!presence->getPayload<MyPayload>());
        }

        void testSetPayloadAfterInitialPresenceResendsPresence() {
            std::shared_ptr<PayloadAddingPresenceSender> testling(createSender());

            testling->sendPresence(Presence::create("bar"));
            testling->setPayload(MyPayload::create("foo"));

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(stanzaChannel->sentStanzas.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("bar"), stanzaChannel->getStanzaAtIndex<Presence>(1)->getStatus());
            CPPUNIT_ASSERT(stanzaChannel->getStanzaAtIndex<Presence>(1)->getPayload<MyPayload>());
        }

        void testSetPayloadAfterUnavailablePresenceDoesNotResendPresence() {
            std::shared_ptr<PayloadAddingPresenceSender> testling(createSender());

            testling->sendPresence(Presence::create("bar"));

            Presence::ref presence = Presence::create("bar");
            presence->setType(Presence::Unavailable);
            testling->sendPresence(presence);

            testling->setPayload(MyPayload::create("foo"));

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(stanzaChannel->sentStanzas.size()));
        }

        void testSetPayloadAfterResetDoesNotResendPresence() {
            std::shared_ptr<PayloadAddingPresenceSender> testling(createSender());
            testling->sendPresence(Presence::create("bar"));

            testling->reset();
            testling->setPayload(MyPayload::create("foo"));

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
        }

        void testSendDirectedPresenceIsNotResent() {
            std::shared_ptr<PayloadAddingPresenceSender> testling(createSender());

            testling->sendPresence(Presence::create("bar"));
            Presence::ref directedPresence = Presence::create("baz");
            directedPresence->setTo(JID("foo@bar.com"));
            testling->sendPresence(directedPresence);
            testling->setPayload(MyPayload::create("foo"));

            CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(stanzaChannel->sentStanzas.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("bar"), stanzaChannel->getStanzaAtIndex<Presence>(2)->getStatus());
        }

    private:
        std::shared_ptr<PayloadAddingPresenceSender> createSender() {
            std::shared_ptr<PayloadAddingPresenceSender> sender(new PayloadAddingPresenceSender(presenceSender));
            return sender;
        }

        struct MyPayload : public Payload {
                typedef std::shared_ptr<MyPayload> ref;

                MyPayload(const std::string& body) : body(body) {}

                static ref create(const std::string& body) {
                    return ref(new MyPayload(body));
                }

                std::string body;
        };

    private:
        DummyStanzaChannel* stanzaChannel;
        StanzaChannelPresenceSender* presenceSender;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PayloadAddingPresenceSenderTest);
