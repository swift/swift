#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Serializer/XML/XMLElement.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"

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
			barElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode("Blo")));
			testling.addNode(barElement);
			boost::shared_ptr<XMLElement> bazElement(new XMLElement("baz"));
			bazElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode("Bli")));
			testling.addNode(bazElement);

			String result = testling.serialize();
			String expectedResult = 
				"<foo myatt=\"myval\" xmlns=\"http://example.com\">"
					"<bar>Blo</bar>"
					"<baz>Bli</baz>"
				"</foo>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, result);
		}

		void testSerialize_NoChildren() {
			XMLElement testling("foo", "http://example.com");

			CPPUNIT_ASSERT_EQUAL(String("<foo xmlns=\"http://example.com\"/>"), testling.serialize());
		}

		void testSerialize_SpecialAttributeCharacters() {
			XMLElement testling("foo");
			testling.setAttribute("myatt", "<\"'&>");

			CPPUNIT_ASSERT_EQUAL(String("<foo myatt=\"&lt;&quot;&apos;&amp;&gt;\"/>"), testling.serialize());
		}

		void testSerialize_EmptyAttributeValue() {
			XMLElement testling("foo");
			testling.setAttribute("myatt", "");

			CPPUNIT_ASSERT_EQUAL(String("<foo myatt=\"\"/>"), testling.serialize());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(XMLElementTest);
