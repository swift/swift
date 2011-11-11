/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/DeliveryReceiptSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/DeliveryReceiptRequestSerializer.h>

using namespace Swift;

class DeliveryReceiptSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(DeliveryReceiptSerializerTest);
		CPPUNIT_TEST(testSerialize_XEP0184Example3);
		CPPUNIT_TEST(testSerialize_XEP0184Example4);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerialize_XEP0184Example3() {
			std::string expected =	"<request xmlns=\"urn:xmpp:receipts\"/>";

			DeliveryReceiptRequest::ref receipt = boost::make_shared<DeliveryReceiptRequest>();

			boost::shared_ptr<DeliveryReceiptRequestSerializer> serializer = boost::make_shared<DeliveryReceiptRequestSerializer>();
			CPPUNIT_ASSERT_EQUAL(expected, serializer->serializePayload(receipt));
		}

		void testSerialize_XEP0184Example4() {
			std::string expected =	"<received id=\"richard2-4.1.247\" xmlns=\"urn:xmpp:receipts\"/>";

			DeliveryReceipt::ref receipt = boost::make_shared<DeliveryReceipt>("richard2-4.1.247");

			boost::shared_ptr<DeliveryReceiptSerializer> serializer = boost::make_shared<DeliveryReceiptSerializer>();
			CPPUNIT_ASSERT_EQUAL(expected, serializer->serializePayload(receipt));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(DeliveryReceiptSerializerTest);

