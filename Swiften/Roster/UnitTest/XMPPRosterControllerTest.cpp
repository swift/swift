/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/smart_ptr/make_shared.hpp>

#include "Swiften/Roster/UnitTest/XMPPRosterSignalHandler.h"
#include "Swiften/Roster/XMPPRosterController.h"
#include "Swiften/Elements/Payload.h"
#include "Swiften/Elements/RosterItemPayload.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Client/DummyStanzaChannel.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Roster/XMPPRosterImpl.h"

using namespace Swift;

class XMPPRosterControllerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(XMPPRosterControllerTest);
		CPPUNIT_TEST(testGet_EmptyResponse);
		CPPUNIT_TEST(testAdd);
		CPPUNIT_TEST(testModify);
		CPPUNIT_TEST(testRemove);
		CPPUNIT_TEST(testMany);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			channel_ = new DummyStanzaChannel();
			router_ = new IQRouter(channel_);
			xmppRoster_ = new XMPPRosterImpl();
			handler_ = new XMPPRosterSignalHandler(xmppRoster_);
			jid1_ = JID("foo@bar.com");
			jid2_ = JID("alice@wonderland.lit");
			jid3_ = JID("jane@austen.lit");
		}

		void tearDown() {
			delete handler_;
			delete xmppRoster_;
			delete router_;
			delete channel_;
		}

		void testGet_EmptyResponse() {
			XMPPRosterController controller(router_, xmppRoster_);

			controller.requestRoster();

			channel_->onIQReceived(IQ::createResult(JID("baz@fum.com/dum"), channel_->sentStanzas[0]->getID(), boost::shared_ptr<RosterPayload>()));
		}

		void testAdd() {
			XMPPRosterController controller(router_, xmppRoster_);

			boost::shared_ptr<RosterPayload> payload(new RosterPayload());
			payload->addItem(RosterItemPayload(jid1_, "Bob", RosterItemPayload::Both));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "eou", payload));

			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), xmppRoster_->getGroupsForJID(jid1_).size());
			CPPUNIT_ASSERT(xmppRoster_->containsJID(jid1_));
			CPPUNIT_ASSERT_EQUAL(std::string("Bob"), xmppRoster_->getNameForJID(jid1_));
		}

		void testModify() {
			XMPPRosterController controller(router_, xmppRoster_);
			boost::shared_ptr<RosterPayload> payload1(new RosterPayload());
			payload1->addItem(RosterItemPayload(jid1_, "Bob", RosterItemPayload::Both));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id1", payload1));

			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			handler_->reset();

			boost::shared_ptr<RosterPayload> payload2(new RosterPayload());
			payload2->addItem(RosterItemPayload(jid1_, "Bob2", RosterItemPayload::Both));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id2", payload2));

			CPPUNIT_ASSERT_EQUAL(Update, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());

			CPPUNIT_ASSERT_EQUAL(std::string("Bob2"), xmppRoster_->getNameForJID(jid1_));
		}

		void testRemove() {
			XMPPRosterController controller(router_, xmppRoster_);
			boost::shared_ptr<RosterPayload> payload1(new RosterPayload());
			payload1->addItem(RosterItemPayload(jid1_, "Bob", RosterItemPayload::Both));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id1", payload1));

			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			handler_->reset();

			boost::shared_ptr<RosterPayload> payload2(new RosterPayload());
			payload2->addItem(RosterItemPayload(jid1_, "Bob", RosterItemPayload::Remove));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id2", payload2));
			CPPUNIT_ASSERT(!xmppRoster_->containsJID(jid1_));
			CPPUNIT_ASSERT_EQUAL(Remove, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());

		}

		void testMany() {
			XMPPRosterController controller(router_, xmppRoster_);
			boost::shared_ptr<RosterPayload> payload1(new RosterPayload());
			payload1->addItem(RosterItemPayload(jid1_, "Bob", RosterItemPayload::Both));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id1", payload1));

			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			handler_->reset();

			boost::shared_ptr<RosterPayload> payload2(new RosterPayload());
			payload2->addItem(RosterItemPayload(jid2_, "Alice", RosterItemPayload::Both));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id2", payload2));

			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid2_, handler_->getLastJID());
			handler_->reset();

			boost::shared_ptr<RosterPayload> payload3(new RosterPayload());
			payload3->addItem(RosterItemPayload(jid1_, "Ernie", RosterItemPayload::Both));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id3", payload3));

			CPPUNIT_ASSERT_EQUAL(Update, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			handler_->reset();

			boost::shared_ptr<RosterPayload> payload4(new RosterPayload());
			RosterItemPayload item(jid3_, "Jane", RosterItemPayload::Both);
			std::string janesGroup("Jane's Group");
			item.addGroup(janesGroup);
			payload4->addItem(item);
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id4", payload4));

			CPPUNIT_ASSERT_EQUAL(Add, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid3_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), xmppRoster_->getGroupsForJID(jid3_).size());
			CPPUNIT_ASSERT_EQUAL(janesGroup, xmppRoster_->getGroupsForJID(jid3_)[0]);
			handler_->reset();

			boost::shared_ptr<RosterPayload> payload5(new RosterPayload());
			payload5->addItem(RosterItemPayload(jid1_, "Bob", RosterItemPayload::Remove));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id5", payload5));
			CPPUNIT_ASSERT(!xmppRoster_->containsJID(jid1_));
			CPPUNIT_ASSERT_EQUAL(Remove, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid1_, handler_->getLastJID());
			handler_->reset();

			boost::shared_ptr<RosterPayload> payload6(new RosterPayload());
			RosterItemPayload item2(jid2_, "Little Alice", RosterItemPayload::Both);
			std::string alicesGroup("Alice's Group");
			item2.addGroup(alicesGroup);
			payload6->addItem(item2);
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id6", payload6));
			CPPUNIT_ASSERT_EQUAL(Update, handler_->getLastEvent());
			CPPUNIT_ASSERT_EQUAL(jid2_, handler_->getLastJID());
			CPPUNIT_ASSERT_EQUAL(std::string("Little Alice"), xmppRoster_->getNameForJID(jid2_));
			CPPUNIT_ASSERT_EQUAL(std::string("Jane"), xmppRoster_->getNameForJID(jid3_));
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), xmppRoster_->getGroupsForJID(jid2_).size());
			CPPUNIT_ASSERT_EQUAL(alicesGroup, xmppRoster_->getGroupsForJID(jid2_)[0]);
			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), xmppRoster_->getGroupsForJID(jid3_).size());
			CPPUNIT_ASSERT_EQUAL(janesGroup, xmppRoster_->getGroupsForJID(jid3_)[0]);
			handler_->reset();

		}

	private:
		DummyStanzaChannel* channel_;
		IQRouter* router_;
		XMPPRosterImpl* xmppRoster_;
		XMPPRosterSignalHandler* handler_;
		JID jid1_;
		JID jid2_;
		JID jid3_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(XMPPRosterControllerTest);


