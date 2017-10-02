/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <algorithm>
#include <string>

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Platform.h>
#include <Swiften/Base/sleep.h>
#include <Swiften/EventLoop/DummyEventLoop.h>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/IDN/PlatformIDNConverter.h>
#include <Swiften/Network/BoostIOServiceThread.h>
#include <Swiften/Network/BoostTimerFactory.h>
#include <Swiften/Network/DomainNameAddressQuery.h>
#include <Swiften/Network/DomainNameServiceQuery.h>
#include <Swiften/Network/NetworkFactories.h>
#ifdef USE_UNBOUND
#include <Swiften/Network/UnboundDomainNameResolver.h>
#else
#include <Swiften/Network/PlatformDomainNameResolver.h>
#endif

using namespace Swift;

struct CompareHostAddresses {
    bool operator()(const HostAddress& h1, const HostAddress& h2) {
        return h1.toString() < h2.toString();
    }
};

class DomainNameResolverTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(DomainNameResolverTest);
        CPPUNIT_TEST(testResolveAddress);
        CPPUNIT_TEST(testResolveAddress_Error);
#ifndef USE_UNBOUND
        /**
         * The native DNS resolver of Windows behaves oddly if the system has no global IPv6
         * routed address and no IPv6 reachability. It will not return IPv6 records from DNS
         * requests for an unspecified protocol (IPv6 or IPv4).
         * The following tests are only enabled on Windows if scons is run with the 'test_ipv6=1'
         * argument, indicating working IPv6 on the test machine.
         */
#if !defined(SWIFTEN_PLATFORM_WINDOWS) || defined(TEST_IPV6)
        CPPUNIT_TEST(testResolveAddress_IPv6);
        CPPUNIT_TEST(testResolveAddress_IPv4and6);
#endif
        CPPUNIT_TEST(testResolveAddress_International);
#endif
        CPPUNIT_TEST(testResolveAddress_Localhost);
        CPPUNIT_TEST(testResolveAddress_Parallel);
#ifndef USE_UNBOUND
        CPPUNIT_TEST(testResolveService);
#endif
        CPPUNIT_TEST(testResolveService_Error);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            ioServiceThread = new BoostIOServiceThread();
            eventLoop = new DummyEventLoop();
            idnConverter = std::shared_ptr<IDNConverter>(PlatformIDNConverter::create());
#ifdef USE_UNBOUND
            resolver = new UnboundDomainNameResolver(idnConverter.get(), ioServiceThread->getIOService(), eventLoop);
#else
            resolver = new PlatformDomainNameResolver(idnConverter.get(), eventLoop);
