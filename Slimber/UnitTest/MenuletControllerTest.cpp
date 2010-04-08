/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Slimber/Menulet.h"
#include "Slimber/MenuletController.h"

using namespace Swift;

class MenuletControllerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(MenuletControllerTest);
		CPPUNIT_TEST(testConstructor);
		CPPUNIT_TEST(testUpdate);
		CPPUNIT_TEST(testSetXMPPStatus_Online);
		CPPUNIT_TEST(testSetXMPPStatus_Offline);
		CPPUNIT_TEST(testSetUserNames);
		CPPUNIT_TEST(testSetUserNames_NoUsers);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			menulet = new FakeMenulet();
		}

		void tearDown() {
			delete menulet;
		}

		void testConstructor() {
			MenuletController testling(menulet);

			CPPUNIT_ASSERT_EQUAL(8, static_cast<int>(menulet->items.size()));
			int i = 0;
			CPPUNIT_ASSERT_EQUAL(String("No online users"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("[Offline] "), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("*About*"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("*Restart*"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("*Exit*"), menulet->items[i++]);
		}

		void testUpdate() {
			MenuletController testling(menulet);

			testling.setXMPPStatus("You are connected", MenuletController::Online);

			CPPUNIT_ASSERT_EQUAL(8, static_cast<int>(menulet->items.size()));
			int i = 0;
			CPPUNIT_ASSERT_EQUAL(String("No online users"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("[Online] You are connected"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("*About*"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("*Restart*"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("*Exit*"), menulet->items[i++]);
		}

		void testSetXMPPStatus_Online() {
			MenuletController testling(menulet);

			testling.setXMPPStatus("You are connected", MenuletController::Online);

			int i = 0;
			CPPUNIT_ASSERT_EQUAL(String("No online users"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("[Online] You are connected"), menulet->items[i++]);
		}


		void testSetXMPPStatus_Offline() {
			MenuletController testling(menulet);

			testling.setXMPPStatus("You are not connected", MenuletController::Offline);

			int i = 0;
			CPPUNIT_ASSERT_EQUAL(String("No online users"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("[Offline] You are not connected"), menulet->items[i++]);
		}

		void testSetUserNames() {
			MenuletController testling(menulet);

			std::vector<String> users;
			users.push_back("Alice In Wonderland");
			users.push_back("The Mad Hatter");
			testling.setUserNames(users);

			int i = 0;
			CPPUNIT_ASSERT_EQUAL(String("Online users:"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("  Alice In Wonderland"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("  The Mad Hatter"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("-"), menulet->items[i++]);
		}

		void testSetUserNames_NoUsers() {
			MenuletController testling(menulet);

			std::vector<String> users;
			testling.setUserNames(users);

			int i = 0;
			CPPUNIT_ASSERT_EQUAL(String("No online users"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(String("-"), menulet->items[i++]);
		}
	
	private:
		struct FakeMenulet : public Menulet {
			virtual void clear() {
				items.clear();
			}

			virtual void addItem(const String& name, const String& icon = String()) {
				String result;
				if (!icon.isEmpty()) {
					result += "[" + icon + "] ";
				}
				result += name;
				items.push_back(result);
			}

			virtual void addAboutItem() {
				items.push_back("*About*");
			}

			virtual void addRestartItem() {
				items.push_back("*Restart*");
			}

			virtual void addExitItem() {
				items.push_back("*Exit*");
			}

			virtual void addSeparator() {
				items.push_back("-");
			}

			virtual void setIcon(const String& i) {
				icon = i;
			}

			std::vector<String> items;
			String icon;
		};

		FakeMenulet* menulet;
};

CPPUNIT_TEST_SUITE_REGISTRATION(MenuletControllerTest);
