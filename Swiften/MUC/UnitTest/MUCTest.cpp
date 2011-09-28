/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/bind.hpp>

#include <Swiften/MUC/MUC.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Presence/StanzaChannelPresenceSender.h>
#include <Swiften/Presence/DirectedPresenceSender.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Elements/MUCUserPayload.h>
#include <Swiften/Elements/MUCOwnerPayload.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/Elements/CapsInfo.h>


using namespace Swift;

class MUCTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(MUCTest);
		CPPUNIT_TEST(testJoin);
		CPPUNIT_TEST(testJoin_ChangePresenceDuringJoinDoesNotSendPresenceBeforeJoinSuccess);
		CPPUNIT_TEST(testJoin_ChangePresenceDuringJoinResendsPresenceAfterJoinSuccess);
		CPPUNIT_TEST(testCreateInstant);
		CPPUNIT_TEST(testReplicateBug);
		/*CPPUNIT_TEST(testJoin_Success);
		CPPUNIT_TEST(testJoin_Fail);*/
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			channel = new DummyStanzaChannel();
			router = new IQRouter(channel);
			mucRegistry = new MUCRegistry();
			stanzaChannelPresenceSender = new StanzaChannelPresenceSender(channel);
			presenceSender = new DirectedPresenceSender(stanzaChannelPresenceSender);
		}

		void tearDown() {
			delete presenceSender;
			delete stanzaChannelPresenceSender;
			delete mucRegistry;
			delete router;
			delete channel;
		}

		void testJoin() {
			MUC::ref testling = createMUC(JID("foo@bar.com"));
			testling->joinAs("Alice");

			CPPUNIT_ASSERT(mucRegistry->isMUC(JID("foo@bar.com")));
			Presence::ref p = channel->getStanzaAtIndex<Presence>(0);
			CPPUNIT_ASSERT(p);
			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/Alice"), p->getTo());
		}

		void testJoin_ChangePresenceDuringJoinDoesNotSendPresenceBeforeJoinSuccess() {
			MUC::ref testling = createMUC(JID("foo@bar.com"));
			testling->joinAs("Alice");

			presenceSender->sendPresence(Presence::create("Test"));
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(channel->sentStanzas.size()));
		}

		void testJoin_ChangePresenceDuringJoinResendsPresenceAfterJoinSuccess() {
			MUC::ref testling = createMUC(JID("foo@bar.com"));
			testling->joinAs("Alice");

			presenceSender->sendPresence(Presence::create("Test"));
			receivePresence(JID("foo@bar.com/Rabbit"), "Here");

			CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(channel->sentStanzas.size()));
			Presence::ref p = channel->getStanzaAtIndex<Presence>(2);
			CPPUNIT_ASSERT(p);
			CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/Alice"), p->getTo());
			CPPUNIT_ASSERT_EQUAL(std::string("Test"), p->getStatus());
		}

		void testCreateInstant() {
			MUC::ref testling = createMUC(JID("rabbithole@wonderland.lit"));
			testling->joinAs("Alice");
			Presence::ref serverRespondsLocked = boost::make_shared<Presence>();
			serverRespondsLocked->setFrom(JID("rabbithole@wonderland.lit/Alice"));
			MUCUserPayload::ref mucPayload(new MUCUserPayload());
			MUCItem myItem;
			myItem.affiliation = MUCOccupant::Owner;
			myItem.role = MUCOccupant::Moderator;
			mucPayload->addItem(myItem);
			mucPayload->addStatusCode(MUCUserPayload::StatusCode(110));
			mucPayload->addStatusCode(MUCUserPayload::StatusCode(201));
			serverRespondsLocked->addPayload(mucPayload);
			channel->onPresenceReceived(serverRespondsLocked);
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(channel->sentStanzas.size()));
			IQ::ref iq = channel->getStanzaAtIndex<IQ>(1);
			CPPUNIT_ASSERT(iq);
			CPPUNIT_ASSERT(iq->getPayload<MUCOwnerPayload>());
			CPPUNIT_ASSERT(iq->getPayload<MUCOwnerPayload>()->getForm());
			CPPUNIT_ASSERT_EQUAL(Form::SubmitType, iq->getPayload<MUCOwnerPayload>()->getForm()->getType());
		}

		void testReplicateBug() {
			Presence::ref initialPresence = boost::make_shared<Presence>();
			initialPresence->setStatus("");
			VCard::ref vcard = boost::make_shared<VCard>();
			vcard->setPhoto(createByteArray("15c30080ae98ec48be94bf0e191d43edd06e500a"));
			initialPresence->addPayload(vcard);
			CapsInfo::ref caps = boost::make_shared<CapsInfo>();
			caps->setNode("http://swift.im");
			caps->setVersion("p2UP0DrcVgKM6jJqYN/B92DKK0o=");
			initialPresence->addPayload(caps);
			channel->sendPresence(initialPresence);

			MUC::ref testling = createMUC(JID("test@rooms.swift.im"));
			testling->joinAs("Test");
			Presence::ref serverRespondsLocked = boost::make_shared<Presence>();
			serverRespondsLocked->setFrom(JID("test@rooms.swift.im/Test"));
			serverRespondsLocked->setTo(JID("test@swift.im/6913d576d55f0b67"));
			serverRespondsLocked->addPayload(vcard);
			serverRespondsLocked->addPayload(caps);
			serverRespondsLocked->setStatus("");
			MUCUserPayload::ref mucPayload(new MUCUserPayload());
			MUCItem myItem;
			myItem.affiliation = MUCOccupant::Owner;
			myItem.role = MUCOccupant::Moderator;
			mucPayload->addItem(myItem);
			mucPayload->addStatusCode(MUCUserPayload::StatusCode(201));
			serverRespondsLocked->addPayload(mucPayload);
			channel->onPresenceReceived(serverRespondsLocked);
			CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(channel->sentStanzas.size()));
			IQ::ref iq = channel->getStanzaAtIndex<IQ>(2);
			CPPUNIT_ASSERT(iq);
			CPPUNIT_ASSERT(iq->getPayload<MUCOwnerPayload>());
			CPPUNIT_ASSERT(iq->getPayload<MUCOwnerPayload>()->getForm());
			CPPUNIT_ASSERT_EQUAL(Form::SubmitType, iq->getPayload<MUCOwnerPayload>()->getForm()->getType());
		}

		/*void testJoin_Success() {
			MUC::ref testling = createMUC(JID("foo@bar.com"));
			testling->onJoinFinished.connect(boost::bind(&MUCTest::handleJoinFinished, this, _1, _2));
			testling->joinAs("Alice");
			receivePresence(JID("foo@bar.com/Rabbit"), "Here");

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(joinResults.size()));
			CPPUNIT_ASSERT_EQUAL(std::string("Alice"), joinResults[0].nick);
			CPPUNIT_ASSERT(joinResults[0].error);
		}

		void testJoin_Fail() {
			//CPPUNIT_ASSERT(!mucRegistry->isMUC(JID("foo@bar.com")));
		}*/

	private:
		MUC::ref createMUC(const JID& jid) {
			return boost::make_shared<MUC>(channel, router, presenceSender, jid, mucRegistry);
		}

		void handleJoinFinished(const std::string& nick, ErrorPayload::ref error) {
			JoinResult r;
			r.nick = nick;
			r.error = error;
			joinResults.push_back(r);
		}

		void receivePresence(const JID& jid, const std::string& status) {
			Presence::ref p = Presence::create(status);
			p->setFrom(jid);
			//MUCUserPayload::ref mucUserPayload = boost::make_shared<MUCUserPayload>();
			//mucUserPayload->addItem(item);
			//p->addPayload(mucUserPayload);
			channel->onPresenceReceived(p);
		}

	private:
		DummyStanzaChannel* channel;
		IQRouter* router;
		MUCRegistry* mucRegistry;
		StanzaChannelPresenceSender* stanzaChannelPresenceSender;
		DirectedPresenceSender* presenceSender;
		struct JoinResult {
			std::string nick;
			ErrorPayload::ref error;
		};
		std::vector<JoinResult> joinResults;
};

CPPUNIT_TEST_SUITE_REGISTRATION(MUCTest);
