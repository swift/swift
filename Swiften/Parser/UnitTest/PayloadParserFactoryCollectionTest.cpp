/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/PayloadParserFactoryCollection.h>
#include <Swiften/Parser/PayloadParserFactory.h>

using namespace Swift;

class PayloadParserFactoryCollectionTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(PayloadParserFactoryCollectionTest);
		CPPUNIT_TEST(testGetPayloadParserFactory);
		CPPUNIT_TEST(testGetPayloadParserFactory_NoMatchingFactory);
		CPPUNIT_TEST(testGetPayloadParserFactory_TwoMatchingFactories);
		CPPUNIT_TEST(testGetPayloadParserFactory_MatchWithDefaultFactory);
		CPPUNIT_TEST(testGetPayloadParserFactory_NoMatchWithDefaultFactory);
		CPPUNIT_TEST_SUITE_END();

	public:
		PayloadParserFactoryCollectionTest() {}

		void setUp() {
		}

		void tearDown() {
		}

		void testGetPayloadParserFactory() {
			PayloadParserFactoryCollection testling;
			DummyFactory factory1("foo");
			testling.addFactory(&factory1);
			DummyFactory factory2("bar");
			testling.addFactory(&factory2);
			DummyFactory factory3("baz");
			testling.addFactory(&factory3);

			PayloadParserFactory* factory = testling.getPayloadParserFactory("bar", "", AttributeMap());

			CPPUNIT_ASSERT(factory == &factory2);
		}

		void testGetPayloadParserFactory_NoMatchingFactory() {
			PayloadParserFactoryCollection testling;
			DummyFactory factory("foo");

			CPPUNIT_ASSERT(!testling.getPayloadParserFactory("bar", "", AttributeMap()));
		}

		void testGetPayloadParserFactory_TwoMatchingFactories() {
			PayloadParserFactoryCollection testling;
			DummyFactory factory1("foo");
			testling.addFactory(&factory1);
			DummyFactory factory2("foo");
			testling.addFactory(&factory2);

			PayloadParserFactory* factory = testling.getPayloadParserFactory("foo", "", AttributeMap());

			CPPUNIT_ASSERT(factory == &factory2);
		}

		void testGetPayloadParserFactory_MatchWithDefaultFactory() {
			PayloadParserFactoryCollection testling;
			DummyFactory factory1("foo");
			testling.addFactory(&factory1);
			DummyFactory factory2;
			testling.setDefaultFactory(&factory2);

			PayloadParserFactory* factory = testling.getPayloadParserFactory("foo", "", AttributeMap());

			CPPUNIT_ASSERT(factory == &factory1);
		}

		void testGetPayloadParserFactory_NoMatchWithDefaultFactory() {
			PayloadParserFactoryCollection testling;
			DummyFactory factory1("foo");
			testling.addFactory(&factory1);
			DummyFactory factory2;
			testling.setDefaultFactory(&factory2);

			PayloadParserFactory* factory = testling.getPayloadParserFactory("baz", "", AttributeMap());

			CPPUNIT_ASSERT(factory == &factory2);
		}
	
	
	private:
		struct DummyFactory : public PayloadParserFactory {
			DummyFactory(const std::string& element = "") : element(element) {}
			virtual bool canParse(const std::string& e, const std::string&, const AttributeMap&) const {
				return element.empty() ? true : element == e;
			}
			virtual PayloadParser* createPayloadParser() { return NULL; }
			std::string element;
		};
};

CPPUNIT_TEST_SUITE_REGISTRATION(PayloadParserFactoryCollectionTest);
