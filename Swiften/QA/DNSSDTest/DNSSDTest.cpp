/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

// TODO: Test registering on different interfaces
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include <Swiften/Base/sleep.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDQuerier.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h>
#include <Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h>
#ifdef HAVE_AVAHI
#include <Swiften/LinkLocal/DNSSD/Avahi/AvahiQuerier.h>
#endif

#define SLEEP_INTERVALS 20

using namespace Swift;

template <typename DNSSDQuerierType>
class DNSSDTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(DNSSDTest);
		CPPUNIT_TEST(testPublish);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			eventLoop = new DummyEventLoop();
			querier = boost::shared_ptr<DNSSDQuerier>(new DNSSDQuerierType());
			querier->start();
		}

		void tearDown() {
			querier->stop();
			querier.reset();
			delete eventLoop;
		}

		void testPublish() {
			boost::shared_ptr<DNSSDBrowseQuery> browseQuery = querier->createBrowseQuery();
			browseQuery->onServiceAdded.connect(boost::bind(&DNSSDTest::handleServiceAdded, this, _1));
			browseQuery->onServiceRemoved.connect(boost::bind(&DNSSDTest::handleServiceRemoved, this, _1));
			browseQuery->onError.connect(boost::bind(&DNSSDTest::handleBrowseError, this));
			browseQuery->startBrowsing();
			eventLoop->processEvents();

			// Publish the service
			LinkLocalServiceInfo info;
			boost::shared_ptr<DNSSDRegisterQuery> registerQuery = querier->createRegisterQuery("DNSSDTest", 1234, info.toTXTRecord());
			registerQuery->onRegisterFinished.connect(boost::bind(&DNSSDTest::handleRegisterFinished, this, _1));
			registerQuery->registerService();

			// Wait for a while
			wait();

			// Check that our registered queries are correct
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>((registered.size())));
			CPPUNIT_ASSERT_EQUAL(std::string("DNSSDTest"), registered[0].getName());
			CPPUNIT_ASSERT_EQUAL(std::string("local"), registered[0].getDomain());
			CPPUNIT_ASSERT_EQUAL(std::string("_presence._tcp"), registered[0].getType());

			// Check that our browse query discovered us
			std::sort(added.begin(), added.end());
			CPPUNIT_ASSERT(added.size() >= 1);
			//for (size_t i = 0; i < added.size(); ++i) {
			for (size_t i = 0; i < added.size(); ++i) {
				CPPUNIT_ASSERT_EQUAL(std::string("DNSSDTest"), added[i].getName());
				CPPUNIT_ASSERT_EQUAL(std::string("local"), added[i].getDomain());
				CPPUNIT_ASSERT_EQUAL(std::string("_presence._tcp"), added[i].getType());
				CPPUNIT_ASSERT(added[i].getNetworkInterfaceID() != 0);
			}

			// Resolve all added services
			for (size_t i = 0; i < added.size(); ++i) {
				resolvedServices.clear();
				boost::shared_ptr<DNSSDResolveServiceQuery> resolveServiceQuery = querier->createResolveServiceQuery(added[i]);
				resolveServiceQuery->onServiceResolved.connect(boost::bind(&DNSSDTest::handleResolveFinished, this, _1));
				resolveServiceQuery->start();
				wait();
				CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(resolvedServices.size()));
				resolveServiceQuery->stop();
			}

			// Unregister the service & check if the browse query picks this up
			toRemove.clear();
			toRemove.insert(toRemove.begin(), added.begin(), added.end());
			registerQuery->unregisterService();
			while (!toRemove.empty()) {
				Swift::sleep(100);
				eventLoop->processEvents();
			}

			browseQuery->stopBrowsing();
			eventLoop->processEvents();
		}
	
	private:
		void handleServiceAdded(const DNSSDServiceID& id) {
			std::cout << "Service added: " << id.getNetworkInterfaceID() << std::endl;
			added.push_back(id);
		}

		void handleServiceRemoved(const DNSSDServiceID& id) {
			CPPUNIT_ASSERT(std::find(toRemove.begin(), toRemove.end(), id) != toRemove.end());
			erase(toRemove, id);
		}

		void handleRegisterFinished(boost::optional<DNSSDServiceID> id) {
			if (id) {
				registered.push_back(*id);
			}
		}

		void handleBrowseError() {
		}
	
		void wait() {
			for (int i = 0; i < SLEEP_INTERVALS; ++i) {
				Swift::sleep(100);
				eventLoop->processEvents();
			}
		}

		void handleResolveFinished(const boost::optional<DNSSDResolveServiceQuery::Result>& result) {
			CPPUNIT_ASSERT(result);
			resolvedServices.push_back(*result);
		}

	private:
		DummyEventLoop* eventLoop;
		boost::shared_ptr<DNSSDQuerier> querier;
		std::vector<DNSSDServiceID> added;
		std::vector<DNSSDServiceID> registered;
		std::vector<DNSSDServiceID> toRemove;
		std::vector<DNSSDResolveServiceQuery::Result> resolvedServices;
};

#ifdef HAVE_AVAHI
CPPUNIT_TEST_SUITE_REGISTRATION(DNSSDTest<AvahiQuerier>);
#endif
