#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>
#include <map>

#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Elements/RosterItemPayload.h"
#include "Slimber/LinkLocalPresenceManager.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/LinkLocal/LinkLocalServiceBrowser.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDServiceID.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h"
#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuerier.h"
#include "Swiften/EventLoop/DummyEventLoop.h"

using namespace Swift;

class LinkLocalPresenceManagerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LinkLocalPresenceManagerTest);
		CPPUNIT_TEST(testServiceAdded);
		CPPUNIT_TEST(testServiceRemoved);
		CPPUNIT_TEST(testServiceChanged);
		CPPUNIT_TEST(testGetRoster);
		CPPUNIT_TEST(testGetAllPresence);
		CPPUNIT_TEST(testGetRoster_InfoWithNick);
		CPPUNIT_TEST(testGetRoster_InfoWithFirstName);
		CPPUNIT_TEST(testGetRoster_InfoWithLastName);
		CPPUNIT_TEST(testGetRoster_InfoWithFirstAndLastName);
		CPPUNIT_TEST(testGetRoster_NoInfo);
		CPPUNIT_TEST_SUITE_END();

	public:
		LinkLocalPresenceManagerTest() {}

		void setUp() {
			eventLoop = new DummyEventLoop();
			querier = boost::shared_ptr<FakeDNSSDQuerier>(new FakeDNSSDQuerier("wonderland.lit"));
			browser = new LinkLocalServiceBrowser(querier);
			browser->start();
		}

		void tearDown() {
			browser->stop();
			delete browser;
			delete eventLoop;
		}

		void testServiceAdded() {
			std::auto_ptr<LinkLocalPresenceManager> testling(createTestling());

			addService("alice@wonderland", "Alice");

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(rosterChanges.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(rosterChanges[0]->getItems().size()));
			boost::optional<RosterItemPayload> item = rosterChanges[0]->getItem(JID("alice@wonderland"));
			CPPUNIT_ASSERT(item);
			CPPUNIT_ASSERT_EQUAL(String("Alice"), item->getName());
			CPPUNIT_ASSERT_EQUAL(RosterItemPayload::Both, item->getSubscription());
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(presenceChanges.size()));
			CPPUNIT_ASSERT(StatusShow::Online == presenceChanges[0]->getShow());
			CPPUNIT_ASSERT(JID("alice@wonderland") == presenceChanges[0]->getFrom());
		}

		void testServiceRemoved() {
			std::auto_ptr<LinkLocalPresenceManager> testling(createTestling());

			addService("alice@wonderland");
			removeService("alice@wonderland");

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(rosterChanges.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(rosterChanges[1]->getItems().size()));
			boost::optional<RosterItemPayload> item = rosterChanges[1]->getItem(JID("alice@wonderland"));
			CPPUNIT_ASSERT(item);
			CPPUNIT_ASSERT_EQUAL(RosterItemPayload::Remove, item->getSubscription());
		}

		void testServiceChanged() {
			std::auto_ptr<LinkLocalPresenceManager> testling(createTestling());

			addService("alice@wonderland");
			updateServicePresence("alice@wonderland", LinkLocalServiceInfo::Away, "I'm Away");

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(rosterChanges.size()));
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(presenceChanges.size()));
			CPPUNIT_ASSERT(StatusShow::Away == presenceChanges[1]->getShow());
			CPPUNIT_ASSERT(JID("alice@wonderland") == presenceChanges[1]->getFrom());
			CPPUNIT_ASSERT_EQUAL(String("I'm Away"), presenceChanges[1]->getStatus());
		}

		void testGetAllPresence() {
			std::auto_ptr<LinkLocalPresenceManager> testling(createTestling());

			addService("alice@wonderland");
			addService("rabbit@teaparty");
			updateServicePresence("rabbit@teaparty", LinkLocalServiceInfo::Away, "Partying");
			
			std::vector<boost::shared_ptr<Presence> > presences = testling->getAllPresence();
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(presences.size()));
			// The order doesn't matter
			CPPUNIT_ASSERT(JID("rabbit@teaparty") == presences[0]->getFrom());
			CPPUNIT_ASSERT(StatusShow::Away == presences[0]->getShow());
			CPPUNIT_ASSERT(JID("alice@wonderland") == presences[1]->getFrom());
			CPPUNIT_ASSERT(StatusShow::Online == presences[1]->getShow());
		}

		void testGetRoster() {
			std::auto_ptr<LinkLocalPresenceManager> testling(createTestling());

			addService("alice@wonderland", "Alice");
			addService("rabbit@teaparty", "Rabbit");

			boost::shared_ptr<RosterPayload> roster = testling->getRoster();
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(roster->getItems().size()));
			boost::optional<RosterItemPayload> item;
			item = roster->getItem(JID("alice@wonderland"));
			CPPUNIT_ASSERT(item);
			CPPUNIT_ASSERT_EQUAL(String("Alice"), item->getName());
			CPPUNIT_ASSERT_EQUAL(RosterItemPayload::Both, item->getSubscription());
			item = roster->getItem(JID("rabbit@teaparty"));
			CPPUNIT_ASSERT(item);
			CPPUNIT_ASSERT_EQUAL(String("Rabbit"), item->getName());
			CPPUNIT_ASSERT_EQUAL(RosterItemPayload::Both, item->getSubscription());
		}

		void testGetRoster_InfoWithNick() {
			std::auto_ptr<LinkLocalPresenceManager> testling(createTestling());

			addService("alice@wonderland", "Alice", "Alice In", "Wonderland");

			boost::optional<RosterItemPayload> item = testling->getRoster()->getItem(JID("alice@wonderland"));
			CPPUNIT_ASSERT_EQUAL(String("Alice"), item->getName());
		}

		void testGetRoster_InfoWithFirstName() {
			std::auto_ptr<LinkLocalPresenceManager> testling(createTestling());

			addService("alice@wonderland", "", "Alice In", "");

			boost::optional<RosterItemPayload> item = testling->getRoster()->getItem(JID("alice@wonderland"));
			CPPUNIT_ASSERT_EQUAL(String("Alice In"), item->getName());
		}

		void testGetRoster_InfoWithLastName() {
			std::auto_ptr<LinkLocalPresenceManager> testling(createTestling());

			addService("alice@wonderland", "", "", "Wonderland");

			boost::optional<RosterItemPayload> item = testling->getRoster()->getItem(JID("alice@wonderland"));
			CPPUNIT_ASSERT_EQUAL(String("Wonderland"), item->getName());
		}

		void testGetRoster_InfoWithFirstAndLastName() {
			std::auto_ptr<LinkLocalPresenceManager> testling(createTestling());

			addService("alice@wonderland", "", "Alice In", "Wonderland");

			boost::optional<RosterItemPayload> item = testling->getRoster()->getItem(JID("alice@wonderland"));
			CPPUNIT_ASSERT_EQUAL(String("Alice In Wonderland"), item->getName());
		}

		void testGetRoster_NoInfo() {
			std::auto_ptr<LinkLocalPresenceManager> testling(createTestling());

			addService("alice@wonderland");

			boost::optional<RosterItemPayload> item = testling->getRoster()->getItem(JID("alice@wonderland"));
			CPPUNIT_ASSERT_EQUAL(String(""), item->getName());
		}

	private:
		std::auto_ptr<LinkLocalPresenceManager> createTestling() {
			std::auto_ptr<LinkLocalPresenceManager> testling(
					new LinkLocalPresenceManager(browser));
			testling->onRosterChanged.connect(boost::bind(
					&LinkLocalPresenceManagerTest::handleRosterChanged, this, _1));
			testling->onPresenceChanged.connect(boost::bind(
					&LinkLocalPresenceManagerTest::handlePresenceChanged, this, _1));
			return testling;
		}

		void addService(const String& name, const String& nickName = String(), const String& firstName = String(), const String& lastName = String()) {
			DNSSDServiceID service(name, "local.");
			LinkLocalServiceInfo info;
			info.setFirstName(firstName);
			info.setLastName(lastName);
			info.setNick(nickName);
			querier->setServiceInfo(service, DNSSDResolveServiceQuery::Result(name + "._presence._tcp.local", "rabbithole.local", 1234, info.toTXTRecord()));
			querier->addService(service);
			eventLoop->processEvents();
		}

		void removeService(const String& name) {
			DNSSDServiceID service(name, "local.");
			querier->removeService(DNSSDServiceID(name, "local."));
			eventLoop->processEvents();
		}

		void updateServicePresence(const String& name, LinkLocalServiceInfo::Status status, const String& message) {
			DNSSDServiceID service(name, "local.");
			LinkLocalServiceInfo info;
			info.setStatus(status);
			info.setMessage(message);
			querier->setServiceInfo(service, DNSSDResolveServiceQuery::Result(name + "._presence._tcp.local", "rabbithole.local", 1234, info.toTXTRecord()));
			eventLoop->processEvents();
		}

		void handleRosterChanged(boost::shared_ptr<RosterPayload> payload) {
			rosterChanges.push_back(payload);
		}

		void handlePresenceChanged(boost::shared_ptr<Presence> presence) {
			presenceChanges.push_back(presence);
		}

	private:
		DummyEventLoop* eventLoop;
		boost::shared_ptr<FakeDNSSDQuerier> querier;
		LinkLocalServiceBrowser* browser;
		std::vector< boost::shared_ptr<RosterPayload> > rosterChanges;
		std::vector< boost::shared_ptr<Presence> > presenceChanges;
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkLocalPresenceManagerTest);
