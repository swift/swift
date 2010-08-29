/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "Swiften/StreamManagement/StanzaAckRequester.h"
#include "Swiften/Elements/Message.h"

using namespace Swift;

namespace Swift {

class StanzaAckRequesterTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(StanzaAckRequesterTest);
		CPPUNIT_TEST(testHandleStanzaSent_RequestsAck);
		CPPUNIT_TEST(testHandleAckReceived_AcksStanza);
		CPPUNIT_TEST(testHandleAckReceived_AcksMultipleStanzas);
		CPPUNIT_TEST(testHandleAckReceived_MultipleAcks);
		CPPUNIT_TEST(testHandleAckReceived_WrapAround);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			acksRequested = 0;
		}

		void testHandleStanzaSent_RequestsAck() {
			std::auto_ptr<StanzaAckRequester> testling(createRequester());
			testling->handleStanzaSent(createMessage("m1"));

			CPPUNIT_ASSERT_EQUAL(1, acksRequested);
		}

		void testHandleAckReceived_AcksStanza() {
			std::auto_ptr<StanzaAckRequester> testling(createRequester());
			testling->handleStanzaSent(createMessage("m1"));

			testling->handleAckReceived(1);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(ackedStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(String("m1"), ackedStanzas[0]->getID());
		}

		void testHandleAckReceived_AcksMultipleStanzas() {
			std::auto_ptr<StanzaAckRequester> testling(createRequester());
			testling->handleStanzaSent(createMessage("m1"));
			testling->handleStanzaSent(createMessage("m2"));

			testling->handleAckReceived(2);

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(ackedStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(String("m1"), ackedStanzas[0]->getID());
			CPPUNIT_ASSERT_EQUAL(String("m2"), ackedStanzas[1]->getID());
		}

		void testHandleAckReceived_MultipleAcks() {
			std::auto_ptr<StanzaAckRequester> testling(createRequester());
			testling->handleStanzaSent(createMessage("m1"));
			testling->handleAckReceived(1);

			testling->handleStanzaSent(createMessage("m2"));
			testling->handleStanzaSent(createMessage("m3"));
			testling->handleAckReceived(3);

			CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(ackedStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(String("m1"), ackedStanzas[0]->getID());
			CPPUNIT_ASSERT_EQUAL(String("m2"), ackedStanzas[1]->getID());
			CPPUNIT_ASSERT_EQUAL(String("m3"), ackedStanzas[2]->getID());
		}

		// Handle stanza ack count wrapping, as per the XEP
		void testHandleAckReceived_WrapAround() {
			std::auto_ptr<StanzaAckRequester> testling(createRequester());
			testling->lastHandledStanzasCount = boost::numeric_cast<unsigned int>((1ULL<<32) - 1);
			testling->handleStanzaSent(createMessage("m1"));
			testling->handleStanzaSent(createMessage("m2"));

			testling->handleAckReceived(1);

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(ackedStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(String("m1"), ackedStanzas[0]->getID());
			CPPUNIT_ASSERT_EQUAL(String("m2"), ackedStanzas[1]->getID());
		}

	private:
		Message::ref createMessage(const String& id) {
			Message::ref result(new Message());
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
