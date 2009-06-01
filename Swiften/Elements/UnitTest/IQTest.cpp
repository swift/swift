#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/SoftwareVersion.h"

using namespace Swift;

class IQTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(IQTest);
		CPPUNIT_TEST(testCreateResult);
		CPPUNIT_TEST(testCreateResult_WithoutPayload);
		CPPUNIT_TEST(testCreateError);
		CPPUNIT_TEST_SUITE_END();

	public:
		IQTest() {}

		void testCreateResult() {
			boost::shared_ptr<Payload> payload(new SoftwareVersion("myclient"));
			boost::shared_ptr<IQ> iq(IQ::createResult(JID("foo@bar/fum"), "myid", payload));

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar/fum"), iq->getTo());
			CPPUNIT_ASSERT_EQUAL(String("myid"), iq->getID());
			CPPUNIT_ASSERT(iq->getPayload<SoftwareVersion>());
			CPPUNIT_ASSERT(payload == iq->getPayload<SoftwareVersion>());
		}

		void testCreateResult_WithoutPayload() {
			boost::shared_ptr<IQ> iq(IQ::createResult(JID("foo@bar/fum"), "myid"));

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar/fum"), iq->getTo());
			CPPUNIT_ASSERT_EQUAL(String("myid"), iq->getID());
			CPPUNIT_ASSERT(!iq->getPayload<SoftwareVersion>());
		}

		void testCreateError() {
			boost::shared_ptr<IQ> iq(IQ::createError(JID("foo@bar/fum"), "myid", Error::BadRequest, Error::Modify));

			CPPUNIT_ASSERT_EQUAL(JID("foo@bar/fum"), iq->getTo());
			CPPUNIT_ASSERT_EQUAL(String("myid"), iq->getID());
			boost::shared_ptr<Error> error(iq->getPayload<Error>());
			CPPUNIT_ASSERT(error);
			CPPUNIT_ASSERT_EQUAL(Error::BadRequest, error->getCondition());
			CPPUNIT_ASSERT_EQUAL(Error::Modify, error->getType());
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION(IQTest);
