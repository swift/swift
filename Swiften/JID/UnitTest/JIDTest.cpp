/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/JID/JID.h"

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
		CPPUNIT_TEST(testConstructorWithStrings);
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

		void testConstructorWithStrings() {
			JID testling("foo", "bar", "baz");

			CPPUNIT_ASSERT_EQUAL(std::string("foo"), testling.getNode());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), testling.getDomain());
			CPPUNIT_ASSERT_EQUAL(std::string("baz"), testling.getResource());
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
};

CPPUNIT_TEST_SUITE_REGISTRATION(JIDTest);
