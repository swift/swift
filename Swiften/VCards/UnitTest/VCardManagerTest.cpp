/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <vector>
#include <boost/bind.hpp>

#include "Swiften/VCards/VCardManager.h"
#include "Swiften/VCards/VCardMemoryStorage.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Client/DummyStanzaChannel.h"

using namespace Swift;

class VCardManagerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(VCardManagerTest);
		CPPUNIT_TEST(testGet_NewVCardRequestsVCard);
		CPPUNIT_TEST(testGet_ExistingVCard);
		CPPUNIT_TEST(testRequest_RequestsVCard);
		CPPUNIT_TEST(testRequest_ReceiveEmitsNotification);
		CPPUNIT_TEST(testRequest_Error);
		CPPUNIT_TEST(testRequest_VCardAlreadyRequested);
		CPPUNIT_TEST(testRequest_AfterPreviousRequest);
		CPPUNIT_TEST(testRequestOwnVCard);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			ownJID = JID("baz@fum.com/dum");
			stanzaChannel = new DummyStanzaChannel();
			iqRouter = new IQRouter(stanzaChannel);
			vcardStorage = new VCardMemoryStorage();
		}

		void tearDown() {
			delete vcardStorage;
			delete iqRouter;
			delete stanzaChannel;
		}

		void testGet_NewVCardRequestsVCard() {
			std::auto_ptr<VCardManager> testling = createManager();
			VCard::ref result = testling->getVCardAndRequestWhenNeeded(JID("foo@bar.com/baz"));

			CPPUNIT_ASSERT(!result);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<VCard>(0, JID("foo@bar.com/baz"), IQ::Get));
		}

		void testGet_ExistingVCard() {
			std::auto_ptr<VCardManager> testling = createManager();
			VCard::ref vcard(new VCard());
			vcard->setFullName("Foo Bar");
			vcardStorage->setVCard(JID("foo@bar.com/baz"), vcard);

			VCard::ref result = testling->getVCardAndRequestWhenNeeded(JID("foo@bar.com/baz"));

			CPPUNIT_ASSERT_EQUAL(String("Foo Bar"), result->getFullName());
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testRequest_RequestsVCard() {
			std::auto_ptr<VCardManager> testling = createManager();
			testling->requestVCard(JID("foo@bar.com/baz"));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<VCard>(0, JID("foo@bar.com/baz"), IQ::Get));
		}

		void testRequest_ReceiveEmitsNotification() {
			std::auto_ptr<VCardManager> testling = createManager();
			testling->requestVCard(JID("foo@bar.com/baz"));
			stanzaChannel->onIQReceived(createVCardResult());

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/baz"), changes[0].first);
			CPPUNIT_ASSERT_EQUAL(String("Foo Bar"), changes[0].second->getFullName());
			CPPUNIT_ASSERT_EQUAL(String("Foo Bar"), vcardStorage->getVCard(JID("foo@bar.com/baz"))->getFullName());
		}

		void testRequest_Error() {
			std::auto_ptr<VCardManager> testling = createManager();
			testling->requestVCard(JID("foo@bar.com/baz"));
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/baz"), changes[0].first);
			CPPUNIT_ASSERT_EQUAL(String(""), changes[0].second->getFullName());
			CPPUNIT_ASSERT_EQUAL(String(""), vcardStorage->getVCard(JID("foo@bar.com/baz"))->getFullName());
		}

		void testRequest_VCardAlreadyRequested() {
			std::auto_ptr<VCardManager> testling = createManager();
			testling->requestVCard(JID("foo@bar.com/baz"));
			VCard::ref result = testling->getVCardAndRequestWhenNeeded(JID("foo@bar.com/baz"));

			CPPUNIT_ASSERT(!result);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testRequest_AfterPreviousRequest() {
			std::auto_ptr<VCardManager> testling = createManager();
			testling->requestVCard(JID("foo@bar.com/baz"));
			stanzaChannel->onIQReceived(createVCardResult());
			testling->requestVCard(JID("foo@bar.com/baz"));

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<VCard>(1, JID("foo@bar.com/baz"), IQ::Get));
		}

		void testRequestOwnVCard() {
			std::auto_ptr<VCardManager> testling = createManager();
			testling->requestVCard(ownJID);
			stanzaChannel->onIQReceived(createOwnVCardResult());

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<VCard>(0, JID(), IQ::Get));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(ownJID.toBare(), changes[0].first);
			CPPUNIT_ASSERT_EQUAL(String("Myself"), changes[0].second->getFullName());
			CPPUNIT_ASSERT_EQUAL(String("Myself"), vcardStorage->getVCard(ownJID.toBare())->getFullName());
		}

	private:
		std::auto_ptr<VCardManager> createManager() {
			std::auto_ptr<VCardManager> manager(new VCardManager(ownJID, iqRouter, vcardStorage));
			manager->onVCardChanged.connect(boost::bind(&VCardManagerTest::handleVCardChanged, this, _1, _2));
			return manager;
		}

		void handleVCardChanged(const JID& jid, VCard::ref vcard) {
			changes.push_back(std::pair<JID, VCard::ref>(jid, vcard));
		}

		IQ::ref createVCardResult() {
			VCard::ref vcard(new VCard());
			vcard->setFullName("Foo Bar");
			return IQ::createResult(JID("baz@fum.com/dum"), stanzaChannel->sentStanzas[0]->getID(), vcard);
		}

		IQ::ref createOwnVCardResult() {
			VCard::ref vcard(new VCard());
			vcard->setFullName("Myself");
			return IQ::createResult(JID(), stanzaChannel->sentStanzas[0]->getID(), vcard);
		}

	private:
		JID ownJID;
		DummyStanzaChannel* stanzaChannel;
		IQRouter* iqRouter;
		VCardMemoryStorage* vcardStorage;
		std::vector< std::pair<JID, VCard::ref> > changes;
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardManagerTest);
