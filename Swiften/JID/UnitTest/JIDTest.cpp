/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/JID/JID.h>

using namespace Swift;

class JIDTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(JIDTest);
		CPPUNIT_TEST(testConstructorWithString);
		CPPUNIT_TEST(testConstructorWithString_NoResource);
		CPPUNIT_TEST(testConstructorWithString_NoNode);
		CPPUNIT_TEST(testConstructorWithString_EmptyResource);
		CPPUNIT_TEST(testConstructorWithString_OnlyDomain);
		CPPUNIT_TEST(testConstructorWithString_UpperCaseNode);
		CPPUNIT_TEST(testConstructorWithString_UpperCaseDomain);
		CPPUNIT_TEST(testConstructorWithString_UpperCaseResource);
		CPPUNIT_TEST(testConstructorWithString_EmptyNode);
		CPPUNIT_TEST(testConstructorWithString_IllegalResource);
		CPPUNIT_TEST(testConstructorWithStrings);
		CPPUNIT_TEST(testConstructorWithStrings_EmptyDomain);
		CPPUNIT_TEST(testIsBare);
		CPPUNIT_TEST(testIsBare_NotBare);
		CPPUNIT_TEST(testToBare);
		CPPUNIT_TEST(testToBare_EmptyNode);
		CPPUNIT_TEST(testToBare_EmptyResource);
		CPPUNIT_TEST(testToString);
		CPPUNIT_TEST(testToString_EmptyNode);
		CPPUNIT_TEST(testToString_EmptyResource);
		CPPUNIT_TEST(testToString_NoResource);
		CPPUNIT_TEST(testCompare_SmallerNode);
		CPPUNIT_TEST(testCompare_LargerNode);
		CPPUNIT_TEST(testCompare_SmallerDomain);
		CPPUNIT_TEST(testCompare_LargerDomain);
		CPPUNIT_TEST(testCompare_SmallerResource);
		CPPUNIT_TEST(testCompare_LargerResource);
		CPPUNIT_TEST(testCompare_Equal);
		CPPUNIT_TEST(testCompare_EqualWithoutResource);
		CPPUNIT_TEST(testCompare_NoResourceAndEmptyResource);
		CPPUNIT_TEST(testCompare_EmptyResourceAndNoResource);
		CPPUNIT_TEST(testEquals);
		CPPUNIT_TEST(testEquals_NotEqual);
		CPPUNIT_TEST(testEquals_WithoutResource);
		CPPUNIT_TEST(testSmallerThan);
		CPPUNIT_TEST(testSmallerThan_Equal);
		CPPUNIT_TEST(testSmallerThan_Larger);
		CPPUNIT_TEST(testHasResource);
		CPPUNIT_TEST(testHasResource_NoResource);
		CPPUNIT_TEST(testGetEscapedNode);
		CPPUNIT_TEST(testGetEscapedNode_XEP106Examples);
		CPPUNIT_TEST(testGetEscapedNode_BackslashAtEnd);
		CPPUNIT_TEST(testGetUnescapedNode);
		CPPUNIT_TEST(testGetUnescapedNode_XEP106Examples);
		CPPUNIT_TEST_SUITE_END();

	public:
		JIDTest() {}

		void testConstructorWithString() {
			JID testling("foo@bar/baz");

			CPPUNIT_ASSERT_EQUAL(std::string("foo"), testling.getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.getDomain());
			CPPUNIT_ASSERT_EQUAL(std::string("baz"), testling.getResource());
			CPPUNIT_ASSERT(!testling.isBare());
		}

		void testConstructorWithString_NoResource() {
			JID testling("foo@bar");

			CPPUNIT_ASSERT_EQUAL(std::string("foo"), testling.getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.getDomain());
			CPPUNIT_ASSERT_EQUAL(std::string(""), testling.getResource());
			CPPUNIT_ASSERT(testling.isBare());
		}

		void testConstructorWithString_EmptyResource() {
			JID testling("bar/");

			CPPUNIT_ASSERT(testling.isValid());
			CPPUNIT_ASSERT(!testling.isBare());
		}

		void testConstructorWithString_NoNode() {
			JID testling("bar/baz");

			CPPUNIT_ASSERT_EQUAL(std::string(""), testling.getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.getDomain());
			CPPUNIT_ASSERT_EQUAL(std::string("baz"), testling.getResource());
			CPPUNIT_ASSERT(!testling.isBare());
		}

		void testConstructorWithString_OnlyDomain() {
			JID testling("bar");

			CPPUNIT_ASSERT_EQUAL(std::string(""), testling.getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.getDomain());
			CPPUNIT_ASSERT_EQUAL(std::string(""), testling.getResource());
			CPPUNIT_ASSERT(testling.isBare());
		}

		void testConstructorWithString_UpperCaseNode() {
			JID testling("Fo\xCE\xA9@bar");

			CPPUNIT_ASSERT_EQUAL(std::string("fo\xCF\x89"), testling.getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.getDomain());
		}

		void testConstructorWithString_UpperCaseDomain() {
			JID testling("Fo\xCE\xA9");

			CPPUNIT_ASSERT_EQUAL(std::string("fo\xCF\x89"), testling.getDomain());
		}

		void testConstructorWithString_UpperCaseResource() {
			JID testling("bar/Fo\xCE\xA9");

			CPPUNIT_ASSERT_EQUAL(testling.getResource(), std::string("Fo\xCE\xA9"));
		}

		void testConstructorWithString_EmptyNode() {
			JID testling("@bar");

			CPPUNIT_ASSERT(!testling.isValid());
		}

		void testConstructorWithString_IllegalResource() {
			JID testling("foo@bar.com/\xd8\xb1\xd9\x85\xd9\x82\xd9\x87\x20\xd8\xaa\xd8\xb1\xd9\x86\xd8\xb3\x20");

			CPPUNIT_ASSERT(!testling.isValid());
		}

		void testConstructorWithStrings() {
			JID testling("foo", "bar", "baz");

			CPPUNIT_ASSERT_EQUAL(std::string("foo"), testling.getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.getDomain());
			CPPUNIT_ASSERT_EQUAL(std::string("baz"), testling.getResource());
		}

		void testConstructorWithStrings_EmptyDomain() {
			JID testling("foo", "", "baz");

			CPPUNIT_ASSERT(!testling.isValid());
		}

		void testIsBare() {
			CPPUNIT_ASSERT(JID("foo@bar").isBare());
		}

		void testIsBare_NotBare() {
			CPPUNIT_ASSERT(!JID("foo@bar/baz").isBare());
		}

		void testToBare() {
			JID testling("foo@bar/baz");

			CPPUNIT_ASSERT_EQUAL(std::string("foo"), testling.toBare().getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.toBare().getDomain());
			CPPUNIT_ASSERT(testling.toBare().isBare());
		}

		void testToBare_EmptyNode() {
			JID testling("bar/baz");

			CPPUNIT_ASSERT_EQUAL(std::string(""), testling.toBare().getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.toBare().getDomain());
			CPPUNIT_ASSERT(testling.toBare().isBare());
		}

		void testToBare_EmptyResource() {
			JID testling("bar/");

			CPPUNIT_ASSERT_EQUAL(std::string(""), testling.toBare().getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.toBare().getDomain());
			CPPUNIT_ASSERT(testling.toBare().isBare());
		}

		void testToString() {
			JID testling("foo@bar/baz");

			CPPUNIT_ASSERT_EQUAL(std::string("foo@bar/baz"), testling.toString());
		}

		void testToString_EmptyNode() {
			JID testling("bar/baz");

			CPPUNIT_ASSERT_EQUAL(std::string("bar/baz"), testling.toString());
		}

		void testToString_NoResource() {
			JID testling("foo@bar");

			CPPUNIT_ASSERT_EQUAL(std::string("foo@bar"), testling.toString());
		}

		void testToString_EmptyResource() {
			JID testling("foo@bar/");

			CPPUNIT_ASSERT_EQUAL(std::string("foo@bar/"), testling.toString());
		}

		void testCompare_SmallerNode() {
			JID testling1("a@c");
			JID testling2("b@b");

			CPPUNIT_ASSERT_EQUAL(-1, testling1.compare(testling2, JID::WithResource));
		}

		void testCompare_LargerNode() {
			JID testling1("c@a");
			JID testling2("b@b");

			CPPUNIT_ASSERT_EQUAL(1, testling1.compare(testling2, JID::WithResource));
		}

		void testCompare_SmallerDomain() {
			JID testling1("x@a/c");
			JID testling2("x@b/b");

			CPPUNIT_ASSERT_EQUAL(-1, testling1.compare(testling2, JID::WithResource));
		}

		void testCompare_LargerDomain() {
			JID testling1("x@b/b");
			JID testling2("x@a/c");

			CPPUNIT_ASSERT_EQUAL(1, testling1.compare(testling2, JID::WithResource));
		}

		void testCompare_SmallerResource() {
			JID testling1("x@y/a");
			JID testling2("x@y/b");

			CPPUNIT_ASSERT_EQUAL(-1, testling1.compare(testling2, JID::WithResource));
		}

		void testCompare_LargerResource() {
			JID testling1("x@y/b");
			JID testling2("x@y/a");

			CPPUNIT_ASSERT_EQUAL(1, testling1.compare(testling2, JID::WithResource));
		}

		void testCompare_Equal() {
			JID testling1("x@y/z");
			JID testling2("x@y/z");

			CPPUNIT_ASSERT_EQUAL(0, testling1.compare(testling2, JID::WithResource));
		}

		void testCompare_EqualWithoutResource() {
			JID testling1("x@y/a");
			JID testling2("x@y/b");

			CPPUNIT_ASSERT_EQUAL(0, testling1.compare(testling2, JID::WithoutResource));
		}

		void testCompare_NoResourceAndEmptyResource() {
			JID testling1("x@y/");
			JID testling2("x@y");

			CPPUNIT_ASSERT_EQUAL(1, testling1.compare(testling2, JID::WithResource));
		}

		void testCompare_EmptyResourceAndNoResource() {
			JID testling1("x@y");
			JID testling2("x@y/");

			CPPUNIT_ASSERT_EQUAL(-1, testling1.compare(testling2, JID::WithResource));
		}

		void testEquals() {
			JID testling1("x@y/c");
			JID testling2("x@y/c");

			CPPUNIT_ASSERT(testling1.equals(testling2, JID::WithResource));
		}

		void testEquals_NotEqual() {
			JID testling1("x@y/c");
			JID testling2("x@y/d");

			CPPUNIT_ASSERT(!testling1.equals(testling2, JID::WithResource));
		}

		void testEquals_WithoutResource() {
			JID testling1("x@y/c");
			JID testling2("x@y/d");

			CPPUNIT_ASSERT(testling1.equals(testling2, JID::WithoutResource));
		}

		void testSmallerThan() {
			JID testling1("x@y/c");
			JID testling2("x@y/d");

			CPPUNIT_ASSERT(testling1 < testling2);
		}

		void testSmallerThan_Equal() {
			JID testling1("x@y/d");
			JID testling2("x@y/d");

			CPPUNIT_ASSERT(!(testling1 < testling2));
		}

		void testSmallerThan_Larger() {
			JID testling1("x@y/d");
			JID testling2("x@y/c");

			CPPUNIT_ASSERT(!(testling1 < testling2));
		}

		void testHasResource() {
			JID testling("x@y/d");

			CPPUNIT_ASSERT(!testling.isBare());
		}

		void testHasResource_NoResource() {
			JID testling("x@y");

			CPPUNIT_ASSERT(testling.isBare());
		}

		void testGetEscapedNode() {
			std::string escaped = JID::getEscapedNode("alice@wonderland.lit");
			CPPUNIT_ASSERT_EQUAL(std::string("alice\\40wonderland.lit"), escaped);

			escaped = JID::getEscapedNode("\\& \" ' / <\\\\> @ :\\3a\\40");
			CPPUNIT_ASSERT_EQUAL(std::string("\\\\26\\20\\22\\20\\27\\20\\2f\\20\\3c\\\\\\3e\\20\\40\\20\\3a\\5c3a\\5c40"), escaped);
		}

		void testGetEscapedNode_XEP106Examples() {
			CPPUNIT_ASSERT_EQUAL(std::string("\\2plus\\2is\\4"), JID::getEscapedNode("\\2plus\\2is\\4"));
			CPPUNIT_ASSERT_EQUAL(std::string("foo\\bar"), JID::getEscapedNode("foo\\bar"));
			CPPUNIT_ASSERT_EQUAL(std::string("foob\\41r"), JID::getEscapedNode("foob\\41r"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("space cadet"), std::string("space\\20cadet"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("call me \"ishmael\""), std::string("call\\20me\\20\\22ishmael\\22"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("at&t guy"), std::string("at\\26t\\20guy"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("d'artagnan"), std::string("d\\27artagnan"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("/.fanboy"), std::string("\\2f.fanboy"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("::foo::"), std::string("\\3a\\3afoo\\3a\\3a"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("<foo>"), std::string("\\3cfoo\\3e"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("user@host"), std::string("user\\40host"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("c:\\net"), std::string("c\\3a\\net"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("c:\\\\net"), std::string("c\\3a\\\\net"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("c:\\cool stuff"), std::string("c\\3a\\cool\\20stuff"));
			CPPUNIT_ASSERT_EQUAL(JID::getEscapedNode("c:\\5commas"), std::string("c\\3a\\5c5commas"));
		}

		void testGetEscapedNode_BackslashAtEnd() {
			CPPUNIT_ASSERT_EQUAL(std::string("foo\\"), JID::getEscapedNode("foo\\"));
		}

		void testGetUnescapedNode() {
			std::string input = "\\& \" ' / <\\\\> @ : \\5c\\40";
			JID testling(JID::getEscapedNode(input) + "@y");
			CPPUNIT_ASSERT(testling.isValid());
			CPPUNIT_ASSERT_EQUAL(input, testling.getUnescapedNode());
		}

		void testGetUnescapedNode_XEP106Examples() {
			CPPUNIT_ASSERT_EQUAL(std::string("\\2plus\\2is\\4"), JID("\\2plus\\2is\\4@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("foo\\bar"), JID("foo\\bar@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("foob\\41r"), JID("foob\\41r@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("space cadet"), JID("space\\20cadet@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("call me \"ishmael\""), JID("call\\20me\\20\\22ishmael\\22@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("at&t guy"), JID("at\\26t\\20guy@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("d'artagnan"), JID("d\\27artagnan@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("/.fanboy"), JID("\\2f.fanboy@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("::foo::"), JID("\\3a\\3afoo\\3a\\3a@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("<foo>"), JID("\\3cfoo\\3e@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("user@host"), JID("user\\40host@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("c:\\net"), JID("c\\3a\\net@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("c:\\\\net"), JID("c\\3a\\\\net@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("c:\\cool stuff"), JID("c\\3a\\cool\\20stuff@example.com").getUnescapedNode());
			CPPUNIT_ASSERT_EQUAL(std::string("c:\\5commas"), JID("c\\3a\\5c5commas@example.com").getUnescapedNode());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(JIDTest);
