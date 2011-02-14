/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Slimber/Menulet.h"
#include "Slimber/MenuletController.h"

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
			CPPUNIT_ASSERT_EQUAL(std::string("No online users"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("[Offline] "), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("*About*"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("*Restart*"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("*Exit*"), menulet->items[i++]);
		}

		void testUpdate() {
			MenuletController testling(menulet);

			testling.setXMPPStatus("You are connected", MenuletController::Online);

			CPPUNIT_ASSERT_EQUAL(8, static_cast<int>(menulet->items.size()));
			int i = 0;
			CPPUNIT_ASSERT_EQUAL(std::string("No online users"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("[Online] You are connected"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("*About*"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("*Restart*"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("*Exit*"), menulet->items[i++]);
		}

		void testSetXMPPStatus_Online() {
			MenuletController testling(menulet);

			testling.setXMPPStatus("You are connected", MenuletController::Online);

			int i = 0;
			CPPUNIT_ASSERT_EQUAL(std::string("No online users"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("[Online] You are connected"), menulet->items[i++]);
		}


		void testSetXMPPStatus_Offline() {
			MenuletController testling(menulet);

			testling.setXMPPStatus("You are not connected", MenuletController::Offline);

			int i = 0;
			CPPUNIT_ASSERT_EQUAL(std::string("No online users"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("-"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("[Offline] You are not connected"), menulet->items[i++]);
		}

		void testSetUserNames() {
			MenuletController testling(menulet);

			std::vector<std::string> users;
			users.push_back("Alice In Wonderland");
			users.push_back("The Mad Hatter");
			testling.setUserNames(users);

			int i = 0;
			CPPUNIT_ASSERT_EQUAL(std::string("Online users:"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("  Alice In Wonderland"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("  The Mad Hatter"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("-"), menulet->items[i++]);
		}

		void testSetUserNames_NoUsers() {
			MenuletController testling(menulet);

			std::vector<std::string> users;
			testling.setUserNames(users);

			int i = 0;
			CPPUNIT_ASSERT_EQUAL(std::string("No online users"), menulet->items[i++]);
			CPPUNIT_ASSERT_EQUAL(std::string("-"), menulet->items[i++]);
		}
	
	private:
		struct FakeMenulet : public Menulet {
			virtual void clear() {
				items.clear();
			}

			virtual void addItem(const std::string& name, const std::string& icon = std::string()) {
				std::string result;
				if (!icon.empty()) {
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

			virtual void setIcon(const std::string& i) {
				icon = i;
			}

			std::vector<std::string> items;
			std::string icon;
		};

		FakeMenulet* menulet;
};

CPPUNIT_TEST_SUITE_REGISTRATION(MenuletControllerTest);
