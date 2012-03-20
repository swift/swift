/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Presence/DirectedPresenceSender.h>
#include <Swiften/Presence/StanzaChannelPresenceSender.h>

using namespace Swift;

class DirectedPresenceSenderTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(DirectedPresenceSenderTest);
		CPPUNIT_TEST(testSendPresence);
		CPPUNIT_TEST(testSendPresence_UndirectedPresenceWithDirectedPresenceReceivers);
		CPPUNIT_TEST(testSendPresence_DirectedPresenceWithDirectedPresenceReceivers);
		CPPUNIT_TEST(testAddDirectedPresenceReceiver);
		CPPUNIT_TEST(testAddDirectedPresenceReceiver_WithoutSendingPresence);
		CPPUNIT_TEST(testAddDirectedPresenceReceiver_AfterSendingDirectedPresence);
		CPPUNIT_TEST(testRemoveDirectedPresenceReceiver);
		CPPUNIT_TEST(testRemoveDirectedPresenceReceiver_WithoutSendingPresence);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			channel = new DummyStanzaChannel();
			testPresence = boost::make_shared<Presence>();
			testPresence->setStatus("Foo");
			secondTestPresence = boost::make_shared<Presence>();
			secondTestPresence->setStatus("Bar");
			stanzaChannelPresenceSender = new StanzaChannelPresenceSender(channel);
		}

		void tearDown() {
			delete stanzaChannelPresenceSender;
			delete channel;
		}

		void testSendPresence() {
			boost::shared_ptr<DirectedPresenceSender> testling(createPresenceSender());
			testling->sendPresence(testPresence);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel->sentStanzas.size()));
			boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0]);
			CPPUNIT_ASSERT(testPresence == presence);
		}

		void testSendPresence_UndirectedPresenceWithDirectedPresenceReceivers() {
			boost::shared_ptr<DirectedPresenceSender> testling(createPresenceSender());
			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"), DirectedPresenceSender::AndSendPresence);

			testling->sendPresence(testPresence);

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(channel->sentStanzas.size()));
			boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0]);
			CPPUNIT_ASSERT(testPresence == presence);
			presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[1]);
			CPPUNIT_ASSERT_EQUAL(testPresence->getStatus(), presence->getStatus());
			CPPUNIT_ASSERT_EQUAL(JID("alice@wonderland.lit/teaparty"), presence->getTo());
		}

		void testSendPresence_DirectedPresenceWithDirectedPresenceReceivers() {
			boost::shared_ptr<DirectedPresenceSender> testling(createPresenceSender());
			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"), DirectedPresenceSender::AndSendPresence);
			channel->sentStanzas.clear();

			testPresence->setTo(JID("foo@bar.com"));
			testling->sendPresence(testPresence);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel->sentStanzas.size()));
			boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0]);
			CPPUNIT_ASSERT(testPresence == presence);
		}

		void testAddDirectedPresenceReceiver() {
			boost::shared_ptr<DirectedPresenceSender> testling(createPresenceSender());
			testling->sendPresence(testPresence);
			channel->sentStanzas.clear();

			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"), DirectedPresenceSender::AndSendPresence);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel->sentStanzas.size()));
			boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0]);
			CPPUNIT_ASSERT_EQUAL(testPresence->getStatus(), presence->getStatus());
			CPPUNIT_ASSERT_EQUAL(JID("alice@wonderland.lit/teaparty"), presence->getTo());
		}

		void testAddDirectedPresenceReceiver_WithoutSendingPresence() {
			boost::shared_ptr<DirectedPresenceSender> testling(createPresenceSender());
			testling->sendPresence(testPresence);
			channel->sentStanzas.clear();

			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"), DirectedPresenceSender::DontSendPresence);

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(channel->sentStanzas.size()));
		}

		void testAddDirectedPresenceReceiver_AfterSendingDirectedPresence() {
			boost::shared_ptr<DirectedPresenceSender> testling(createPresenceSender());
			testling->sendPresence(testPresence);
			secondTestPresence->setTo(JID("foo@bar.com"));
			testling->sendPresence(secondTestPresence);
			channel->sentStanzas.clear();

			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"), DirectedPresenceSender::AndSendPresence);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel->sentStanzas.size()));
			boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0]);
			CPPUNIT_ASSERT_EQUAL(testPresence->getStatus(), presence->getStatus());
			CPPUNIT_ASSERT_EQUAL(JID("alice@wonderland.lit/teaparty"), presence->getTo());
		}

		void testRemoveDirectedPresenceReceiver() {
			boost::shared_ptr<DirectedPresenceSender> testling(createPresenceSender());
			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"), DirectedPresenceSender::DontSendPresence);

			testling->removeDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"), DirectedPresenceSender::AndSendPresence);
			testling->sendPresence(testPresence);

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(channel->sentStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0])->getType(), Presence::Unavailable);
			CPPUNIT_ASSERT(channel->sentStanzas[1] == testPresence);
		}

		void testRemoveDirectedPresenceReceiver_WithoutSendingPresence() {
			boost::shared_ptr<DirectedPresenceSender> testling(createPresenceSender());
			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"), DirectedPresenceSender::AndSendPresence);
			channel->sentStanzas.clear();

			testling->removeDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"), DirectedPresenceSender::DontSendPresence);
			testling->sendPresence(testPresence);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel->sentStanzas.size()));
			CPPUNIT_ASSERT(channel->sentStanzas[0] == testPresence);
		}

	private:
		DirectedPresenceSender* createPresenceSender() {
			return new DirectedPresenceSender(stanzaChannelPresenceSender);
		}
	
	private:
		DummyStanzaChannel* channel;
		StanzaChannelPresenceSender* stanzaChannelPresenceSender;
		boost::shared_ptr<Presence> testPresence;
		boost::shared_ptr<Presence> secondTestPresence;
};

CPPUNIT_TEST_SUITE_REGISTRATION(DirectedPresenceSenderTest);
