/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swift/Controllers/Chat/MUCController.h"

using namespace Swift;

class MUCControllerTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(MUCControllerTest);
	CPPUNIT_TEST(testJoinPartStringContructionSimple);
	CPPUNIT_TEST(testJoinPartStringContructionMixed);
	CPPUNIT_TEST(testAppendToJoinParts);
	CPPUNIT_TEST_SUITE_END();

public:
	MUCControllerTest() {};

	void setUp() {
	};

	void tearDown() {
	}

	void checkEqual(const std::vector<NickJoinPart>& expected, const std::vector<NickJoinPart>& actual) {
		CPPUNIT_ASSERT_EQUAL(expected.size(), actual.size());
		for (size_t i = 0; i < expected.size(); i++) {
			CPPUNIT_ASSERT_EQUAL(expected[i].nick, actual[i].nick);
			CPPUNIT_ASSERT_EQUAL(expected[i].type, actual[i].type);
		}
	}

	void testAppendToJoinParts() {
		std::vector<NickJoinPart> list;
		std::vector<NickJoinPart> gold;
		MUCController::appendToJoinParts(list, NickJoinPart("Kev", Join));
		gold.push_back(NickJoinPart("Kev", Join));
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Remko", Join));
		gold.push_back(NickJoinPart("Remko", Join));
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Bert", Join));
		gold.push_back(NickJoinPart("Bert", Join));
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Bert", Part));
		gold[2].type = JoinThenPart;
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Kev", Part));
		gold[0].type = JoinThenPart;
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Remko", Part));
		gold[1].type = JoinThenPart;
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Ernie", Part));
		gold.push_back(NickJoinPart("Ernie", Part));
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Ernie", Join));
		gold[3].type = PartThenJoin;
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Kev", Join));
		gold[0].type = Join;
		checkEqual(gold, list);
		MUCController::appendToJoinParts(list, NickJoinPart("Ernie", Part));
		gold[3].type = Part;
		checkEqual(gold, list);

	}

	void testJoinPartStringContructionSimple() {
		std::vector<NickJoinPart> list;
		list.push_back(NickJoinPart("Kev", Join));
		CPPUNIT_ASSERT_EQUAL(String("Kev has joined the room."), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Remko", Part));
		CPPUNIT_ASSERT_EQUAL(String("Kev has joined the room and Remko has left the room."), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Bert", Join));
		CPPUNIT_ASSERT_EQUAL(String("Kev has joined the room, Remko has left the room and Bert has joined the room."), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Ernie", Join));
		CPPUNIT_ASSERT_EQUAL(String("Kev has joined the room, Remko has left the room, Bert has joined the room and Ernie has joined the room."), MUCController::generateJoinPartString(list));
	}

	void testJoinPartStringContructionMixed() {
		std::vector<NickJoinPart> list;
		list.push_back(NickJoinPart("Kev", JoinThenPart));
		CPPUNIT_ASSERT_EQUAL(String("Kev joined then left the room."), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Remko", Part));
		CPPUNIT_ASSERT_EQUAL(String("Kev joined then left the room and Remko has left the room."), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Bert", PartThenJoin));
		CPPUNIT_ASSERT_EQUAL(String("Kev joined then left the room, Remko has left the room and Bert left then rejoined the room."), MUCController::generateJoinPartString(list));
		list.push_back(NickJoinPart("Ernie", JoinThenPart));
		CPPUNIT_ASSERT_EQUAL(String("Kev joined then left the room, Remko has left the room, Bert left then rejoined the room and Ernie joined then left the room."), MUCController::generateJoinPartString(list));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(MUCControllerTest);

