#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>
#include <map>

#include "Swiften/LinkLocal/LinkLocalServiceBrowser.h"
#include "Swiften/LinkLocal/LinkLocalService.h"
#include "Swiften/LinkLocal/UnitTest/MockDNSSDService.h"
#include "Swiften/LinkLocal/DNSSDService.h"
#include "Swiften/EventLoop/DummyEventLoop.h"

// Test canCreate() == false

using namespace Swift;

class LinkLocalServiceBrowserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LinkLocalServiceBrowserTest);
		CPPUNIT_TEST(testServiceAdded);
		CPPUNIT_TEST(testServiceAdded_NoServiceInfo);
		CPPUNIT_TEST(testServiceChanged);
		CPPUNIT_TEST(testServiceRemoved);
		CPPUNIT_TEST_SUITE_END();

	public:
		LinkLocalServiceBrowserTest() {}

		void setUp() {
			eventLoop = new DummyEventLoop();
			dnsSDServiceFactory = new MockDNSSDServiceFactory();
			testServiceID = new LinkLocalServiceID("foo", "bar.local");
			testServiceInfo = new DNSSDService::ResolveResult("xmpp.bar.local", 1234, LinkLocalServiceInfo());
			testServiceInfo2 = new DNSSDService::ResolveResult("xmpp.foo.local", 2345, LinkLocalServiceInfo());
		}

		void tearDown() {
			addedServices.clear();
			removedServices.clear();
			changedServices.clear();

			delete testServiceInfo2;
			delete testServiceInfo;
			delete testServiceID;
      delete dnsSDServiceFactory;
			delete eventLoop;
		}

		void testServiceAdded() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();

			dnsSDService()->setServiceInfo(*testServiceID,*testServiceInfo);
			dnsSDService()->addService(*testServiceID);
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
		}

		void testServiceAdded_NoServiceInfo() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();

			dnsSDService()->addService(*testServiceID);
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(addedServices.size()));
			std::vector<LinkLocalService> services = testling->getServices();
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(services.size()));
		}

		void testServiceChanged() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			dnsSDService()->setServiceInfo(*testServiceID,*testServiceInfo);
			dnsSDService()->addService(*testServiceID);
			eventLoop->processEvents();

			dnsSDService()->setServiceInfo(*testServiceID,*testServiceInfo2);
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
		}

		void testServiceRemoved() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			dnsSDService()->setServiceInfo(*testServiceID,*testServiceInfo);
			dnsSDService()->addService(*testServiceID);
			eventLoop->processEvents();

			dnsSDService()->removeService(*testServiceID);
			eventLoop->processEvents();
			dnsSDService()->setServiceInfo(*testServiceID,*testServiceInfo2);
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(addedServices.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(changedServices.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(removedServices.size()));
			CPPUNIT_ASSERT(removedServices[0] == *testServiceID);
			std::vector<LinkLocalService> services = testling->getServices();
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(services.size()));
		}

	private:
		boost::shared_ptr<LinkLocalServiceBrowser> createTestling() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling(
					new LinkLocalServiceBrowser(dnsSDServiceFactory));
			testling->onServiceAdded.connect(boost::bind(
					&LinkLocalServiceBrowserTest::handleServiceAdded, this, _1));
			testling->onServiceChanged.connect(boost::bind(
					&LinkLocalServiceBrowserTest::handleServiceChanged, this, _1));
			testling->onServiceRemoved.connect(boost::bind(
					&LinkLocalServiceBrowserTest::handleServiceRemoved, this, _1));
			return testling;
		}

		void handleServiceAdded(const LinkLocalServiceID& service) {
			addedServices.push_back(service);
		}

		void handleServiceRemoved(const LinkLocalServiceID& service) {
			removedServices.push_back(service);
		}

		void handleServiceChanged(const LinkLocalServiceID& service) {
			changedServices.push_back(service);
		}

    boost::shared_ptr<MockDNSSDService> dnsSDService() const {
      CPPUNIT_ASSERT(dnsSDServiceFactory->services.size() > 0);
      return dnsSDServiceFactory->services[0];
    }

	private:
		DummyEventLoop* eventLoop;
		MockDNSSDServiceFactory* dnsSDServiceFactory;
		std::vector<LinkLocalServiceID> addedServices;
		std::vector<LinkLocalServiceID> changedServices;
		std::vector<LinkLocalServiceID> removedServices;
		LinkLocalServiceID* testServiceID;
		DNSSDService::ResolveResult* testServiceInfo;
		DNSSDService::ResolveResult* testServiceInfo2;
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkLocalServiceBrowserTest);
