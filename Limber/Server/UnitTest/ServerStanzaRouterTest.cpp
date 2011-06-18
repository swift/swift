/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Elements/Message.h"
#include "Limber/Server/ServerStanzaRouter.h"
#include "Limber/Server/ServerSession.h"

using namespace Swift;

class ServerStanzaRouterTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ServerStanzaRouterTest);
		CPPUNIT_TEST(testRouteStanza_FullJID);
		CPPUNIT_TEST(testRouteStanza_FullJIDWithNegativePriority);
		CPPUNIT_TEST(testRouteStanza_FullJIDWithOnlyBareJIDMatchingSession);
		CPPUNIT_TEST(testRouteStanza_BareJIDWithoutMatchingSession);
		CPPUNIT_TEST(testRouteStanza_BareJIDWithMultipleSessions);
		CPPUNIT_TEST(testRouteStanza_BareJIDWithOnlyNegativePriorities);
		CPPUNIT_TEST(testRouteStanza_BareJIDWithChangingPresence);
		CPPUNIT_TEST_SUITE_END();

	public:
		ServerStanzaRouterTest() {}

		void setUp() {
		}

		void tearDown() {
		}

		void testRouteStanza_FullJID() {
			ServerStanzaRouter testling;
			MockServerSession session1(JID("foo@bar.com/Bla"), 0);
			testling.addClientSession(&session1);
			MockServerSession session2(JID("foo@bar.com/Baz"), 0);
			testling.addClientSession(&session2);

			bool result = testling.routeStanza(createMessageTo("foo@bar.com/Baz"));

			CPPUNIT_ASSERT(result);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(session1.sentStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(session2.sentStanzas.size()));
		}

		void testRouteStanza_FullJIDWithNegativePriority() {
			ServerStanzaRouter testling;
			MockServerSession session1(JID("foo@bar.com/Bla"), -1);
			testling.addClientSession(&session1);
			MockServerSession session2(JID("foo@bar.com/Baz"), 0);
			testling.addClientSession(&session2);

			bool result = testling.routeStanza(createMessageTo("foo@bar.com/Bla"));

			CPPUNIT_ASSERT(result);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(session1.sentStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(session2.sentStanzas.size()));
		}

		void testRouteStanza_FullJIDWithOnlyBareJIDMatchingSession() {
			ServerStanzaRouter testling;
			MockServerSession session(JID("foo@bar.com/Bla"), 0);
			testling.addClientSession(&session);

			bool result = testling.routeStanza(createMessageTo("foo@bar.com/Baz"));

			CPPUNIT_ASSERT(result);
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(session.sentStanzas.size()));
		}

		void testRouteStanza_BareJIDWithoutMatchingSession() {
			ServerStanzaRouter testling;

			bool result = testling.routeStanza(createMessageTo("foo@bar.com"));

			CPPUNIT_ASSERT(!result);
		}

		void testRouteStanza_BareJIDWithMultipleSessions() {
			ServerStanzaRouter testling;
			MockServerSession session1(JID("foo@bar.com/Bla"), 1);
			testling.addClientSession(&session1);
			MockServerSession session2(JID("foo@bar.com/Baz"), 8);
			testling.addClientSession(&session2);
			MockServerSession session3(JID("foo@bar.com/Bar"), 5);
			testling.addClientSession(&session3);

			bool result = testling.routeStanza(createMessageTo("foo@bar.com"));

			CPPUNIT_ASSERT(result);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(session1.sentStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(session2.sentStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(session3.sentStanzas.size()));
		}

		void testRouteStanza_BareJIDWithOnlyNegativePriorities() {
			ServerStanzaRouter testling;
			MockServerSession session(JID("foo@bar.com/Bla"), -1);
			testling.addClientSession(&session);

			bool result = testling.routeStanza(createMessageTo("foo@bar.com"));

			CPPUNIT_ASSERT(!result);
		}

		void testRouteStanza_BareJIDWithChangingPresence() {
			ServerStanzaRouter testling;
			MockServerSession session1(JID("foo@bar.com/Baz"), 8);
			testling.addClientSession(&session1);
			MockServerSession session2(JID("foo@bar.com/Bar"), 5);
			testling.addClientSession(&session2);

			session1.priority = 3;
			session2.priority = 4;
			bool result = testling.routeStanza(createMessageTo("foo@bar.com"));

			CPPUNIT_ASSERT(result);
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(session1.sentStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(session2.sentStanzas.size()));
		}

	private:
		boost::shared_ptr<Message> createMessageTo(const std::string& recipient) {
			boost::shared_ptr<Message> message(new Message());
			message->setTo(JID(recipient));
			return message;
		}

		class MockServerSession : public ServerSession {
			public:
				MockServerSession(const JID& jid, int priority) : jid(jid), priority(priority) {}

				virtual const JID& getJID() const { return jid; }
				virtual int getPriority() const { return priority; }

				virtual void sendStanza(boost::shared_ptr<Stanza> stanza) {
					sentStanzas.push_back(stanza);
				}

				JID jid;
				int priority;
				std::vector< boost::shared_ptr<Stanza> > sentStanzas;
		};
};

CPPUNIT_TEST_SUITE_REGISTRATION(ServerStanzaRouterTest);
