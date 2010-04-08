/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/LinkLocal/LinkLocalService.h"

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

			CPPUNIT_ASSERT_EQUAL(String("Alice"), testling.getDescription());
		}

		void testGetDescription_WithFirstName() {
			LinkLocalService testling = createService("alice@wonderland", "", "Alice In");

			CPPUNIT_ASSERT_EQUAL(String("Alice In"), testling.getDescription());
		}

		void testGetDescription_WithLastName() {
			LinkLocalService testling = createService("alice@wonderland", "", "", "Wonderland");

			CPPUNIT_ASSERT_EQUAL(String("Wonderland"), testling.getDescription());
		}

		void testGetDescription_WithFirstAndLastName() {
			LinkLocalService testling = createService("alice@wonderland", "", "Alice In", "Wonderland");

			CPPUNIT_ASSERT_EQUAL(String("Alice In Wonderland"), testling.getDescription());
		}

		void testGetDescription_NoInfo() {
			LinkLocalService testling = createService("alice@wonderland");

			CPPUNIT_ASSERT_EQUAL(String("alice@wonderland"), testling.getDescription());
		}
	
	private:
		LinkLocalService createService(const String& name, const String& nickName = String(), const String& firstName = String(), const String& lastName = String()) {
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
