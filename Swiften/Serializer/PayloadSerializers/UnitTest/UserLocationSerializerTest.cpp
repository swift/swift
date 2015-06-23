/*
 * Copyright (c) 2015 Tarun Gupta.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializers/UserLocationSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h> 
#include <Swiften/Elements/UserLocation.h>
#include <Swiften/Base/DateTime.h>

using namespace Swift;

class UserLocationSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(UserLocationSerializerTest);
		CPPUNIT_TEST(testSerialize_withAllVariablesSet);
		CPPUNIT_TEST(testSerialize_withSomeVariablesSet);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerialize_withAllVariablesSet() {
			UserLocationSerializer testling(&serializers);
			boost::shared_ptr<UserLocation> userLocation(new UserLocation());
			userLocation->setArea(boost::optional<std::string>("Barbaric"));
			userLocation->setAltitude(5.75F);
			userLocation->setLocality(boost::optional<std::string>("Near"));
			userLocation->setLatitude(boost::optional<float>(5.75F));
			userLocation->setAccuracy(5.75F);
			userLocation->setDescription(boost::optional<std::string>("Nice"));
			userLocation->setCountryCode(boost::optional<std::string>("+91"));
			userLocation->setTimestamp(stringToDateTime("2015-06-11T20:55:50Z"));
			userLocation->setFloor(boost::optional<std::string>("3"));
			userLocation->setBuilding(boost::optional<std::string>("First"));
			userLocation->setRoom(boost::optional<std::string>("E315"));
			userLocation->setCountry(boost::optional<std::string>("USA"));
			userLocation->setRegion(boost::optional<std::string>("NewSode"));
			userLocation->setURI(boost::optional<std::string>("URIs"));
			userLocation->setLongitude(5.75F);
			userLocation->setError(5.75F);
			userLocation->setPostalCode(boost::optional<std::string>("67"));
			userLocation->setBearing(5.75F);
			userLocation->setText(boost::optional<std::string>("Hello"));
			userLocation->setDatum(boost::optional<std::string>("Datee"));
			userLocation->setStreet(boost::optional<std::string>("Highway"));
			userLocation->setSpeed(5.75F);

			std::string expectedResult = 
								"<geoloc xmlns=\"http://jabber.org/protocol/geoloc\">"
								"<area>Barbaric</area><alt>5.75</alt><locality>Near</locality>"
								"<lat>5.75</lat><accuracy>5.75</accuracy><description>Nice</description>"
								"<countrycode>+91</countrycode><timestamp>2015-06-11T20:55:50Z</timestamp><floor>3</floor>"
								"<building>First</building><room>E315</room><country>USA</country>"
								"<region>NewSode</region><uri>URIs</uri><lon>5.75</lon><error>5.75</error>"
								"<postalcode>67</postalcode><bearing>5.75</bearing><text>Hello</text>"
								"<datum>Datee</datum><street>Highway</street><speed>5.75</speed></geoloc>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(userLocation));
		}

		void testSerialize_withSomeVariablesSet() {
			UserLocationSerializer testling(&serializers);
			boost::shared_ptr<UserLocation> userLocation(new UserLocation());
			userLocation->setArea(boost::optional<std::string>("Barbaric"));
			userLocation->setAltitude(5.75F);
			userLocation->setLocality(boost::optional<std::string>("Near"));
			userLocation->setAccuracy(5.75F);
			userLocation->setDescription(boost::optional<std::string>("Nice"));
			userLocation->setCountryCode(boost::optional<std::string>("+91"));
			userLocation->setTimestamp(stringToDateTime("2015-06-11T20:55:50Z"));
			userLocation->setFloor(boost::optional<std::string>("3"));
			userLocation->setRegion(boost::optional<std::string>("NewSode"));
			userLocation->setURI(boost::optional<std::string>("URIs"));
			userLocation->setLongitude(5.75F);
			userLocation->setError(5.75F);
			userLocation->setPostalCode(boost::optional<std::string>("67"));
			userLocation->setBearing(5.75F);
			userLocation->setText(boost::optional<std::string>("Hello"));

			std::string expectedResult = 
								"<geoloc xmlns=\"http://jabber.org/protocol/geoloc\">"
								"<area>Barbaric</area><alt>5.75</alt><locality>Near</locality>"
								"<accuracy>5.75</accuracy><description>Nice</description>"
								"<countrycode>+91</countrycode><timestamp>2015-06-11T20:55:50Z</timestamp><floor>3</floor>"
								"<region>NewSode</region><uri>URIs</uri><lon>5.75</lon><error>5.75</error>"
								"<postalcode>67</postalcode><bearing>5.75</bearing><text>Hello</text></geoloc>";

			CPPUNIT_ASSERT_EQUAL(expectedResult, testling.serialize(userLocation));
		}
	private:
		FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(UserLocationSerializerTest);
