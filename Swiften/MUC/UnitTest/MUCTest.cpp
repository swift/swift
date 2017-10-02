/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Elements/CapsInfo.h>
#include <Swiften/Elements/MUCOwnerPayload.h>
#include <Swiften/Elements/MUCUserPayload.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/MUC/MUCImpl.h>
#include <Swiften/Presence/DirectedPresenceSender.h>
#include <Swiften/Presence/StanzaChannelPresenceSender.h>
#include <Swiften/Queries/IQRouter.h>

using namespace Swift;

class MUCTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(MUCTest);
        CPPUNIT_TEST(testJoin);
        CPPUNIT_TEST(testJoin_ChangePresenceDuringJoinDoesNotSendPresenceBeforeJoinSuccess);
        CPPUNIT_TEST(testJoin_ChangePresenceDuringJoinResendsPresenceAfterJoinSuccess);
        CPPUNIT_TEST(testJoin_NoPresenceChangeDuringJoinDoesNotResendAfterJoinSuccess);
        CPPUNIT_TEST(testCreateInstant);
        CPPUNIT_TEST(testReplicateBug);
        CPPUNIT_TEST(testNicknameChange);
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
            nickChanges = 0;
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

        void testJoin_NoPresenceChangeDuringJoinDoesNotResendAfterJoinSuccess() {
            MUC::ref testling = createMUC(JID("foo@bar.com"));
            testling->joinAs("Alice");

            receivePresence(JID("foo@bar.com/Rabbit"), "Here");

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel->sentStanzas.size()));
            Presence::ref p = channel->getStanzaAtIndex<Presence>(0);
            CPPUNIT_ASSERT(p);
            CPPUNIT_ASSERT_EQUAL(JID("foo@bar.com/Alice"), p->getTo());
            CPPUNIT_ASSERT_EQUAL(std::string(""), p->getStatus());
        }

        void testCreateInstant() {
            MUC::ref testling = createMUC(JID("rabbithole@wonderland.lit"));
            testling->joinAs("Alice");
            Presence::ref serverRespondsLocked = std::make_shared<Presence>();
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
            Presence::ref initialPresence = std::make_shared<Presence>();
            initialPresence->setStatus("");
            VCard::ref vcard = std::make_shared<VCard>();
            vcard->setPhoto(createByteArray("15c30080ae98ec48be94bf0e191d43edd06e500a"));
            initialPresence->addPayload(vcard);
            CapsInfo::ref caps = std::make_shared<CapsInfo>();
            caps->setNode("http://swift.im");
            caps->setVersion("p2UP0DrcVgKM6jJqYN/B92DKK0o=");
            initialPresence->addPayload(caps);
            channel->sendPresence(initialPresence);

            MUC::ref testling = createMUC(JID("test@rooms.swift.im"));
            testling->joinAs("Test");
            Presence::ref serverRespondsLocked = std::make_shared<Presence>();
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

        void testNicknameChange() {
            MUC::ref testling = createMUC(JID("foo@bar.com"));
            // Join as Rabbit
            testling->joinAs("Rabbit");

            // Rabbit joins
            Presence::ref rabbitJoins = std::make_shared<Presence>();
            rabbitJoins->setTo("test@swift.im/6913d576d55f0b67");
            rabbitJoins->setFrom(testling->getJID().toString() + "/Rabbit");
            channel->onPresenceReceived(rabbitJoins);
            CPPUNIT_ASSERT_EQUAL(true, testling->hasOccupant("Rabbit"));

            // Alice joins
            Presence::ref aliceJoins = std::make_shared<Presence>();
            aliceJoins->setTo("test@swift.im/6913d576d55f0b67");
            aliceJoins->setFrom(testling->getJID().toString() + "/Alice");
            channel->onPresenceReceived(aliceJoins);
            CPPUNIT_ASSERT_EQUAL(true, testling->hasOccupant("Alice"));

            // Change nick to Dodo
            testling->changeNickname("Dodo");
            Presence::ref stanza = channel->getStanzaAtIndex<Presence>(1);
            CPPUNIT_ASSERT(stanza);
            CPPUNIT_ASSERT_EQUAL(std::string("Dodo"), stanza->getTo().getResource());

            // Alice changes nick to Alice2
            stanza = std::make_shared<Presence>();
            stanza->setFrom(JID("foo@bar.com/Alice"));
            stanza->setTo(JID(router->getJID()));
            stanza->setType(Presence::Unavailable);
            MUCUserPayload::ref mucPayload(new MUCUserPayload());
            MUCItem myItem;
            myItem.affiliation = MUCOccupant::Member;
            myItem.nick = "Alice2";
            myItem.role = MUCOccupant::Participant;
            mucPayload->addItem(myItem);
            mucPayload->addStatusCode(303);
            stanza->addPayload(mucPayload);
            channel->onPresenceReceived(stanza);
            CPPUNIT_ASSERT_EQUAL(1, nickChanges);
            CPPUNIT_ASSERT_EQUAL(false, testling->hasOccupant("Alice"));
            CPPUNIT_ASSERT_EQUAL(true, testling->hasOccupant("Alice2"));

            // We (Rabbit) change nick to Robot
            stanza = std::make_shared<Presence>();
            stanza->setFrom(JID("foo@bar.com/Rabbit"));
            stanza->setTo(JID(router->getJID()));
            stanza->setType(Presence::Unavailable);
            mucPayload = MUCUserPayload::ref(new MUCUserPayload());
            myItem.affiliation = MUCOccupant::Member;
            myItem.nick = "Robot";
            myItem.role = MUCOccupant::Participant;
            mucPayload->addItem(myItem);
            mucPayload->addStatusCode(303);
            stanza->addPayload(mucPayload);
            channel->onPresenceReceived(stanza);
            CPPUNIT_ASSERT_EQUAL(2, nickChanges);
            CPPUNIT_ASSERT_EQUAL(false, testling->hasOccupant("Rabbit"));
            CPPUNIT_ASSERT_EQUAL(true, testling->hasOccupant("Robot"));
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
            MUC::ref muc = std::make_shared<MUCImpl>(channel, router, presenceSender, jid, mucRegistry);
            muc->onOccupantNicknameChanged.connect(boost::bind(&MUCTest::handleOccupantNicknameChanged, this, _1, _2));
            return muc;
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
            //MUCUserPayload::ref mucUserPayload = std::make_shared<MUCUserPayload>();
            //mucUserPayload->addItem(item);
            //p->addPayload(mucUserPayload);
            channel->onPresenceReceived(p);
        }

        void handleOccupantNicknameChanged(const std::string&, const std::string&) {
            nickChanges++;
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
        int nickChanges;
};

CPPUNIT_TEST_SUITE_REGISTRATION(MUCTest);
