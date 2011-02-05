/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/smart_ptr/make_shared.hpp>

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
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			channel_ = new DummyStanzaChannel();
			router_ = new IQRouter(channel_);
			xmppRoster_ = new XMPPRosterImpl();
		}

		void tearDown() {
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
			payload->addItem(RosterItemPayload(JID("foo@bar.com"), "Bob", RosterItemPayload::Both));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "eou", payload));

			CPPUNIT_ASSERT(xmppRoster_->containsJID(JID("foo@bar.com")));
			CPPUNIT_ASSERT_EQUAL(String("Bob"), xmppRoster_->getNameForJID(JID("foo@bar.com")));
		}

		void testModify() {
			XMPPRosterController controller(router_, xmppRoster_);
			boost::shared_ptr<RosterPayload> payload1(new RosterPayload());
			payload1->addItem(RosterItemPayload(JID("foo@bar"), "Bob", RosterItemPayload::Both));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id1", payload1));

			boost::shared_ptr<RosterPayload> payload2(new RosterPayload());
			payload2->addItem(RosterItemPayload(JID("foo@bar"), "Bob2", RosterItemPayload::Both));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id2", payload2));

			CPPUNIT_ASSERT_EQUAL(String("Bob2"), xmppRoster_->getNameForJID(JID("foo@bar")));
		}

		void testRemove() {
			XMPPRosterController controller(router_, xmppRoster_);
			boost::shared_ptr<RosterPayload> payload1(new RosterPayload());
			payload1->addItem(RosterItemPayload(JID("foo@bar"), "Bob", RosterItemPayload::Both));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id1", payload1));

			boost::shared_ptr<RosterPayload> payload2(new RosterPayload());
			payload2->addItem(RosterItemPayload(JID("foo@bar"), "Bob", RosterItemPayload::Remove));
			channel_->onIQReceived(IQ::createRequest(IQ::Set, JID(), "id2", payload2));
			CPPUNIT_ASSERT(!xmppRoster_->containsJID(JID("foo@bar")));
		}

	private:
		DummyStanzaChannel* channel_;
		IQRouter* router_;
		XMPPRosterImpl* xmppRoster_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(XMPPRosterControllerTest);


