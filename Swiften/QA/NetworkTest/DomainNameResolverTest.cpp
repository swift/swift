/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include "Swiften/Base/sleep.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Network/PlatformDomainNameResolver.h"
#include "Swiften/Network/DomainNameAddressQuery.h"
#include "Swiften/Network/DomainNameServiceQuery.h"
#include "Swiften/EventLoop/DummyEventLoop.h"

using namespace Swift;

class DomainNameResolverTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(DomainNameResolverTest);
		CPPUNIT_TEST(testResolveAddress);
		CPPUNIT_TEST(testResolveAddress_Error);
		CPPUNIT_TEST(testResolveAddress_IPv6);
		CPPUNIT_TEST(testResolveAddress_IPv4and6);
		CPPUNIT_TEST(testResolveAddress_International);
		CPPUNIT_TEST(testResolveAddress_Localhost);
		CPPUNIT_TEST(testResolveService);
		CPPUNIT_TEST(testResolveService_Error);
		CPPUNIT_TEST_SUITE_END();

	public:
		DomainNameResolverTest() {}

		void setUp() {
			eventLoop = new DummyEventLoop();
			resolver = new PlatformDomainNameResolver();
			resultsAvailable = false;
		}

		void tearDown() {
			delete resolver;
			delete eventLoop;
		}

		void testResolveAddress() {
			boost::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("xmpp.test.swift.im"));

			query->run();
			waitForResults();

			CPPUNIT_ASSERT(!addressQueryError);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(addressQueryResult.size()));
			CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.0"), addressQueryResult[0].toString());
		}

		void testResolveAddress_Error() {
			boost::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("invalid.test.swift.im"));

			query->run();
			waitForResults();

			CPPUNIT_ASSERT(addressQueryError);
		}

		void testResolveAddress_IPv6() {
			boost::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("xmpp-ipv6.test.swift.im"));

			query->run();
			waitForResults();

			CPPUNIT_ASSERT(!addressQueryError);
			CPPUNIT_ASSERT_EQUAL(std::string("0000:0000:0000:0000:0000:ffff:0a00:0104"), addressQueryResult[0].toString());
		}

		void testResolveAddress_IPv4and6() {
			boost::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("xmpp-ipv46.test.swift.im"));

			query->run();
			waitForResults();

			CPPUNIT_ASSERT(!addressQueryError);
			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(addressQueryResult.size()));
			CPPUNIT_ASSERT_EQUAL(std::string("1234:5678:9abc:def0:0fed:cba9:8765:4321"), addressQueryResult[0].toString());
			CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.7"), addressQueryResult[1].toString());
		}

		void testResolveAddress_International() {
			boost::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("tron\xc3\xa7on.test.swift.im"));

			query->run();
			waitForResults();

			CPPUNIT_ASSERT(!addressQueryError);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(addressQueryResult.size()));
			CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.3"), addressQueryResult[0].toString());
		}

		void testResolveAddress_Localhost() {
			boost::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("localhost"));

			query->run();
			waitForResults();

			CPPUNIT_ASSERT(!addressQueryError);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(addressQueryResult.size()));
			CPPUNIT_ASSERT_EQUAL(std::string("127.0.0.1"), addressQueryResult[0].toString());
		}


		void testResolveService() {
			boost::shared_ptr<DomainNameServiceQuery> query(createServiceQuery("_xmpp-client._tcp.xmpp-srv.test.swift.im"));

			query->run();
			waitForResults();

			CPPUNIT_ASSERT_EQUAL(4, static_cast<int>(serviceQueryResult.size()));
			CPPUNIT_ASSERT_EQUAL(String("xmpp1.test.swift.im"), serviceQueryResult[0].hostname);
			CPPUNIT_ASSERT_EQUAL(5000, serviceQueryResult[0].port);
			CPPUNIT_ASSERT_EQUAL(0, serviceQueryResult[0].priority);
			CPPUNIT_ASSERT_EQUAL(1, serviceQueryResult[0].weight);
			CPPUNIT_ASSERT_EQUAL(String("xmpp-invalid.test.swift.im"), serviceQueryResult[1].hostname);
			CPPUNIT_ASSERT_EQUAL(5000, serviceQueryResult[1].port);
			CPPUNIT_ASSERT_EQUAL(1, serviceQueryResult[1].priority);
			CPPUNIT_ASSERT_EQUAL(100, serviceQueryResult[1].weight);
			CPPUNIT_ASSERT_EQUAL(String("xmpp3.test.swift.im"), serviceQueryResult[2].hostname);
			CPPUNIT_ASSERT_EQUAL(5000, serviceQueryResult[2].port);
			CPPUNIT_ASSERT_EQUAL(3, serviceQueryResult[2].priority);
			CPPUNIT_ASSERT_EQUAL(100, serviceQueryResult[2].weight);
			CPPUNIT_ASSERT_EQUAL(String("xmpp2.test.swift.im"), serviceQueryResult[3].hostname);
			CPPUNIT_ASSERT_EQUAL(5000, serviceQueryResult[3].port);
			CPPUNIT_ASSERT_EQUAL(5, serviceQueryResult[3].priority);
			CPPUNIT_ASSERT_EQUAL(100, serviceQueryResult[3].weight);
		}

		void testResolveService_Error() {
		}

/*
		}
		*/

	private:
			boost::shared_ptr<DomainNameAddressQuery> createAddressQuery(const String& domain) {
				boost::shared_ptr<DomainNameAddressQuery> result = resolver->createAddressQuery(domain);
				result->onResult.connect(boost::bind(&DomainNameResolverTest::handleAddressQueryResult, this, _1, _2));
				return result;
			}

			void handleAddressQueryResult(const std::vector<HostAddress>& addresses, boost::optional<DomainNameResolveError> error) {
				addressQueryResult = addresses;
				addressQueryError = error;
				resultsAvailable = true;
			}

			boost::shared_ptr<DomainNameServiceQuery> createServiceQuery(const String& domain) {
				boost::shared_ptr<DomainNameServiceQuery> result = resolver->createServiceQuery(domain);
				result->onResult.connect(boost::bind(&DomainNameResolverTest::handleServiceQueryResult, this, _1));
				return result;
			}

			void handleServiceQueryResult(const std::vector<DomainNameServiceQuery::Result>& result) {
				serviceQueryResult = result;
				resultsAvailable = true;
			}

			void waitForResults() {
				eventLoop->processEvents();
				int ticks = 0;
				while (!resultsAvailable) {
					ticks++;
					if (ticks > 1000) {
						CPPUNIT_ASSERT(false);
					}
					Swift::sleep(10);
					eventLoop->processEvents();
				}
			}

	private:
		DummyEventLoop* eventLoop;
		bool resultsAvailable;
		std::vector<HostAddress> addressQueryResult;
		boost::optional<DomainNameResolveError> addressQueryError;
		std::vector<DomainNameServiceQuery::Result> serviceQueryResult;
		PlatformDomainNameResolver* resolver;
};

CPPUNIT_TEST_SUITE_REGISTRATION(DomainNameResolverTest);
