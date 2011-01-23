/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swift/Controllers/PreviousStatusStore.h"

using namespace Swift;

class PreviousStatusStoreTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(PreviousStatusStoreTest);
	CPPUNIT_TEST(testGetAll);
	//CPPUNIT_TEST(testGetAllLimited);
	//CPPUNIT_TEST(testGetSuggestionsInexact);
	//CPPUNIT_TEST(testGetSuggestionsExact);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {
		store_ = new PreviousStatusStore();
		store_->addStatus(StatusShow::Online, "At home in the study");
		store_->addStatus(StatusShow::DND, "In a meeting");
		store_->addStatus(StatusShow::DND, "With a client");
		store_->addStatus(StatusShow::Away, "Walking the elephant");
		store_->addStatus(StatusShow::Online, "In the office, at my desk");
	}

	void tearDown() {
		
	}

	void testGetAll() {

	}

private:
	PreviousStatusStore* store_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PreviousStatusStoreTest);
