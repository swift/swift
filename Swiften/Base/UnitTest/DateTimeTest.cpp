/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/Base/DateTime.h>

using namespace Swift;

class DateTimeTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(DateTimeTest);
		CPPUNIT_TEST(testStringToDateTime_UTC);
		CPPUNIT_TEST(testStringToDateTime_WithTimezone);
		CPPUNIT_TEST(testDateTimeToString);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testStringToDateTime_UTC() {
			boost::posix_time::ptime time = stringToDateTime("1969-07-21T02:56:15Z");

			CPPUNIT_ASSERT_EQUAL(std::string("1969-07-21T02:56:15"), boost::posix_time::to_iso_extended_string(time));
		}

		void testStringToDateTime_WithTimezone() {
			boost::posix_time::ptime time = stringToDateTime("1969-07-20T21:56:15-05:00");

			CPPUNIT_ASSERT_EQUAL(std::string("1969-07-21T02:56:15"), boost::posix_time::to_iso_extended_string(time));
		}

		void testDateTimeToString() {
			boost::posix_time::ptime time = stringToDateTime("1969-07-20T21:56:15-05:00");

			CPPUNIT_ASSERT_EQUAL(std::string("1969-07-21T02:56:15Z"), dateTimeToString(time));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(DateTimeTest);
