#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>
#include <map>

#include "Swiften/LinkLocal/LinkLocalServiceBrowser.h"
#include "Swiften/LinkLocal/LinkLocalService.h"
#include "Swiften/LinkLocal/UnitTest/MockDNSSDService.h"
#include "Swiften/LinkLocal/DNSSDService.h"
#include "Swiften/EventLoop/DummyEventLoop.h"

// Test IP address change

using namespace Swift;

class LinkLocalServiceBrowserTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LinkLocalServiceBrowserTest);
		CPPUNIT_TEST(testServiceAdded);
		CPPUNIT_TEST(testServiceAdded_NoServiceInfo);
		CPPUNIT_TEST(testServiceChanged);
		CPPUNIT_TEST_SUITE_END();

	public:
		LinkLocalServiceBrowserTest() {}

		void setUp() {
			eventLoop = new DummyEventLoop();
			dnsSDService = boost::shared_ptr<MockDNSSDService>(new MockDNSSDService());
			testServiceID = new LinkLocalServiceID("foo", "bar.local");
			testServiceInfo = new DNSSDService::ResolveResult("xmpp.bar.local", 1234, LinkLocalServiceInfo());
			testServiceInfo2 = new DNSSDService::ResolveResult("xmpp.foo.local", 2345, LinkLocalServiceInfo());
		}

		void tearDown() {
			delete testServiceInfo2;
			delete testServiceInfo;
			delete testServiceID;
			delete eventLoop;
		}

		void testServiceAdded() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();

			dnsSDService->setServiceInfo(*testServiceID,*testServiceInfo);
			dnsSDService->addService(*testServiceID);
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(addedServices.size()));
			CPPUNIT_ASSERT(addedServices[0] == *testServiceID);
			// TODO: Check getServices
		}

		void testServiceAdded_NoServiceInfo() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();

			dnsSDService->addService(*testServiceID);
			eventLoop->processEvents();

			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(addedServices.size()));
		}

		void testServiceChanged() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling = createTestling();
			dnsSDService->setServiceInfo(*testServiceID,*testServiceInfo);
			dnsSDService->addService(*testServiceID);
			eventLoop->processEvents();

			dnsSDService->setServiceInfo(*testServiceID,*testServiceInfo2);
			eventLoop->processEvents();
			// TODO: Check getServices
		}

	private:
		boost::shared_ptr<LinkLocalServiceBrowser> createTestling() {
			boost::shared_ptr<LinkLocalServiceBrowser> testling(
					new LinkLocalServiceBrowser(dnsSDService));
			testling->onServiceAdded.connect(boost::bind(
					&LinkLocalServiceBrowserTest::handleServiceAdded, this, _1));
			return testling;
		}

		void handleServiceAdded(const LinkLocalServiceID& service) {
			addedServices.push_back(service);
		}

	private:
		DummyEventLoop* eventLoop;
		boost::shared_ptr<MockDNSSDService> dnsSDService;
		std::vector<LinkLocalServiceID> addedServices;
		LinkLocalServiceID* testServiceID;
		DNSSDService::ResolveResult* testServiceInfo;
		DNSSDService::ResolveResult* testServiceInfo2;
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkLocalServiceBrowserTest);
