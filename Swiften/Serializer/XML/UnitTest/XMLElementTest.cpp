/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

using namespace Swift;

class XMLElementTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(XMLElementTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST(testSerialize_NoChildren);
		CPPUNIT_TEST(testSerialize_SpecialAttributeCharacters);
		CPPUNIT_TEST(testSerialize_EmptyAttributeValue);
		CPPUNIT_TEST_SUITE_END();

	public:
		XMLElementTest() {}

		void testSerialize() {
			XMLElement testling("foo", "http://example.com");
			testling.setAttribute("myatt", "myval");
			boost::shared_ptr<XMLElement> barElement(new XMLElement("bar"));
			barElement->addNode(boost::make_shared<XMLTextNode>("Blo"));
			testling.addNode(barElement);
			boost::shared_ptr<XMLElement> bazElement(new XMLElement("baz"));
			bazElement->addNode(boost::make_shared<XMLTextNode>("Bli&</stream>"));
			testling.addNode(bazElement);

			std::string result = testling.serialize();
			std::string expectedResult = 
				"<foo myatt=\"myval\" xmlns=\"http://example.com\">"
					"<bar>Blo</bar>"
					"<baz>Bli&amp;&lt;/stream&gt;</baz>"
				"</foo>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, result);
		}

		void testSerialize_NoChildren() {
			XMLElement testling("foo", "http://example.com");

			CPPUNIT_ASSERT_EQUAL(std::string("<foo xmlns=\"http://example.com\"/>"), testling.serialize());
		}

		void testSerialize_SpecialAttributeCharacters() {
			XMLElement testling("foo");
			testling.setAttribute("myatt", "<\"'&>");

			CPPUNIT_ASSERT_EQUAL(std::string("<foo myatt=\"&lt;&quot;&apos;&amp;&gt;\"/>"), testling.serialize());
		}

		void testSerialize_EmptyAttributeValue() {
			XMLElement testling("foo");
			testling.setAttribute("myatt", "");

			CPPUNIT_ASSERT_EQUAL(std::string("<foo myatt=\"\"/>"), testling.serialize());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(XMLElementTest);
