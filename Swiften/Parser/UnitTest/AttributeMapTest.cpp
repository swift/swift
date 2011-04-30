/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Parser/AttributeMap.h>

using namespace Swift;

class AttributeMapTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(AttributeMapTest);
		CPPUNIT_TEST(testGetAttribute_Namespaced);
		CPPUNIT_TEST(testGetBoolAttribute_True);
		CPPUNIT_TEST(testGetBoolAttribute_1);
		CPPUNIT_TEST(testGetBoolAttribute_False);
		CPPUNIT_TEST(testGetBoolAttribute_0);
		CPPUNIT_TEST(testGetBoolAttribute_Invalid);
		CPPUNIT_TEST(testGetBoolAttribute_UnknownWithDefaultTrue);
		CPPUNIT_TEST(testGetBoolAttribute_UnknownWithDefaultFalse);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetAttribute_Namespaced() {
			AttributeMap testling;
			testling.addAttribute("lang", "", "nl");
			testling.addAttribute("lang", "http://www.w3.org/XML/1998/namespace", "en");
			testling.addAttribute("lang", "", "fr");

			CPPUNIT_ASSERT_EQUAL(std::string("en"), testling.getAttribute("lang", "http://www.w3.org/XML/1998/namespace"));
		}

		void testGetBoolAttribute_True() {
			AttributeMap testling;
			testling.addAttribute("foo", "", "true");

			CPPUNIT_ASSERT(testling.getBoolAttribute("foo"));
		}

		void testGetBoolAttribute_1() {
			AttributeMap testling;
			testling.addAttribute("foo", "", "1");

			CPPUNIT_ASSERT(testling.getBoolAttribute("foo"));
		}

		void testGetBoolAttribute_False() {
			AttributeMap testling;
			testling.addAttribute("foo", "", "false");

			CPPUNIT_ASSERT(!testling.getBoolAttribute("foo", true));
		}

		void testGetBoolAttribute_0() {
			AttributeMap testling;
			testling.addAttribute("foo", "", "0");

			CPPUNIT_ASSERT(!testling.getBoolAttribute("foo", true));
		}

		void testGetBoolAttribute_Invalid() {
			AttributeMap testling;
			testling.addAttribute("foo", "", "bla");

			CPPUNIT_ASSERT(!testling.getBoolAttribute("foo", true));
		}

		void testGetBoolAttribute_UnknownWithDefaultTrue() {
			AttributeMap testling;

			CPPUNIT_ASSERT(testling.getBoolAttribute("foo", true));
		}

		void testGetBoolAttribute_UnknownWithDefaultFalse() {
			AttributeMap testling;

			CPPUNIT_ASSERT(!testling.getBoolAttribute("foo", false));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(AttributeMapTest);
