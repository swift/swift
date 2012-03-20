/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/Elements/Stanza.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Delay.h>

using namespace Swift;

class StanzaTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(StanzaTest);
		CPPUNIT_TEST(testConstructor_Copy);
		CPPUNIT_TEST(testGetPayload);
		CPPUNIT_TEST(testGetPayloads);
		CPPUNIT_TEST(testGetPayload_NoSuchPayload);
		CPPUNIT_TEST(testDestructor);
		CPPUNIT_TEST(testDestructor_Copy);
		CPPUNIT_TEST(testUpdatePayload_ExistingPayload);
		CPPUNIT_TEST(testUpdatePayload_NewPayload);
		CPPUNIT_TEST(testGetPayloadOfSameType);
		CPPUNIT_TEST(testGetPayloadOfSameType_NoSuchPayload);
		CPPUNIT_TEST(testGetTimestamp);
		CPPUNIT_TEST(testGetTimestamp_TimestampWithFrom);
		CPPUNIT_TEST(testGetTimestamp_NoDelay);
		CPPUNIT_TEST(testGetTimestampFrom);
		CPPUNIT_TEST(testGetTimestampFrom_Fallsback);
		CPPUNIT_TEST_SUITE_END();

	public:
		class MyPayload1 : public Payload {
			public:
				MyPayload1() {}
		};

		class MyPayload2 : public Payload {
			public:
				MyPayload2(const std::string& s = "") : text_(s) {}

				std::string text_;
		};

		class MyPayload3 : public Payload {
			public:
				MyPayload3() {}
		};

		class DestroyingPayload : public Payload {
			public:
				DestroyingPayload(bool* alive) : alive_(alive) {
				}

				~DestroyingPayload() {
					(*alive_) = false;
				}
			
			private:
				bool* alive_;
		};

		void testConstructor_Copy() {
			Message m;
			m.addPayload(boost::make_shared<MyPayload1>());
			m.addPayload(boost::make_shared<MyPayload2>());
			Message copy(m);

			CPPUNIT_ASSERT(copy.getPayload<MyPayload1>());
			CPPUNIT_ASSERT(copy.getPayload<MyPayload2>());
		}

		void testDestructor() {
			bool payloadAlive = true;
			{
				Message m;
				m.addPayload(boost::make_shared<DestroyingPayload>(&payloadAlive));
			}

			CPPUNIT_ASSERT(!payloadAlive);
		}

		void testDestructor_Copy() {
			bool payloadAlive = true;
			Message* m1 = new Message();
			m1->addPayload(boost::make_shared<DestroyingPayload>(&payloadAlive));
			Message* m2 = new Message(*m1);

			delete m1;
			CPPUNIT_ASSERT(payloadAlive);

			delete m2;
			CPPUNIT_ASSERT(!payloadAlive);
		}

		void testGetPayload() {
			Message m;
			m.addPayload(boost::make_shared<MyPayload1>());
			m.addPayload(boost::make_shared<MyPayload2>());
			m.addPayload(boost::make_shared<MyPayload3>());

			boost::shared_ptr<MyPayload2> p(m.getPayload<MyPayload2>());
			CPPUNIT_ASSERT(p);
		}

		void testGetPayload_NoSuchPayload() {
			Message m;
			m.addPayload(boost::make_shared<MyPayload1>());
			m.addPayload(boost::make_shared<MyPayload3>());

			boost::shared_ptr<MyPayload2> p(m.getPayload<MyPayload2>());
			CPPUNIT_ASSERT(!p);
		}

		void testGetPayloads() {
			Message m;
			boost::shared_ptr<MyPayload2> payload1(new MyPayload2());
			boost::shared_ptr<MyPayload2> payload2(new MyPayload2());
			m.addPayload(boost::make_shared<MyPayload1>());
			m.addPayload(payload1);
			m.addPayload(boost::make_shared<MyPayload3>());
			m.addPayload(payload2);

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), m.getPayloads<MyPayload2>().size());
			CPPUNIT_ASSERT_EQUAL(payload1, m.getPayloads<MyPayload2>()[0]);
			CPPUNIT_ASSERT_EQUAL(payload2, m.getPayloads<MyPayload2>()[1]);
		}


		void testUpdatePayload_ExistingPayload() {
			Message m;
			m.addPayload(boost::make_shared<MyPayload1>());
			m.addPayload(boost::make_shared<MyPayload2>("foo"));
			m.addPayload(boost::make_shared<MyPayload3>());

			m.updatePayload(boost::make_shared<MyPayload2>("bar"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), m.getPayloads().size());
			boost::shared_ptr<MyPayload2> p(m.getPayload<MyPayload2>());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), p->text_);
		}

		void testUpdatePayload_NewPayload() {
			Message m;
			m.addPayload(boost::make_shared<MyPayload1>());
			m.addPayload(boost::make_shared<MyPayload3>());

			m.updatePayload(boost::make_shared<MyPayload2>("bar"));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), m.getPayloads().size());
			boost::shared_ptr<MyPayload2> p(m.getPayload<MyPayload2>());
			CPPUNIT_ASSERT_EQUAL(std::string("bar"), p->text_);
		}

		void testGetPayloadOfSameType() {
			Message m;
			m.addPayload(boost::make_shared<MyPayload1>());
			m.addPayload(boost::make_shared<MyPayload2>("foo"));
			m.addPayload(boost::make_shared<MyPayload3>());

			boost::shared_ptr<MyPayload2> payload(boost::dynamic_pointer_cast<MyPayload2>(m.getPayloadOfSameType(boost::make_shared<MyPayload2>("bar"))));
			CPPUNIT_ASSERT(payload);
			CPPUNIT_ASSERT_EQUAL(std::string("foo"), payload->text_);
		}

		void testGetPayloadOfSameType_NoSuchPayload() {
			Message m;
			m.addPayload(boost::make_shared<MyPayload1>());
			m.addPayload(boost::make_shared<MyPayload3>());

			CPPUNIT_ASSERT(!m.getPayloadOfSameType(boost::make_shared<MyPayload2>("bar")));
		}

		void testGetTimestamp() {
			Message m;
			m.addPayload(boost::make_shared<Delay>(boost::posix_time::from_time_t(1)));

			boost::optional<boost::posix_time::ptime> timestamp = m.getTimestamp();

			CPPUNIT_ASSERT(timestamp);
			CPPUNIT_ASSERT_EQUAL(std::string("1970-Jan-01 00:00:01"), boost::posix_time::to_simple_string(*timestamp));
		}

		void testGetTimestamp_TimestampWithFrom() {
			Message m;
			m.addPayload(boost::make_shared<Delay>(boost::posix_time::from_time_t(1), JID("foo@bar.com")));

			boost::optional<boost::posix_time::ptime> timestamp = m.getTimestamp();

			CPPUNIT_ASSERT(timestamp);
			CPPUNIT_ASSERT_EQUAL(std::string("1970-Jan-01 00:00:01"), boost::posix_time::to_simple_string(*timestamp));
		}

		void testGetTimestamp_NoDelay() {
			Message m;
			CPPUNIT_ASSERT(!m.getTimestamp());
		}

		void testGetTimestampFrom() {
			Message m;
			m.addPayload(boost::make_shared<Delay>(boost::posix_time::from_time_t(0)));
			m.addPayload(boost::make_shared<Delay>(boost::posix_time::from_time_t(1), JID("foo1@bar.com")));
			m.addPayload(boost::make_shared<Delay>(boost::posix_time::from_time_t(2), JID("foo2@bar.com")));
			m.addPayload(boost::make_shared<Delay>(boost::posix_time::from_time_t(3), JID("foo3@bar.com")));

			boost::optional<boost::posix_time::ptime> timestamp = m.getTimestampFrom(JID("foo2@bar.com"));

			CPPUNIT_ASSERT(timestamp);
			CPPUNIT_ASSERT_EQUAL(std::string("1970-Jan-01 00:00:02"), boost::posix_time::to_simple_string(*timestamp));
		}

		void testGetTimestampFrom_Fallsback() {
			Message m;
			m.addPayload(boost::make_shared<Delay>(boost::posix_time::from_time_t(1), JID("foo1@bar.com")));
			m.addPayload(boost::make_shared<Delay>(boost::posix_time::from_time_t(3), JID("foo3@bar.com")));

			boost::optional<boost::posix_time::ptime> timestamp = m.getTimestampFrom(JID("foo2@bar.com"));

			CPPUNIT_ASSERT(timestamp);
			CPPUNIT_ASSERT_EQUAL(std::string("1970-Jan-01 00:00:01"), boost::posix_time::to_simple_string(*timestamp));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StanzaTest);
