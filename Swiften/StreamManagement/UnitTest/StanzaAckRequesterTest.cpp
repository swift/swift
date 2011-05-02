/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/StreamManagement/StanzaAckRequester.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/IQ.h>

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
			boost::shared_ptr<StanzaAckRequester> testling(createRequester());
			testling->handleStanzaSent(createMessage("m1"));

			CPPUNIT_ASSERT_EQUAL(1, acksRequested);
		}

		void testHandleStanzaSent_IQDoesNotRequestAck() {
			boost::shared_ptr<StanzaAckRequester> testling(createRequester());
			testling->handleStanzaSent(createIQ("iq1"));

			CPPUNIT_ASSERT_EQUAL(0, acksRequested);
		}

		void testHandleStanzaSent_PresenceDoesNotRequestAck() {
			boost::shared_ptr<StanzaAckRequester> testling(createRequester());
			testling->handleStanzaSent(createPresence("p1"));

			CPPUNIT_ASSERT_EQUAL(0, acksRequested);
		}

		void testHandleAckReceived_AcksStanza() {
			boost::shared_ptr<StanzaAckRequester> testling(createRequester());
			testling->handleStanzaSent(createMessage("m1"));

			testling->handleAckReceived(1);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(ackedStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(std::string("m1"), ackedStanzas[0]->getID());
		}

		void testHandleAckReceived_AcksMultipleMessages() {
			boost::shared_ptr<StanzaAckRequester> testling(createRequester());
			testling->handleStanzaSent(createMessage("m1"));
			testling->handleStanzaSent(createMessage("m2"));

			testling->handleAckReceived(2);

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(ackedStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(std::string("m1"), ackedStanzas[0]->getID());
			CPPUNIT_ASSERT_EQUAL(std::string("m2"), ackedStanzas[1]->getID());
		}

		void testHandleAckReceived_AcksMultipleStanzas() {
			boost::shared_ptr<StanzaAckRequester> testling(createRequester());
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
			boost::shared_ptr<StanzaAckRequester> testling(createRequester());
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
			boost::shared_ptr<StanzaAckRequester> testling(createRequester());
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

		void handleStanzaAcked(boost::shared_ptr<Stanza> stanza) {
			ackedStanzas.push_back(stanza);
		}

	private:
		int acksRequested;
		std::vector< boost::shared_ptr<Stanza> > ackedStanzas;
};

}

CPPUNIT_TEST_SUITE_REGISTRATION(Swift::StanzaAckRequesterTest);
