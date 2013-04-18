/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <boost/shared_ptr.hpp>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/IDN/PlatformIDNConverter.h>

using namespace Swift;

class IDNConverterTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(IDNConverterTest);
		CPPUNIT_TEST(testStringPrep);
		CPPUNIT_TEST(testStringPrep_Empty);
		CPPUNIT_TEST(testGetEncoded);
		CPPUNIT_TEST(testGetEncoded_International);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			testling = boost::shared_ptr<IDNConverter>(PlatformIDNConverter::create());
		}

		void testStringPrep() {
			std::string result = testling->getStringPrepared("tron\xc3\x87on", IDNConverter::NamePrep);

			CPPUNIT_ASSERT_EQUAL(std::string("tron\xc3\xa7on"), result);
		}

		void testStringPrep_Empty() {
			CPPUNIT_ASSERT_EQUAL(std::string(""), testling->getStringPrepared("", IDNConverter::NamePrep));
			CPPUNIT_ASSERT_EQUAL(std::string(""), testling->getStringPrepared("", IDNConverter::XMPPNodePrep));
			CPPUNIT_ASSERT_EQUAL(std::string(""), testling->getStringPrepared("", IDNConverter::XMPPResourcePrep));
		}

		void testGetEncoded() {
			std::string result = testling->getIDNAEncoded("www.swift.im");
			CPPUNIT_ASSERT_EQUAL(std::string("www.swift.im"), result);
		}

		void testGetEncoded_International() {
			std::string result = testling->getIDNAEncoded("www.tron\xc3\x87on.com");
			CPPUNIT_ASSERT_EQUAL(std::string("www.xn--tronon-zua.com"), result); 
		}


	private:
		boost::shared_ptr<IDNConverter> testling;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IDNConverterTest);
