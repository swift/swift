/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Network/DomainNameServiceQuery.h>
#include <Swiften/Base/RandomGenerator.h>

using namespace Swift;

namespace {
	struct RandomGenerator1 : public RandomGenerator {
		virtual int generateRandomInteger(int) {
			return 0;
		}
	};

	struct RandomGenerator2 : public RandomGenerator {
		virtual int generateRandomInteger(int i) {
			return i;
		}
	};
}

class DomainNameServiceQueryTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(DomainNameServiceQueryTest);
		CPPUNIT_TEST(testSortResults_Random1);
		CPPUNIT_TEST(testSortResults_Random2);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSortResults_Random1() {
			std::vector<DomainNameServiceQuery::Result> results;
			results.push_back(DomainNameServiceQuery::Result("server1.com", 5222, 5, 1));
			results.push_back(DomainNameServiceQuery::Result("server2.com", 5222, 3, 10));
			results.push_back(DomainNameServiceQuery::Result("server3.com", 5222, 6, 1));
			results.push_back(DomainNameServiceQuery::Result("server4.com", 5222, 3, 20));
			results.push_back(DomainNameServiceQuery::Result("server5.com", 5222, 2, 1));
			results.push_back(DomainNameServiceQuery::Result("server6.com", 5222, 3, 10));

			RandomGenerator1 generator;
			DomainNameServiceQuery::sortResults(results, generator);

			CPPUNIT_ASSERT_EQUAL(std::string("server5.com"), results[0].hostname);
			CPPUNIT_ASSERT_EQUAL(std::string("server2.com"), results[1].hostname);
			CPPUNIT_ASSERT_EQUAL(std::string("server4.com"), results[2].hostname);
			CPPUNIT_ASSERT_EQUAL(std::string("server6.com"), results[3].hostname);
			CPPUNIT_ASSERT_EQUAL(std::string("server1.com"), results[4].hostname);
			CPPUNIT_ASSERT_EQUAL(std::string("server3.com"), results[5].hostname);
		}

		void testSortResults_Random2() {
			std::vector<DomainNameServiceQuery::Result> results;
			results.push_back(DomainNameServiceQuery::Result("server1.com", 5222, 5, 1));
			results.push_back(DomainNameServiceQuery::Result("server2.com", 5222, 3, 10));
			results.push_back(DomainNameServiceQuery::Result("server3.com", 5222, 6, 1));
			results.push_back(DomainNameServiceQuery::Result("server4.com", 5222, 3, 20));
			results.push_back(DomainNameServiceQuery::Result("server5.com", 5222, 2, 1));
			results.push_back(DomainNameServiceQuery::Result("server6.com", 5222, 3, 10));
			results.push_back(DomainNameServiceQuery::Result("server7.com", 5222, 3, 40));

			RandomGenerator2 generator;
			DomainNameServiceQuery::sortResults(results, generator);

			CPPUNIT_ASSERT_EQUAL(std::string("server5.com"), results[0].hostname);
			CPPUNIT_ASSERT_EQUAL(std::string("server7.com"), results[1].hostname);
			CPPUNIT_ASSERT_EQUAL(std::string("server2.com"), results[2].hostname);
			CPPUNIT_ASSERT_EQUAL(std::string("server4.com"), results[3].hostname);
			CPPUNIT_ASSERT_EQUAL(std::string("server6.com"), results[4].hostname);
			CPPUNIT_ASSERT_EQUAL(std::string("server1.com"), results[5].hostname);
			CPPUNIT_ASSERT_EQUAL(std::string("server3.com"), results[6].hostname);
		}
};


CPPUNIT_TEST_SUITE_REGISTRATION(DomainNameServiceQueryTest);
