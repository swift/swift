/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/LinkLocal/LinkLocalService.h>

using namespace Swift;

class LinkLocalServiceTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(LinkLocalServiceTest);
		CPPUNIT_TEST(testGetDescription_WithNick);
		CPPUNIT_TEST(testGetDescription_WithFirstName);
		CPPUNIT_TEST(testGetDescription_WithLastName);
		CPPUNIT_TEST(testGetDescription_WithFirstAndLastName);
		CPPUNIT_TEST(testGetDescription_NoInfo);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetDescription_WithNick() {
			LinkLocalService testling = createService("alice@wonderland", "Alice", "Alice In", "Wonderland");

			CPPUNIT_ASSERT_EQUAL(std::string("Alice"), testling.getDescription());
		}

		void testGetDescription_WithFirstName() {
			LinkLocalService testling = createService("alice@wonderland", "", "Alice In");

			CPPUNIT_ASSERT_EQUAL(std::string("Alice In"), testling.getDescription());
		}

		void testGetDescription_WithLastName() {
			LinkLocalService testling = createService("alice@wonderland", "", "", "Wonderland");

			CPPUNIT_ASSERT_EQUAL(std::string("Wonderland"), testling.getDescription());
		}

		void testGetDescription_WithFirstAndLastName() {
			LinkLocalService testling = createService("alice@wonderland", "", "Alice In", "Wonderland");

			CPPUNIT_ASSERT_EQUAL(std::string("Alice In Wonderland"), testling.getDescription());
		}

		void testGetDescription_NoInfo() {
			LinkLocalService testling = createService("alice@wonderland");

			CPPUNIT_ASSERT_EQUAL(std::string("alice@wonderland"), testling.getDescription());
		}
	
	private:
		LinkLocalService createService(const std::string& name, const std::string& nickName = std::string(), const std::string& firstName = std::string(), const std::string& lastName = std::string()) {
			DNSSDServiceID service(name, "local.");
			LinkLocalServiceInfo info;
			info.setFirstName(firstName);
			info.setLastName(lastName);
			info.setNick(nickName);
			return LinkLocalService(service, 
					DNSSDResolveServiceQuery::Result(
						name + "._presence._tcp.local", "rabbithole.local", 1234, 
						info.toTXTRecord()));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(LinkLocalServiceTest);
