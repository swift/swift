/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <vector>
#include <boost/bind.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/StreamStack/StreamStack.h"
#include "Swiften/StreamStack/LowLayer.h"
#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/StreamStack/StreamLayer.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

using namespace Swift;

class StreamStackTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(StreamStackTest);
		CPPUNIT_TEST(testWriteData_NoIntermediateStreamStack);
		CPPUNIT_TEST(testWriteData_OneIntermediateStream);
		CPPUNIT_TEST(testWriteData_TwoIntermediateStreamStack);
		CPPUNIT_TEST(testReadData_NoIntermediateStreamStack);
		CPPUNIT_TEST(testReadData_OneIntermediateStream);
		CPPUNIT_TEST(testReadData_TwoIntermediateStreamStack);
		CPPUNIT_TEST(testAddLayer_ExistingOnWriteDataSlot);
		CPPUNIT_TEST_SUITE_END();

	public:
		StreamStackTest() {}

		void setUp() {
			physicalStream_ = boost::shared_ptr<TestLowLayer>(new TestLowLayer());
			xmppStream_ = boost::shared_ptr<XMPPLayer>(new XMPPLayer(&parserFactories_, &serializers_));
			elementsReceived_ = 0;
			dataWriteReceived_ = 0;
		}

		void tearDown() {
		}

		void testWriteData_NoIntermediateStreamStack() {
			StreamStack testling(xmppStream_, physicalStream_);
			
			xmppStream_->writeData("foo");

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), physicalStream_->data_.size());
			CPPUNIT_ASSERT_EQUAL(ByteArray("foo"), physicalStream_->data_[0]);
		}

		void testWriteData_OneIntermediateStream() {
			StreamStack testling(xmppStream_, physicalStream_);
			boost::shared_ptr<MyStreamLayer> xStream(new MyStreamLayer("X"));
			testling.addLayer(xStream);

			xmppStream_->writeData("foo");

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), physicalStream_->data_.size());
			CPPUNIT_ASSERT_EQUAL(ByteArray("Xfoo"), physicalStream_->data_[0]);
		}

		void testWriteData_TwoIntermediateStreamStack() {
			StreamStack testling(xmppStream_, physicalStream_);
			boost::shared_ptr<MyStreamLayer> xStream(new MyStreamLayer("X"));
			boost::shared_ptr<MyStreamLayer> yStream(new MyStreamLayer("Y"));
			testling.addLayer(xStream);
			testling.addLayer(yStream);

			xmppStream_->writeData("foo");

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), physicalStream_->data_.size());
			CPPUNIT_ASSERT_EQUAL(ByteArray("XYfoo"), physicalStream_->data_[0]);
		}

		void testReadData_NoIntermediateStreamStack() {
			StreamStack testling(xmppStream_, physicalStream_);
			xmppStream_->onElement.connect(boost::bind(&StreamStackTest::handleElement, this, _1));
			
			physicalStream_->onDataRead(ByteArray("<stream:stream xmlns:stream='http://etherx.jabber.org/streams'><presence/>"));

			CPPUNIT_ASSERT_EQUAL(1, elementsReceived_);
		}

		void testReadData_OneIntermediateStream() {
			StreamStack testling(xmppStream_, physicalStream_);
			xmppStream_->onElement.connect(boost::bind(&StreamStackTest::handleElement, this, _1));
			boost::shared_ptr<MyStreamLayer> xStream(new MyStreamLayer("<"));
			testling.addLayer(xStream);

			physicalStream_->onDataRead(ByteArray("stream:stream xmlns:stream='http://etherx.jabber.org/streams'><presence/>"));

			CPPUNIT_ASSERT_EQUAL(1, elementsReceived_);
		}

		void testReadData_TwoIntermediateStreamStack() {
			StreamStack testling(xmppStream_, physicalStream_);
			xmppStream_->onElement.connect(boost::bind(&StreamStackTest::handleElement, this, _1));
			boost::shared_ptr<MyStreamLayer> xStream(new MyStreamLayer("s"));
			boost::shared_ptr<MyStreamLayer> yStream(new MyStreamLayer("<"));
			testling.addLayer(xStream);
			testling.addLayer(yStream);

			physicalStream_->onDataRead(ByteArray("tream:stream xmlns:stream='http://etherx.jabber.org/streams'><presence/>"));

			CPPUNIT_ASSERT_EQUAL(1, elementsReceived_);
		}

		void testAddLayer_ExistingOnWriteDataSlot() {
			StreamStack testling(xmppStream_, physicalStream_);
			xmppStream_->onWriteData.connect(boost::bind(&StreamStackTest::handleWriteData, this, _1));
			boost::shared_ptr<MyStreamLayer> xStream(new MyStreamLayer("X"));
			testling.addLayer(xStream);

			xmppStream_->writeData("foo");

			CPPUNIT_ASSERT_EQUAL(1, dataWriteReceived_);
		}

		void handleElement(boost::shared_ptr<Element>) {
			++elementsReceived_;
		}

		void handleWriteData(ByteArray) {
			++dataWriteReceived_;
		}

	private:
		class MyStreamLayer : public StreamLayer {
			public:
				MyStreamLayer(const String& prepend) : prepend_(prepend) {
				}

				virtual void writeData(const ByteArray& data) {
					onWriteData(ByteArray(prepend_) + data);
				}

				virtual void handleDataRead(const ByteArray& data) {
					onDataRead(ByteArray(prepend_) + data);
				}

			private:
				String prepend_;
		};

		class TestLowLayer : public LowLayer {
			public:
				TestLowLayer() {
				}

				virtual void writeData(const ByteArray& data) {
					data_.push_back(data);
				}

				std::vector<ByteArray> data_;
		};
	
	private:
		FullPayloadParserFactoryCollection parserFactories_;
		FullPayloadSerializerCollection serializers_;
		boost::shared_ptr<TestLowLayer> physicalStream_;
		boost::shared_ptr<XMPPLayer> xmppStream_;
		int elementsReceived_;
		int dataWriteReceived_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(StreamStackTest);
