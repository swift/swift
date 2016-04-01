/*
 * Copyright (c) 2015 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/Delay.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/IsodeIQDelegation.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/Subject.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/IsodeIQDelegationSerializer.h>

using namespace Swift;

class IsodeIQDelegationSerializerTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(IsodeIQDelegationSerializerTest);
        CPPUNIT_TEST(testSerialize_Forwarded_IQ);
        CPPUNIT_TEST(testSerialize_Forwarded_Message);
        CPPUNIT_TEST(testSerialize_Forwarded_MessageNoDelay);
        CPPUNIT_TEST(testSerialize_Forwarded_Presence);
        CPPUNIT_TEST_SUITE_END();

    public:
        IsodeIQDelegationSerializerTest() {}

        void testSerialize_Forwarded_IQ() {
            IsodeIQDelegationSerializer testling(&serializers);
            std::shared_ptr<IsodeIQDelegation> isodeIQDelegation = std::make_shared<IsodeIQDelegation>();

            std::shared_ptr<IQ> iq = IQ::createResult(JID("juliet@capulet.lit/balcony"), JID("romeo@montague.lit/orchard"), "id0", std::make_shared<Subject>("text"));

            std::shared_ptr<Forwarded> forwarded(std::make_shared<Forwarded>());
            forwarded->setStanza(iq);
            forwarded->setDelay(std::make_shared<Delay>(stringToDateTime(std::string("2010-07-10T23:08:25Z"))));
            isodeIQDelegation->setForward(forwarded);

            CPPUNIT_ASSERT_EQUAL(std::string("<delegate xmlns=\"http://isode.com/iq_delegation\">"
                                "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                                "<delay stamp=\"2010-07-10T23:08:25Z\" xmlns=\"urn:xmpp:delay\"/>"
                                "<iq from=\"romeo@montague.lit/orchard\" id=\"id0\" to=\"juliet@capulet.lit/balcony\" type=\"result\" xmlns=\"jabber:client\"><subject>text</subject></iq>"
                                "</forwarded>"
                                "</delegate>"), testling.serialize(isodeIQDelegation));
        }

        void testSerialize_Forwarded_Message() {
            IsodeIQDelegationSerializer testling(&serializers);
            std::shared_ptr<IsodeIQDelegation> isodeIQDelegation = std::make_shared<IsodeIQDelegation>();

            std::shared_ptr<Message> message(std::make_shared<Message>());
            message->setType(Message::Chat);
            message->setTo(JID("juliet@capulet.lit/balcony"));
            message->setFrom(JID("romeo@montague.lit/orchard"));
            message->setBody("Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.");

            std::shared_ptr<Forwarded> forwarded(std::make_shared<Forwarded>());
            forwarded->setStanza(message);
            forwarded->setDelay(std::make_shared<Delay>(stringToDateTime(std::string("2010-07-10T23:08:25Z"))));

            isodeIQDelegation->setForward(forwarded);
            CPPUNIT_ASSERT_EQUAL(std::string("<delegate xmlns=\"http://isode.com/iq_delegation\">"
                                                  "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                                                "<delay stamp=\"2010-07-10T23:08:25Z\" xmlns=\"urn:xmpp:delay\"/>"
                                                "<message from=\"romeo@montague.lit/orchard\" to=\"juliet@capulet.lit/balcony\" type=\"chat\" xmlns=\"jabber:client\">"
                                                    "<body>Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.</body>"
                                                "</message>"
                                                "</forwarded>"
                                              "</delegate>"), testling.serialize(isodeIQDelegation));
        }

        void testSerialize_Forwarded_MessageNoDelay() {
            IsodeIQDelegationSerializer testling(&serializers);
            std::shared_ptr<IsodeIQDelegation> isodeIQDelegation = std::make_shared<IsodeIQDelegation>();

            std::shared_ptr<Message> message(std::make_shared<Message>());
            message->setType(Message::Chat);
            message->setTo(JID("juliet@capulet.lit/balcony"));
            message->setFrom(JID("romeo@montague.lit/orchard"));
            message->setBody("Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.");

            std::shared_ptr<Forwarded> forwarded(std::make_shared<Forwarded>());
            forwarded->setStanza(message);
            isodeIQDelegation->setForward(forwarded);

            CPPUNIT_ASSERT_EQUAL(std::string("<delegate xmlns=\"http://isode.com/iq_delegation\">"
                                                   "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                                                    "<message from=\"romeo@montague.lit/orchard\" to=\"juliet@capulet.lit/balcony\" type=\"chat\" xmlns=\"jabber:client\">"
                                                        "<body>Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.</body>"
                                                    "</message>"
                                                "</forwarded>"
                                              "</delegate>"), testling.serialize(isodeIQDelegation));
        }

        void testSerialize_Forwarded_Presence() {
            IsodeIQDelegationSerializer testling(&serializers);
            std::shared_ptr<IsodeIQDelegation> isodeIQDelegation = std::make_shared<IsodeIQDelegation>();

            std::shared_ptr<Presence> presence(std::make_shared<Presence>());
            presence->setType(Presence::Subscribe);

            std::shared_ptr<Forwarded> forwarded(std::make_shared<Forwarded>());
            forwarded->setStanza(presence);
            forwarded->setDelay(std::make_shared<Delay>(stringToDateTime(std::string("2010-07-10T23:08:25Z"))));
            isodeIQDelegation->setForward(forwarded);

            CPPUNIT_ASSERT_EQUAL(std::string("<delegate xmlns=\"http://isode.com/iq_delegation\">"
                                                   "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                                                    "<delay stamp=\"2010-07-10T23:08:25Z\" xmlns=\"urn:xmpp:delay\"/>"
                                                    "<presence type=\"subscribe\" xmlns=\"jabber:client\"/>"
                                                "</forwarded>"
                                            "</delegate>"), testling.serialize(isodeIQDelegation));
        }
    private:
        FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IsodeIQDelegationSerializerTest);