#endif
            resultsAvailable = false;
        }

        void tearDown() {
            delete ioServiceThread;
            delete resolver;
            delete eventLoop;
        }

        void testResolveAddress() {
            std::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("xmpp.test.swift.im"));

            query->run();
            waitForResults();

            CPPUNIT_ASSERT(!addressQueryError);
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(addressQueryResult.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.0"), addressQueryResult[0].toString());
        }

        void testResolveAddress_Error() {
            std::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("invalid.test.swift.im"));

            query->run();
            waitForResults();

            CPPUNIT_ASSERT(addressQueryError);
        }

        void testResolveAddress_IPv6() {
            std::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("xmpp-ipv6.test.swift.im"));

            query->run();
            waitForResults();

            CPPUNIT_ASSERT(!addressQueryError);
            CPPUNIT_ASSERT_EQUAL(std::string("2001:470:1f0e:852::2"), addressQueryResult[0].toString());
        }

        void testResolveAddress_IPv4and6() {
            std::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("xmpp-ipv46.test.swift.im"));

            query->run();
            waitForResults();

            CPPUNIT_ASSERT(!addressQueryError);
            CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(addressQueryResult.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.7"), addressQueryResult[0].toString());
            CPPUNIT_ASSERT_EQUAL(std::string("1234:5678:9abc:def0:fed:cba9:8765:4321"), addressQueryResult[1].toString());
        }

        void testResolveAddress_International() {
            std::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("tron\xc3\xa7on.test.swift.im"));

            query->run();
            waitForResults();

            CPPUNIT_ASSERT(!addressQueryError);
            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(addressQueryResult.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.3"), addressQueryResult[0].toString());
        }

        void testResolveAddress_Localhost() {
            std::shared_ptr<DomainNameAddressQuery> query(createAddressQuery("localhost"));

            query->run();
            waitForResults();

            CPPUNIT_ASSERT(!addressQueryError);
            CPPUNIT_ASSERT(std::find(addressQueryResult.begin(), addressQueryResult.end(), HostAddress::fromString("127.0.0.1").get()) != addressQueryResult.end());
        }

        void testResolveAddress_Parallel() {
            std::vector<DomainNameAddressQuery::ref> queries;
            static const size_t numQueries = 100;
            for (size_t i = 0; i < numQueries; ++i) {
                DomainNameAddressQuery::ref query(createAddressQuery("xmpp.test.swift.im"));
                queries.push_back(query);
                query->run();
            }

            eventLoop->processEvents();
            int ticks = 0;
            while (allAddressQueryResults.size() < numQueries) {
                ticks++;
                if (ticks > 1000) {
                    CPPUNIT_ASSERT(false);
                }
                Swift::sleep(10);
                eventLoop->processEvents();
            }

            CPPUNIT_ASSERT_EQUAL(numQueries, allAddressQueryResults.size());
            for (size_t i = 0; i < numQueries; ++i) {
                CPPUNIT_ASSERT_EQUAL(std::string("10.0.0.0"), allAddressQueryResults[i].toString());
            }
        }

        void testResolveService() {
            std::shared_ptr<DomainNameServiceQuery> query(createServiceQuery("_xmpp-client._tcp.", "xmpp-srv.test.swift.im"));

            query->run();
            waitForResults();

            CPPUNIT_ASSERT_EQUAL(4, static_cast<int>(serviceQueryResult.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("xmpp1.test.swift.im"), serviceQueryResult[0].hostname);
            CPPUNIT_ASSERT_EQUAL(5000, serviceQueryResult[0].port);
            CPPUNIT_ASSERT_EQUAL(0, serviceQueryResult[0].priority);
            CPPUNIT_ASSERT_EQUAL(1, serviceQueryResult[0].weight);
            CPPUNIT_ASSERT_EQUAL(std::string("xmpp-invalid.test.swift.im"), serviceQueryResult[1].hostname);
            CPPUNIT_ASSERT_EQUAL(5000, serviceQueryResult[1].port);
            CPPUNIT_ASSERT_EQUAL(1, serviceQueryResult[1].priority);
            CPPUNIT_ASSERT_EQUAL(100, serviceQueryResult[1].weight);
            CPPUNIT_ASSERT_EQUAL(std::string("xmpp3.test.swift.im"), serviceQueryResult[2].hostname);
            CPPUNIT_ASSERT_EQUAL(5000, serviceQueryResult[2].port);
            CPPUNIT_ASSERT_EQUAL(3, serviceQueryResult[2].priority);
            CPPUNIT_ASSERT_EQUAL(100, serviceQueryResult[2].weight);
            CPPUNIT_ASSERT_EQUAL(std::string("xmpp2.test.swift.im"), serviceQueryResult[3].hostname);
            CPPUNIT_ASSERT_EQUAL(5000, serviceQueryResult[3].port);
            CPPUNIT_ASSERT_EQUAL(5, serviceQueryResult[3].priority);
            CPPUNIT_ASSERT_EQUAL(100, serviceQueryResult[3].weight);
        }

        void testResolveService_Error() {
        }

    private:
            std::shared_ptr<DomainNameAddressQuery> createAddressQuery(const std::string& domain) {
                std::shared_ptr<DomainNameAddressQuery> result = resolver->createAddressQuery(domain);
                result->onResult.connect(boost::bind(&DomainNameResolverTest::handleAddressQueryResult, this, _1, _2));
                return result;
            }

            void handleAddressQueryResult(const std::vector<HostAddress>& addresses, boost::optional<DomainNameResolveError> error) {
                addressQueryResult = addresses;
                std::sort(addressQueryResult.begin(), addressQueryResult.end(), CompareHostAddresses());
                allAddressQueryResults.insert(allAddressQueryResults.begin(), addresses.begin(), addresses.end());
                addressQueryError = error;
                resultsAvailable = true;
            }

            std::shared_ptr<DomainNameServiceQuery> createServiceQuery(const std::string& serviceLookupPrefix, const std::string& domain) {
                std::shared_ptr<DomainNameServiceQuery> result = resolver->createServiceQuery(serviceLookupPrefix, domain);
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
        BoostIOServiceThread* ioServiceThread;
        DummyEventLoop* eventLoop;
        std::shared_ptr<IDNConverter> idnConverter;
        std::shared_ptr<TimerFactory> timerFactory;
        bool resultsAvailable;
        std::vector<HostAddress> addressQueryResult;
        std::vector<HostAddress> allAddressQueryResults;
        boost::optional<DomainNameResolveError> addressQueryError;
        std::vector<DomainNameServiceQuery::Result> serviceQueryResult;
        DomainNameResolver* resolver;
};

CPPUNIT_TEST_SUITE_REGISTRATION(DomainNameResolverTest);
