#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>
#include <map>

#include "Swiften/LinkLocal/LinkLocalServiceBrowser.h"
#include "Swiften/LinkLocal/LinkLocalService.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDServiceID.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h"
#include "Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuerier.h"
#include "Swiften/EventLoop/DummyEventLoop.h"

using namespace Swift;

class LinkLocalServiceBrowserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LinkLocalServiceBrowserTest);
		CPPUNIT_TEST(testConstructor);
		CPPUNIT_TEST(testStart);
		CPPUNIT_TEST(testServiceAdded);
		CPPUNIT_TEST(testServiceAdded_NoServiceInfo);
		CPPUNIT_TEST(testServiceAdded_RegisteredService);
		CPPUNIT_TEST(testServiceAdded_UnregisteredService);
		CPPUNIT_TEST(testServiceChanged);
		CPPUNIT_TEST(testServiceRemoved);
		CPPUNIT_TEST(testError_BrowseErrorAfterStart);
		CPPUNIT_TEST(testError_BrowseErrorAfterResolve);
		CPPUNIT_TEST(testRegisterService);
		CPPUNIT_TEST(testRegisterService_Error);
		CPPUNIT_TEST(testRegisterService_Reregister);
		CPPUNIT_TEST_SUITE_END();

	public:
		LinkLocalServiceBrowserTest() {}

		void setUp() {
			eventLoop = new DummyEventLoop();
			querier = boost::shared_ptr<FakeDNSSDQuerier>(new FakeDNSSDQuerier("wonderland.lit"));
			aliceServiceID = new DNSSDServiceID("alice", "wonderland.lit");
			aliceServiceInfo = new DNSSDResolveServiceQuery::Result("_presence._tcp.wonderland.lit", "xmpp.wonderland.lit", 1234, LinkLocalServiceInfo().toTXTRecord());
			testServiceID = new DNSSDServiceID("foo", "bar.local");
			testServiceInfo = new DNSSDResolveServiceQuery::Result("_presence._tcp.bar.local", "xmpp.bar.local", 1234, LinkLocalServiceInfo().toTXTRecord());
			testServiceInfo2 = new DNSSDResolveServiceQuery::Result("_presence.tcp.bar.local", "xmpp.foo.local", 2345, LinkLocalServiceInfo().toTXTRecord());
			errorStopReceived = false;
			normalStopReceived = false;
		}

		void tearDown() {
			addedServices.clear();
			removedServices.clear();
			changedServices.clear();

			delete aliceServiceID;
			delete aliceServiceInfo;
			delete testServiceInfo2;
			delete testServiceInfo;
			delete testServiceID;
			delete eventLoop;
		}

		void testConstructor() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();

			CPPUNIT_ASSERT(!testling->isRunning());
			CPPUNIT_ASSERT(!testling->hasError());
		}

		void testStart() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();

			CPPUNIT_ASSERT(testling->isRunning());
			CPPUNIT_ASSERT(!testling->hasError());

			testling->stop();
		}

		void testServiceAdded() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();
			eventLoop->processEvents();

			querier->setServiceInfo(*testServiceID,*testServiceInfo);
			querier->addService(*testServiceID);
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(addedServices.size()));
			CPPUNIT_ASSERT(addedServices[0] == *testServiceID);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changedServices.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(removedServices.size()));
			std::vector<LinkLocalService> services = testling->getServices();
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(services.size()));
			CPPUNIT_ASSERT(*testServiceID == services[0].getID());
			CPPUNIT_ASSERT(testServiceInfo->port == services[0].getPort());
			CPPUNIT_ASSERT(testServiceInfo->host == services[0].getHostname());

			testling->stop();
		}

		void testServiceAdded_NoServiceInfo() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();
			eventLoop->processEvents();

			querier->addService(*testServiceID);
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(addedServices.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling->getServices().size()));

			testling->stop();
		}

		void testServiceAdded_RegisteredService() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();
			eventLoop->processEvents();

			testling->registerService("alice", 1234, LinkLocalServiceInfo());
			eventLoop->processEvents();
			querier->setServiceInfo(*aliceServiceID, *aliceServiceInfo);
			querier->addService(*aliceServiceID);
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(addedServices.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling->getServices().size()));

			testling->stop();
		}

		void testServiceAdded_UnregisteredService() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();
			eventLoop->processEvents();
			testling->registerService("alice", 1234, LinkLocalServiceInfo());
			eventLoop->processEvents();
			testling->unregisterService();
			eventLoop->processEvents();

			querier->setServiceInfo(*aliceServiceID, *aliceServiceInfo);
			querier->addService(*aliceServiceID);
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(addedServices.size()));
			CPPUNIT_ASSERT(addedServices[0] == *aliceServiceID);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changedServices.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(removedServices.size()));
			std::vector<LinkLocalService> services = testling->getServices();
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(services.size()));
			CPPUNIT_ASSERT(*aliceServiceID == services[0].getID());
			CPPUNIT_ASSERT(aliceServiceInfo->port == services[0].getPort());
			CPPUNIT_ASSERT(aliceServiceInfo->host == services[0].getHostname());

			testling->stop();
		}

		void testServiceChanged() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();
			querier->setServiceInfo(*testServiceID,*testServiceInfo);
			querier->addService(*testServiceID);
			eventLoop->processEvents();

			querier->setServiceInfo(*testServiceID,*testServiceInfo2);
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(addedServices.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(changedServices.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(removedServices.size()));
			CPPUNIT_ASSERT(changedServices[0] == *testServiceID);
			std::vector<LinkLocalService> services = testling->getServices();
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(services.size()));
			CPPUNIT_ASSERT(*testServiceID == services[0].getID());
			CPPUNIT_ASSERT(testServiceInfo2->port == services[0].getPort());
			CPPUNIT_ASSERT(testServiceInfo2->host == services[0].getHostname());

			testling->stop();
		}

		void testServiceRemoved() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();
			querier->setServiceInfo(*testServiceID,*testServiceInfo);
			querier->addService(*testServiceID);
			eventLoop->processEvents();

			querier->removeService(*testServiceID);
			eventLoop->processEvents();
			querier->setServiceInfo(*testServiceID,*testServiceInfo2);
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(addedServices.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changedServices.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(removedServices.size()));
			CPPUNIT_ASSERT(removedServices[0] == *testServiceID);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling->getServices().size()));

			testling->stop();
		}

		void testError_BrowseErrorAfterStart() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();

			querier->setBrowseError();
			eventLoop->processEvents();

			CPPUNIT_ASSERT(!testling->isRunning());
			CPPUNIT_ASSERT(testling->hasError());
			CPPUNIT_ASSERT(errorStopReceived);
		}

		void testError_BrowseErrorAfterResolve() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();
			querier->setServiceInfo(*testServiceID,*testServiceInfo);
			querier->addService(*testServiceID);
			eventLoop->processEvents();

			querier->setBrowseError();
			eventLoop->processEvents();
			querier->setServiceInfo(*testServiceID,*testServiceInfo2);
			eventLoop->processEvents();

			CPPUNIT_ASSERT(!testling->isRunning());
			CPPUNIT_ASSERT(testling->hasError());
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(testling->getServices().size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changedServices.size()));
			CPPUNIT_ASSERT(errorStopReceived);
		}

		void testRegisterService() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();
			eventLoop->processEvents();

			LinkLocalServiceInfo info;
			info.setFirstName("Foo");
			testling->registerService("foo@bar", 1234, info);
			eventLoop->processEvents();

			CPPUNIT_ASSERT(querier->isServiceRegistered("foo@bar", 1234, info.toTXTRecord()));

			testling->stop();
		}

		void testRegisterService_Error() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();
			LinkLocalServiceInfo info;
			testling->registerService("foo@bar", 1234, info);
			eventLoop->processEvents();

			querier->setRegisterError();
			eventLoop->processEvents();

			CPPUNIT_ASSERT(!testling->isRunning());
			CPPUNIT_ASSERT(testling->hasError());
			CPPUNIT_ASSERT(errorStopReceived);
			CPPUNIT_ASSERT(!querier->isServiceRegistered("foo@bar", 1234, info.toTXTRecord()));
		}

		void testRegisterService_Reregister() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			testling->start();
			eventLoop->processEvents();
			LinkLocalServiceInfo info;
			info.setFirstName("Foo");
			testling->registerService("foo@bar", 1234, info);
			eventLoop->processEvents();
			testling->unregisterService();
			eventLoop->processEvents();

			info.setFirstName("Bar");
			testling->registerService("bar@baz", 3456, info);
			eventLoop->processEvents();

			CPPUNIT_ASSERT(querier->isServiceRegistered("bar@baz", 3456, info.toTXTRecord()));

			testling->stop();
		}

	private:
		boost::shared_ptr<LinkLocalServiceBrowser> createTestling() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling(
					new LinkLocalServiceBrowser(querier));
			testling->onServiceAdded.connect(boost::bind(
					&LinkLocalServiceBrowserTest::handleServiceAdded, this, _1));
			testling->onServiceChanged.connect(boost::bind(
					&LinkLocalServiceBrowserTest::handleServiceChanged, this, _1));
			testling->onServiceRemoved.connect(boost::bind(
					&LinkLocalServiceBrowserTest::handleServiceRemoved, this, _1));
			testling->onStopped.connect(boost::bind(
					&LinkLocalServiceBrowserTest::handleStopped, this, _1));
			return testling;
		}

		void handleServiceAdded(const DNSSDServiceID& service) {
			addedServices.push_back(service);
		}

		void handleServiceRemoved(const DNSSDServiceID& service) {
			removedServices.push_back(service);
		}

		void handleServiceChanged(const DNSSDServiceID& service) {
			changedServices.push_back(service);
		}

		void handleStopped(bool error) {
			CPPUNIT_ASSERT(!errorStopReceived);
			CPPUNIT_ASSERT(!normalStopReceived);
			if (error) {
				errorStopReceived = true;
			}
			else {
				normalStopReceived = true;
			}
		}

	private:
		DummyEventLoop* eventLoop;
		boost::shared_ptr<FakeDNSSDQuerier> querier;
		std::vector<DNSSDServiceID> addedServices;
		std::vector<DNSSDServiceID> changedServices;
		std::vector<DNSSDServiceID> removedServices;
		DNSSDServiceID* aliceServiceID;
		DNSSDResolveServiceQuery::Result* aliceServiceInfo;
		DNSSDServiceID* testServiceID;
		DNSSDResolveServiceQuery::Result* testServiceInfo;
		DNSSDResolveServiceQuery::Result* testServiceInfo2;
		bool errorStopReceived;
		bool normalStopReceived;
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkLocalServiceBrowserTest);
