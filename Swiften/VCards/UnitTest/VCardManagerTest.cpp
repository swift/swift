/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <vector>
#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardMemoryStorage.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Client/DummyStanzaChannel.h>

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
		CPPUNIT_TEST(testCreateSetVCardRequest);
		CPPUNIT_TEST(testCreateSetVCardRequest_Error);
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
			boost::shared_ptr<VCardManager> testling = createManager();
			VCard::ref result = testling->getVCardAndRequestWhenNeeded(JID("foo@bar.com/baz"));

			CPPUNIT_ASSERT(!result);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<VCard>(0, JID("foo@bar.com/baz"), IQ::Get));
		}

		void testGet_ExistingVCard() {
			boost::shared_ptr<VCardManager> testling = createManager();
			VCard::ref vcard(new VCard());
			vcard->setFullName("Foo Bar");
			vcardStorage->setVCard(JID("foo@bar.com/baz"), vcard);

			VCard::ref result = testling->getVCardAndRequestWhenNeeded(JID("foo@bar.com/baz"));

			CPPUNIT_ASSERT_EQUAL(std::string("Foo Bar"), result->getFullName());
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testRequest_RequestsVCard() {
			boost::shared_ptr<VCardManager> testling = createManager();
			testling->requestVCard(JID("foo@bar.com/baz"));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<VCard>(0, JID("foo@bar.com/baz"), IQ::Get));
		}

		void testRequest_ReceiveEmitsNotification() {
			boost::shared_ptr<VCardManager> testling = createManager();
			testling->requestVCard(JID("foo@bar.com/baz"));
			stanzaChannel->onIQReceived(createVCardResult());

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/baz"), changes[0].first);
			CPPUNIT_ASSERT_EQUAL(std::string("Foo Bar"), changes[0].second->getFullName());
			CPPUNIT_ASSERT_EQUAL(std::string("Foo Bar"), vcardStorage->getVCard(JID("foo@bar.com/baz"))->getFullName());

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(ownChanges.size()));
		}

		void testRequest_Error() {
			boost::shared_ptr<VCardManager> testling = createManager();
			testling->requestVCard(JID("foo@bar.com/baz"));
			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID()));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/baz"), changes[0].first);
			CPPUNIT_ASSERT_EQUAL(std::string(""), changes[0].second->getFullName());
			CPPUNIT_ASSERT_EQUAL(std::string(""), vcardStorage->getVCard(JID("foo@bar.com/baz"))->getFullName());
		}

		void testRequest_VCardAlreadyRequested() {
			boost::shared_ptr<VCardManager> testling = createManager();
			testling->requestVCard(JID("foo@bar.com/baz"));
			VCard::ref result = testling->getVCardAndRequestWhenNeeded(JID("foo@bar.com/baz"));

			CPPUNIT_ASSERT(!result);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
		}

		void testRequest_AfterPreviousRequest() {
			boost::shared_ptr<VCardManager> testling = createManager();
			testling->requestVCard(JID("foo@bar.com/baz"));
			stanzaChannel->onIQReceived(createVCardResult());
			testling->requestVCard(JID("foo@bar.com/baz"));

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<VCard>(1, JID("foo@bar.com/baz"), IQ::Get));
		}

		void testRequestOwnVCard() {
			boost::shared_ptr<VCardManager> testling = createManager();
			testling->requestVCard(ownJID);
			stanzaChannel->onIQReceived(createOwnVCardResult());

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(stanzaChannel->sentStanzas.size()));
			CPPUNIT_ASSERT(stanzaChannel->isRequestAtIndex<VCard>(0, JID(), IQ::Get));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(ownJID.toBare(), changes[0].first);
			CPPUNIT_ASSERT_EQUAL(std::string("Myself"), changes[0].second->getFullName());
			CPPUNIT_ASSERT_EQUAL(std::string("Myself"), vcardStorage->getVCard(ownJID.toBare())->getFullName());

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(ownChanges.size()));
			CPPUNIT_ASSERT_EQUAL(std::string("Myself"), ownChanges[0]->getFullName());
		}

		void testCreateSetVCardRequest() {
			boost::shared_ptr<VCardManager> testling = createManager();
			VCard::ref vcard = boost::make_shared<VCard>();
			vcard->setFullName("New Name");
			SetVCardRequest::ref request = testling->createSetVCardRequest(vcard);
			request->send();

			stanzaChannel->onIQReceived(createSetVCardResult());

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changes.size()));
			CPPUNIT_ASSERT_EQUAL(ownJID.toBare(), changes[0].first);
			CPPUNIT_ASSERT_EQUAL(std::string("New Name"), changes[0].second->getFullName());
		}

		void testCreateSetVCardRequest_Error() {
			boost::shared_ptr<VCardManager> testling = createManager();
			VCard::ref vcard = boost::make_shared<VCard>();
			vcard->setFullName("New Name");
			SetVCardRequest::ref request = testling->createSetVCardRequest(vcard);
			request->send();

			stanzaChannel->onIQReceived(IQ::createError(JID("baz@fum.com/foo"), stanzaChannel->sentStanzas[0]->getID()));

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changes.size()));
		}

	private:
		boost::shared_ptr<VCardManager> createManager() {
			boost::shared_ptr<VCardManager> manager(new VCardManager(ownJID, iqRouter, vcardStorage));
			manager->onVCardChanged.connect(boost::bind(&VCardManagerTest::handleVCardChanged, this, _1, _2));
			manager->onOwnVCardChanged.connect(boost::bind(&VCardManagerTest::handleOwnVCardChanged, this, _1));
			return manager;
		}

		void handleVCardChanged(const JID& jid, VCard::ref vcard) {
			changes.push_back(std::pair<JID, VCard::ref>(jid, vcard));
		}

		void handleOwnVCardChanged(VCard::ref vcard) {
			ownChanges.push_back(vcard);
		}

		IQ::ref createVCardResult() {
			VCard::ref vcard(new VCard());
			vcard->setFullName("Foo Bar");
			return IQ::createResult(JID("baz@fum.com/dum"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID(), vcard);
		}

		IQ::ref createOwnVCardResult() {
			VCard::ref vcard(new VCard());
			vcard->setFullName("Myself");
			return IQ::createResult(JID(), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID(), vcard);
		}

		IQ::ref createSetVCardResult() {
			return IQ::createResult(JID("baz@fum.com/dum"), stanzaChannel->sentStanzas[0]->getTo(), stanzaChannel->sentStanzas[0]->getID(), VCard::ref());
		}


	private:
		JID ownJID;
		DummyStanzaChannel* stanzaChannel;
		IQRouter* iqRouter;
		VCardMemoryStorage* vcardStorage;
		std::vector< std::pair<JID, VCard::ref> > changes;
		std::vector<VCard::ref> ownChanges;
};

CPPUNIT_TEST_SUITE_REGISTRATION(VCardManagerTest);
