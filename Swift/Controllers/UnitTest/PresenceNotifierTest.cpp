/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <vector>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Avatars/DummyAvatarManager.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Network/DummyTimerFactory.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Roster/XMPPRosterImpl.h>

#include <Swift/Controllers/PresenceNotifier.h>

#include <SwifTools/Notifier/LoggingNotifier.h>

using namespace Swift;

class PresenceNotifierTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(PresenceNotifierTest);
        CPPUNIT_TEST(testReceiveFirstPresenceCreatesAvailableNotification);
        CPPUNIT_TEST(testReceiveSecondPresenceCreatesStatusChangeNotification);
        CPPUNIT_TEST(testReceiveUnavailablePresenceAfterAvailablePresenceCreatesUnavailableNotification);
        CPPUNIT_TEST(testReceiveUnavailablePresenceWithoutAvailableDoesNotCreateNotification);
        CPPUNIT_TEST(testReceiveAvailablePresenceAfterUnavailableCreatesAvailableNotification);
        CPPUNIT_TEST(testReceiveAvailablePresenceAfterReconnectCreatesAvailableNotification);
        CPPUNIT_TEST(testReceiveAvailablePresenceFromMUCDoesNotCreateNotification);
        CPPUNIT_TEST(testNotificationSubjectContainsNameForJIDInRoster);
        CPPUNIT_TEST(testNotificationSubjectContainsJIDForJIDNotInRoster);
        CPPUNIT_TEST(testNotificationSubjectContainsStatus);
        CPPUNIT_TEST(testNotificationMessageContainsStatusMessage);
        CPPUNIT_TEST(testNotificationPicture);
        CPPUNIT_TEST(testNotificationActivationEmitsSignal);
        CPPUNIT_TEST(testReceiveFirstPresenceWithQuietPeriodDoesNotNotify);
        CPPUNIT_TEST(testReceiveFirstPresenceWithQuietPeriodDoesNotCountAsQuietPeriod);
        CPPUNIT_TEST(testReceivePresenceDuringQuietPeriodDoesNotNotify);
        CPPUNIT_TEST(testReceivePresenceDuringQuietPeriodResetsTimer);
        CPPUNIT_TEST(testReceivePresenceAfterQuietPeriodNotifies);
        CPPUNIT_TEST(testReceiveFirstPresenceAfterReconnectWithQuietPeriodDoesNotNotify);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            stanzaChannel = new DummyStanzaChannel();
            notifier = new LoggingNotifier();
            mucRegistry = new MUCRegistry();
            user1 = JID("user1@bar.com/bla");
            user2 = JID("user2@foo.com/baz");
            avatarManager = new DummyAvatarManager();
            roster = new XMPPRosterImpl();
            nickResolver = new NickResolver(JID("foo@bar.com"), roster, nullptr, mucRegistry);
            presenceOracle = new PresenceOracle(stanzaChannel, roster);
            timerFactory = new DummyTimerFactory();
        }

        void tearDown() {
            delete timerFactory;
            delete presenceOracle;
            delete nickResolver;
            delete roster;
            delete avatarManager;
            delete mucRegistry;
            delete notifier;
            delete stanzaChannel;
        }

        void testReceiveFirstPresenceCreatesAvailableNotification() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();

            sendPresence(user1, StatusShow::Online);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(notifier->notifications.size()));
            CPPUNIT_ASSERT_EQUAL(Notifier::ContactAvailable, notifier->notifications[0].type);
        }

        void testReceiveSecondPresenceCreatesStatusChangeNotification() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            sendPresence(user1, StatusShow::Away);
            notifier->notifications.clear();

            sendPresence(user1, StatusShow::Online);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(notifier->notifications.size()));
            CPPUNIT_ASSERT_EQUAL(Notifier::ContactStatusChange, notifier->notifications[0].type);
        }

        void testReceiveUnavailablePresenceAfterAvailablePresenceCreatesUnavailableNotification() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            sendPresence(user1, StatusShow::Away);
            notifier->notifications.clear();

            sendUnavailablePresence(user1);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(notifier->notifications.size()));
            CPPUNIT_ASSERT_EQUAL(Notifier::ContactUnavailable, notifier->notifications[0].type);
        }

        void testReceiveUnavailablePresenceWithoutAvailableDoesNotCreateNotification() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();

            sendUnavailablePresence(user1);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(notifier->notifications.size()));
        }

        void testReceiveAvailablePresenceAfterUnavailableCreatesAvailableNotification() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            sendPresence(user1, StatusShow::Away);
            sendUnavailablePresence(user1);
            notifier->notifications.clear();

            sendPresence(user1, StatusShow::Away);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(notifier->notifications.size()));
            CPPUNIT_ASSERT_EQUAL(Notifier::ContactAvailable, notifier->notifications[0].type);
        }

        void testReceiveAvailablePresenceAfterReconnectCreatesAvailableNotification() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            sendPresence(user1, StatusShow::Away);
            stanzaChannel->setAvailable(false);
            stanzaChannel->setAvailable(true);
            notifier->notifications.clear();

            sendPresence(user1, StatusShow::Away);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(notifier->notifications.size()));
            CPPUNIT_ASSERT_EQUAL(Notifier::ContactAvailable, notifier->notifications[0].type);
        }

        void testReceiveAvailablePresenceFromMUCDoesNotCreateNotification() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            mucRegistry->addMUC(JID("teaparty@wonderland.lit"));

            sendPresence(JID("teaparty@wonderland.lit/Alice"), StatusShow::Away);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(notifier->notifications.size()));
        }

        void testNotificationPicture() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            avatarManager->avatars[user1] = createByteArray("abcdef");

            sendPresence(user1, StatusShow::Online);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(notifier->notifications.size()));
            CPPUNIT_ASSERT_EQUAL(boost::filesystem::path("/avatars/user1@bar.com/bla"), notifier->notifications[0].picture);
        }

        void testNotificationActivationEmitsSignal() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();

            sendPresence(user1, StatusShow::Online);
            CPPUNIT_ASSERT(notifier->notifications[0].callback);
            notifier->notifications[0].callback();

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(activatedNotifications.size()));
            CPPUNIT_ASSERT_EQUAL(user1, activatedNotifications[0]);
        }

        void testNotificationSubjectContainsNameForJIDInRoster() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            roster->addContact(user1.toBare(), "User 1", std::vector<std::string>(), RosterItemPayload::Both);

            sendPresence(user1, StatusShow::Online);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(notifier->notifications.size()));
            std::string subject = notifier->notifications[0].subject;
            CPPUNIT_ASSERT(subject.find("User 1") != std::string::npos);
        }

        void testNotificationSubjectContainsJIDForJIDNotInRoster() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();

            sendPresence(user1, StatusShow::Online);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(notifier->notifications.size()));
            std::string subject = notifier->notifications[0].subject;
            CPPUNIT_ASSERT(subject.find(user1.toBare().toString()) != std::string::npos);
        }

        void testNotificationSubjectContainsStatus() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();

            sendPresence(user1, StatusShow::Away);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(notifier->notifications.size()));
            std::string subject = notifier->notifications[0].subject;
            CPPUNIT_ASSERT(subject.find("Away") != std::string::npos);
        }

        void testNotificationMessageContainsStatusMessage() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();

            sendPresence(user1, StatusShow::Away);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(notifier->notifications.size()));
            CPPUNIT_ASSERT(notifier->notifications[0].description.find("Status Message") != std::string::npos);
        }

        void testReceiveFirstPresenceWithQuietPeriodDoesNotNotify() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            testling->setInitialQuietPeriodMS(10);

            sendPresence(user1, StatusShow::Online);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(notifier->notifications.size()));
        }

        void testReceivePresenceDuringQuietPeriodDoesNotNotify() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            testling->setInitialQuietPeriodMS(10);

            sendPresence(user1, StatusShow::Online);
            timerFactory->setTime(1);
            sendPresence(user2, StatusShow::Away);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(notifier->notifications.size()));
        }

        void testReceivePresenceDuringQuietPeriodResetsTimer() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            testling->setInitialQuietPeriodMS(10);

            sendPresence(user1, StatusShow::Online);
            timerFactory->setTime(9);
            sendPresence(user2, StatusShow::Away);
            timerFactory->setTime(18);
            sendPresence(user1, StatusShow::Away);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(notifier->notifications.size()));
        }

        void testReceivePresenceAfterQuietPeriodNotifies() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            testling->setInitialQuietPeriodMS(10);

            sendPresence(user1, StatusShow::Online);
            timerFactory->setTime(11);
            sendPresence(user2, StatusShow::Away);

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(notifier->notifications.size()));
        }

        void testReceiveFirstPresenceWithQuietPeriodDoesNotCountAsQuietPeriod() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            testling->setInitialQuietPeriodMS(10);

            timerFactory->setTime(11);
            sendPresence(user1, StatusShow::Away);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(notifier->notifications.size()));
        }

        void testReceiveFirstPresenceAfterReconnectWithQuietPeriodDoesNotNotify() {
            std::shared_ptr<PresenceNotifier> testling = createNotifier();
            testling->setInitialQuietPeriodMS(10);
            sendPresence(user1, StatusShow::Online);
            timerFactory->setTime(15);
            notifier->notifications.clear();

            stanzaChannel->setAvailable(false);
            stanzaChannel->setAvailable(true);
            sendPresence(user1, StatusShow::Online);
            timerFactory->setTime(21);
            sendPresence(user2, StatusShow::Online);

            CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(notifier->notifications.size()));
        }


    private:
        std::shared_ptr<PresenceNotifier> createNotifier() {
            std::shared_ptr<PresenceNotifier> result(new PresenceNotifier(stanzaChannel, notifier, mucRegistry, avatarManager, nickResolver, presenceOracle, timerFactory));
            result->onNotificationActivated.connect(boost::bind(&PresenceNotifierTest::handleNotificationActivated, this, _1));
            result->setInitialQuietPeriodMS(0);
            return result;
        }

        void sendPresence(const JID& jid, StatusShow::Type type) {
            std::shared_ptr<Presence> presence(new Presence());
            presence->setFrom(jid);
            presence->setShow(type);
            presence->setStatus("Status Message");
            stanzaChannel->onPresenceReceived(presence);
        }

        void sendUnavailablePresence(const JID& jid) {
            std::shared_ptr<Presence> presence(new Presence());
            presence->setType(Presence::Unavailable);
            presence->setFrom(jid);
            stanzaChannel->onPresenceReceived(presence);
        }

        void handleNotificationActivated(const JID& j) {
            activatedNotifications.push_back(j);
        }

    private:
        DummyStanzaChannel* stanzaChannel;
        LoggingNotifier* notifier;
        MUCRegistry* mucRegistry;
        DummyAvatarManager* avatarManager;
        XMPPRosterImpl* roster;
        NickResolver* nickResolver;
        PresenceOracle* presenceOracle;
        DummyTimerFactory* timerFactory;
        JID user1;
        JID user2;
        std::vector<JID> activatedNotifications;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PresenceNotifierTest);
