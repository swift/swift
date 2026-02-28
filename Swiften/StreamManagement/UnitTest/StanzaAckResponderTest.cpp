/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cstdint>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/Message.h>
#include <Swiften/StreamManagement/StanzaAckResponder.h>

using namespace Swift;

namespace Swift {

class StanzaAckResponderTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(StanzaAckResponderTest);
        CPPUNIT_TEST(testHandleAckRequestReceived_AcksStanza);
        CPPUNIT_TEST(testHandleAckRequestReceived_AcksMultipleStanzas);
        CPPUNIT_TEST(testHandleAckRequestReceived_MultipleAcks);
        CPPUNIT_TEST(testHandleAckRequestReceived_WrapAround);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testHandleAckRequestReceived_AcksStanza() {
            std::shared_ptr<StanzaAckResponder> testling(createResponder());
            testling->handleStanzaReceived();

            testling->handleAckRequestReceived();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(acks.size()));
            CPPUNIT_ASSERT_EQUAL(1U, acks[0]);
        }

        void testHandleAckRequestReceived_AcksMultipleStanzas() {
            std::shared_ptr<StanzaAckResponder> testling(createResponder());
            testling->handleStanzaReceived();
            testling->handleStanzaReceived();

            testling->handleAckRequestReceived();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(acks.size()));
            CPPUNIT_ASSERT_EQUAL(2U, acks[0]);
        }

        void testHandleAckRequestReceived_MultipleAcks() {
            std::shared_ptr<StanzaAckResponder> testling(createResponder());
            testling->handleStanzaReceived();
            testling->handleAckRequestReceived();

            testling->handleStanzaReceived();
            testling->handleAckRequestReceived();

            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(acks.size()));
            CPPUNIT_ASSERT_EQUAL(1U, acks[0]);
            CPPUNIT_ASSERT_EQUAL(2U, acks[1]);
        }

        // Handle stanza ack count wrapping, as per the XEP
        void testHandleAckRequestReceived_WrapAround() {
            std::shared_ptr<StanzaAckResponder> testling(createResponder());
            testling->handledStanzasCount = UINT32_MAX;
            testling->handleStanzaReceived();
            testling->handleStanzaReceived();

            testling->handleAckRequestReceived();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(acks.size()));
            CPPUNIT_ASSERT_EQUAL(1U, acks[0]);
        }

    private:
        Message::ref createMessage(const std::string& id) {
            Message::ref result(new Message());
            result->setID(id);
            return result;
        }

        StanzaAckResponder* createResponder() {
            StanzaAckResponder* responder = new StanzaAckResponder();
            responder->onAck.connect(boost::bind(&StanzaAckResponderTest::handleAck, this, _1));
            return responder;
        }

        void handleAck(unsigned int h) {
            acks.push_back(h);
        }

    private:
        std::vector<unsigned int> acks;
};

}

CPPUNIT_TEST_SUITE_REGISTRATION(Swift::StanzaAckResponderTest);
