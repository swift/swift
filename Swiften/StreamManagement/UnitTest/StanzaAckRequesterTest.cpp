/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/StreamManagement/StanzaAckRequester.h>

using namespace Swift;

namespace Swift {

class StanzaAckRequesterTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(StanzaAckRequesterTest);
        CPPUNIT_TEST(testHandleStanzaSent_MessageRequestsAck);
        CPPUNIT_TEST(testHandleStanzaSent_IQDoesNotRequestAck);
        CPPUNIT_TEST(testHandleStanzaSent_PresenceDoesNotRequestAck);
        CPPUNIT_TEST(testHandleAckReceived_AcksStanza);
        CPPUNIT_TEST(testHandleAckReceived_AcksMultipleMessages);
        CPPUNIT_TEST(testHandleAckReceived_AcksMultipleStanzas);
        CPPUNIT_TEST(testHandleAckReceived_MultipleAcks);
        CPPUNIT_TEST(testHandleAckReceived_WrapAround);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            acksRequested = 0;
        }

        void testHandleStanzaSent_MessageRequestsAck() {
            std::shared_ptr<StanzaAckRequester> testling(createRequester());
            testling->handleStanzaSent(createMessage("m1"));

            CPPUNIT_ASSERT_EQUAL(1, acksRequested);
        }

        void testHandleStanzaSent_IQDoesNotRequestAck() {
            std::shared_ptr<StanzaAckRequester> testling(createRequester());
            testling->handleStanzaSent(createIQ("iq1"));

            CPPUNIT_ASSERT_EQUAL(0, acksRequested);
        }

        void testHandleStanzaSent_PresenceDoesNotRequestAck() {
            std::shared_ptr<StanzaAckRequester> testling(createRequester());
            testling->handleStanzaSent(createPresence("p1"));

            CPPUNIT_ASSERT_EQUAL(0, acksRequested);
        }

        void testHandleAckReceived_AcksStanza() {
            std::shared_ptr<StanzaAckRequester> testling(createRequester());
            testling->handleStanzaSent(createMessage("m1"));

            testling->handleAckReceived(1);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(ackedStanzas.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("m1"), ackedStanzas[0]->getID());
        }

        void testHandleAckReceived_AcksMultipleMessages() {
            std::shared_ptr<StanzaAckRequester> testling(createRequester());
            testling->handleStanzaSent(createMessage("m1"));
            testling->handleStanzaSent(createMessage("m2"));

            testling->handleAckReceived(2);

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(ackedStanzas.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("m1"), ackedStanzas[0]->getID());
            CPPUNIT_ASSERT_EQUAL(std::string("m2"), ackedStanzas[1]->getID());
        }

        void testHandleAckReceived_AcksMultipleStanzas() {
            std::shared_ptr<StanzaAckRequester> testling(createRequester());
            testling->handleStanzaSent(createIQ("iq1"));
            testling->handleStanzaSent(createPresence("p1"));
            testling->handleStanzaSent(createMessage("m1"));

            testling->handleAckReceived(3);

            CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(ackedStanzas.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("iq1"), ackedStanzas[0]->getID());
            CPPUNIT_ASSERT_EQUAL(std::string("p1"), ackedStanzas[1]->getID());
            CPPUNIT_ASSERT_EQUAL(std::string("m1"), ackedStanzas[2]->getID());
        }

        void testHandleAckReceived_MultipleAcks() {
            std::shared_ptr<StanzaAckRequester> testling(createRequester());
            testling->handleStanzaSent(createMessage("m1"));
            testling->handleAckReceived(1);

            testling->handleStanzaSent(createMessage("m2"));
            testling->handleStanzaSent(createMessage("m3"));
            testling->handleAckReceived(3);

            CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(ackedStanzas.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("m1"), ackedStanzas[0]->getID());
            CPPUNIT_ASSERT_EQUAL(std::string("m2"), ackedStanzas[1]->getID());
            CPPUNIT_ASSERT_EQUAL(std::string("m3"), ackedStanzas[2]->getID());
        }

        // Handle stanza ack count wrapping, as per the XEP
        void testHandleAckReceived_WrapAround() {
            std::shared_ptr<StanzaAckRequester> testling(createRequester());
            testling->lastHandledStanzasCount = boost::numeric_cast<unsigned int>((1ULL<<32) - 1);
            testling->handleStanzaSent(createMessage("m1"));
            testling->handleStanzaSent(createMessage("m2"));

            testling->handleAckReceived(1);

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(ackedStanzas.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("m1"), ackedStanzas[0]->getID());
            CPPUNIT_ASSERT_EQUAL(std::string("m2"), ackedStanzas[1]->getID());
        }

    private:
        Message::ref createMessage(const std::string& id) {
            Message::ref result(new Message());
            result->setID(id);
            return result;
        }

        IQ::ref createIQ(const std::string& id) {
            IQ::ref result(new IQ());
            result->setID(id);
            return result;
        }

        Presence::ref createPresence(const std::string& id) {
            Presence::ref result(new Presence());
            result->setID(id);
            return result;
        }

        StanzaAckRequester* createRequester() {
            StanzaAckRequester* requester = new StanzaAckRequester();
            requester->onRequestAck.connect(boost::bind(&StanzaAckRequesterTest::handleRequestAck, this));
            requester->onStanzaAcked.connect(boost::bind(&StanzaAckRequesterTest::handleStanzaAcked, this, _1));
            return requester;
        }

        void handleRequestAck() {
            acksRequested++;
        }

        void handleStanzaAcked(std::shared_ptr<Stanza> stanza) {
            ackedStanzas.push_back(stanza);
        }

    private:
        int acksRequested = 0;
        std::vector< std::shared_ptr<Stanza> > ackedStanzas;
};

}

CPPUNIT_TEST_SUITE_REGISTRATION(Swift::StanzaAckRequesterTest);
