/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Client/NickResolver.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Roster/XMPPRosterImpl.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardMemoryStorage.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Client/DummyStanzaChannel.h>

using namespace Swift;

class NickResolverTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(NickResolverTest);
		CPPUNIT_TEST(testNoMatch);
		CPPUNIT_TEST(testZeroLengthMatch);
		CPPUNIT_TEST(testMatch);
		CPPUNIT_TEST(testOverwrittenMatch);
		CPPUNIT_TEST(testRemovedMatch);
		CPPUNIT_TEST(testMUCNick);
		CPPUNIT_TEST(testMUCNoNick);
		CPPUNIT_TEST(testRemovedMatch);
		CPPUNIT_TEST(testOwnNickFullOnly);
		CPPUNIT_TEST(testOwnNickGivenAndFull);
		CPPUNIT_TEST(testOwnNickNickEtAl);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			ownJID_ = JID("kev@wonderland.lit");
			xmppRoster_ = new XMPPRosterImpl();
			stanzaChannel_ = new DummyStanzaChannel();
		  iqRouter_ = new IQRouter(stanzaChannel_);
			vCardStorage_ = new VCardMemoryStorage();
			vCardManager_ = new VCardManager(ownJID_, iqRouter_, vCardStorage_);
			registry_ = new MUCRegistry();
			resolver_ = new NickResolver(ownJID_, xmppRoster_, vCardManager_, registry_);
		}

		void tearDown() {
			delete resolver_;
			delete registry_;
			delete vCardManager_;
			delete iqRouter_;
			delete stanzaChannel_;
			delete vCardStorage_;
			delete xmppRoster_;
		}

		void testMUCNick() {
			registry_->addMUC(JID("foo@bar"));
			JID testJID("foo@bar/baz");

			CPPUNIT_ASSERT_EQUAL(std::string("baz"), resolver_->jidToNick(testJID));
		}

		void testMUCNoNick() {
			registry_->addMUC(JID("foo@bar"));
			JID testJID("foo@bar");

			CPPUNIT_ASSERT_EQUAL(std::string("foo@bar"), resolver_->jidToNick(testJID));
		}


		void testNoMatch() {
			JID testJID("foo@bar/baz");

			CPPUNIT_ASSERT_EQUAL(std::string("foo@bar"), resolver_->jidToNick(testJID));
		}
		
		void testZeroLengthMatch() {
			JID testJID("foo@bar/baz");
			xmppRoster_->addContact(testJID, "", groups_, RosterItemPayload::Both);
			CPPUNIT_ASSERT_EQUAL(std::string("foo@bar"), resolver_->jidToNick(testJID));
		}

		void testMatch() {
			JID testJID("foo@bar/baz");
			xmppRoster_->addContact(testJID, "Test", groups_, RosterItemPayload::Both);

			CPPUNIT_ASSERT_EQUAL(std::string("Test"), resolver_->jidToNick(testJID));
		}

		void testOverwrittenMatch() {
			JID testJID("foo@bar/baz");
			xmppRoster_->addContact(testJID, "FailTest", groups_, RosterItemPayload::Both);
			xmppRoster_->addContact(testJID, "Test", groups_, RosterItemPayload::Both);

			CPPUNIT_ASSERT_EQUAL(std::string("Test"), resolver_->jidToNick(testJID));
		}

		void testRemovedMatch() {
			JID testJID("foo@bar/baz");
			xmppRoster_->addContact(testJID, "FailTest", groups_, RosterItemPayload::Both);
			xmppRoster_->removeContact(testJID);
			CPPUNIT_ASSERT_EQUAL(std::string("foo@bar"), resolver_->jidToNick(testJID));
		}

		void testOwnNickFullOnly() {
			populateOwnVCard("", "", "Kevin Smith");
			CPPUNIT_ASSERT_EQUAL(std::string("Kevin Smith"), resolver_->jidToNick(ownJID_));
		}

		void testOwnNickGivenAndFull() {
			populateOwnVCard("", "Kevin", "Kevin Smith");
			CPPUNIT_ASSERT_EQUAL(std::string("Kevin"), resolver_->jidToNick(ownJID_));
		}

		void testOwnNickNickEtAl() {
			populateOwnVCard("Kev", "Kevin", "Kevin Smith");
			CPPUNIT_ASSERT_EQUAL(std::string("Kev"), resolver_->jidToNick(ownJID_));
		}

		void populateOwnVCard(const std::string& nick, const std::string& given, const std::string& full) {
			VCard::ref vcard(new VCard());
			if (!nick.empty()) {
				vcard->setNickname(nick);
			}
			if (!given.empty()) {
				vcard->setGivenName(given);
			}
			if (!full.empty()) {
				vcard->setFullName(full);
			}
			vCardManager_->requestVCard(ownJID_);
			IQ::ref result(IQ::createResult(JID(), stanzaChannel_->sentStanzas[0]->getID(), vcard));
			stanzaChannel_->onIQReceived(result);
		}
	
	private:
		std::vector<std::string> groups_;
		XMPPRosterImpl* xmppRoster_;
		VCardStorage* vCardStorage_;
		IQRouter* iqRouter_;
		DummyStanzaChannel* stanzaChannel_;
		VCardManager* vCardManager_;
		MUCRegistry* registry_;
		NickResolver* resolver_;
		JID ownJID_;

};

CPPUNIT_TEST_SUITE_REGISTRATION(NickResolverTest);

