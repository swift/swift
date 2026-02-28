/*
 * Copyright (c) 2015 Tarun Gupta.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/UserLocation.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Parser/PayloadParsers/UserLocationParser.h>

using namespace Swift;

class UserLocationParserTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(UserLocationParserTest);
        CPPUNIT_TEST(testParse_with_all_variables);
        CPPUNIT_TEST(testParse_with_Some_variables);
        CPPUNIT_TEST_SUITE_END();

    public:
        UserLocationParserTest() {}

        void testParse_with_all_variables() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                                "<geoloc xmlns=\"http://jabber.org/protocol/geoloc\">"
                                "<area>Barbaric</area><alt>5.75</alt><locality>Near</locality>"
                                "<lat>1.67</lat><accuracy>0.95</accuracy><description>Nice</description>"
                                "<countrycode>+91</countrycode><timestamp>2015-06-11T20:55:50Z</timestamp><floor>3</floor>"
                                "<building>First</building><room>E315</room><country>USA</country>"
                                "<region>NewSode</region><uri>URIs</uri><lon>6.7578</lon><error>5.66</error>"
                                "<postalcode>67</postalcode><bearing>12.89</bearing><text>Hello</text>"
                                "<datum>Datee</datum><street>Highway</street><speed>56.77</speed></geoloc>"));

            UserLocation* payload = dynamic_cast<UserLocation*>(parser.getPayload().get());
            CPPUNIT_ASSERT(payload);
            CPPUNIT_ASSERT_EQUAL(std::string("Barbaric"), payload->getArea().get());
            CPPUNIT_ASSERT_EQUAL(5.75, payload->getAltitude().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Near"), payload->getLocality().get());
            CPPUNIT_ASSERT_EQUAL(1.670, payload->getLatitude().get());
            CPPUNIT_ASSERT_EQUAL(0.95, payload->getAccuracy().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Nice"), payload->getDescription().get());
            CPPUNIT_ASSERT_EQUAL(std::string("+91"), payload->getCountryCode().get());
            CPPUNIT_ASSERT_EQUAL(std::string("2015-06-11T20:55:50Z"), dateTimeToString(payload->getTimestamp().get()));
            CPPUNIT_ASSERT_EQUAL(std::string("3"), payload->getFloor().get());
            CPPUNIT_ASSERT_EQUAL(std::string("First"), payload->getBuilding().get());
            CPPUNIT_ASSERT_EQUAL(std::string("E315"), payload->getRoom().get());
            CPPUNIT_ASSERT_EQUAL(std::string("USA"), payload->getCountry().get());
            CPPUNIT_ASSERT_EQUAL(std::string("NewSode"), payload->getRegion().get());
            CPPUNIT_ASSERT_EQUAL(std::string("URIs"), payload->getURI().get());
            CPPUNIT_ASSERT_EQUAL(6.7578, payload->getLongitude().get());
            CPPUNIT_ASSERT_EQUAL(5.66, payload->getError().get());
            CPPUNIT_ASSERT_EQUAL(std::string("67"), payload->getPostalCode().get());
            CPPUNIT_ASSERT_EQUAL(12.89, payload->getBearing().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Hello"), payload->getText().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Datee"), payload->getDatum().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Highway"), payload->getStreet().get());
            CPPUNIT_ASSERT_EQUAL(56.77, payload->getSpeed().get());
        }

        void testParse_with_Some_variables() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse(
                                "<geoloc xmlns=\"http://jabber.org/protocol/geoloc\">"
                                "<area>Barbaric</area><alt>5.75</alt><locality>Near</locality>"
                                "<accuracy>0.95</accuracy><description>Nice</description>"
                                "<countrycode>+91</countrycode><timestamp>2015-06-11T20:55:50Z</timestamp><floor>3</floor>"
                                "<region>NewSode</region><uri>URIs</uri><lon>6.7578</lon><error>5.66</error>"
                                "<postalcode>67</postalcode><bearing>12.89</bearing><text>Hello</text></geoloc>"));

            UserLocation* payload = dynamic_cast<UserLocation*>(parser.getPayload().get());
            CPPUNIT_ASSERT(payload);
            CPPUNIT_ASSERT_EQUAL(std::string("Barbaric"), payload->getArea().get());
            CPPUNIT_ASSERT_EQUAL(5.75, payload->getAltitude().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Near"), payload->getLocality().get());
            CPPUNIT_ASSERT(!payload->getLatitude());
            CPPUNIT_ASSERT_EQUAL(0.95, payload->getAccuracy().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Nice"), payload->getDescription().get());
            CPPUNIT_ASSERT_EQUAL(std::string("+91"), payload->getCountryCode().get());
            CPPUNIT_ASSERT_EQUAL(std::string("2015-06-11T20:55:50Z"), dateTimeToString(payload->getTimestamp().get()));
            CPPUNIT_ASSERT_EQUAL(std::string("3"), payload->getFloor().get());
            CPPUNIT_ASSERT(!payload->getBuilding());
            CPPUNIT_ASSERT(!payload->getRoom());
            CPPUNIT_ASSERT(!payload->getCountry());
            CPPUNIT_ASSERT_EQUAL(std::string("NewSode"), payload->getRegion().get());
            CPPUNIT_ASSERT_EQUAL(std::string("URIs"), payload->getURI().get());
            CPPUNIT_ASSERT_EQUAL(6.7578, payload->getLongitude().get());
            CPPUNIT_ASSERT_EQUAL(5.66, payload->getError().get());
            CPPUNIT_ASSERT_EQUAL(std::string("67"), payload->getPostalCode().get());
            CPPUNIT_ASSERT_EQUAL(12.89, payload->getBearing().get());
            CPPUNIT_ASSERT_EQUAL(std::string("Hello"), payload->getText().get());
            CPPUNIT_ASSERT(!payload->getDatum());
            CPPUNIT_ASSERT(!payload->getStreet());
            CPPUNIT_ASSERT(!payload->getSpeed());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(UserLocationParserTest);
