/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>
#include <vector>

#include <boost/bind.hpp>

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/StreamStack/LowLayer.h>
#include <Swiften/StreamStack/StreamLayer.h>
#include <Swiften/StreamStack/StreamStack.h>
#include <Swiften/StreamStack/XMPPLayer.h>

using namespace Swift;

class StreamStackTest : public CppUnit::TestFixture {
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
        void setUp() {
            testling_ = std::make_unique<StreamStack>(std::make_unique<XMPPLayer>(&parserFactories_, &serializers_, &xmlParserFactory_, ClientStreamType), std::make_unique<TestLowLayer>());
            physicalStream_ = testling_->getLayer<TestLowLayer>();
            xmppStream_ = testling_->getLayer<XMPPLayer>();
            elementsReceived_ = 0;
            dataWriteReceived_ = 0;
        }

        void tearDown() {
        }

        void testWriteData_NoIntermediateStreamStack() {

            xmppStream_->writeData("foo");

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), physicalStream_->data_.size());
            CPPUNIT_ASSERT_EQUAL(createSafeByteArray("foo"), physicalStream_->data_[0]);
        }

        void testWriteData_OneIntermediateStream() {
            std::unique_ptr<MyStreamLayer> xStream(new MyStreamLayer("X"));
            testling_->addLayer(std::move(xStream));

            xmppStream_->writeData("foo");

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), physicalStream_->data_.size());
            CPPUNIT_ASSERT_EQUAL(createSafeByteArray("Xfoo"), physicalStream_->data_[0]);
        }

        void testWriteData_TwoIntermediateStreamStack() {
            std::unique_ptr<MyStreamLayer> xStream(new MyStreamLayer("X"));
            std::unique_ptr<MyStreamLayer> yStream(new MyStreamLayer("Y"));
            testling_->addLayer(std::move(xStream));
            testling_->addLayer(std::move(yStream));

            xmppStream_->writeData("foo");

            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), physicalStream_->data_.size());
            CPPUNIT_ASSERT_EQUAL(createSafeByteArray("XYfoo"), physicalStream_->data_[0]);
        }

        void testReadData_NoIntermediateStreamStack() {
            xmppStream_->onElement.connect(boost::bind(&StreamStackTest::handleElement, this, _1));

            physicalStream_->onDataRead(createSafeByteArray("<stream:stream xmlns:stream='http://etherx.jabber.org/streams'><presence/>"));

            CPPUNIT_ASSERT_EQUAL(1, elementsReceived_);
        }

        void testReadData_OneIntermediateStream() {
            xmppStream_->onElement.connect(boost::bind(&StreamStackTest::handleElement, this, _1));
            std::unique_ptr<MyStreamLayer> xStream(new MyStreamLayer("<"));
            testling_->addLayer(std::move(xStream));

            physicalStream_->onDataRead(createSafeByteArray("stream:stream xmlns:stream='http://etherx.jabber.org/streams'><presence/>"));

            CPPUNIT_ASSERT_EQUAL(1, elementsReceived_);
        }

        void testReadData_TwoIntermediateStreamStack() {
            xmppStream_->onElement.connect(boost::bind(&StreamStackTest::handleElement, this, _1));
            std::unique_ptr<MyStreamLayer> xStream(new MyStreamLayer("s"));
            std::unique_ptr<MyStreamLayer> yStream(new MyStreamLayer("<"));
            testling_->addLayer(std::move(xStream));
            testling_->addLayer(std::move(yStream));

            physicalStream_->onDataRead(createSafeByteArray("tream:stream xmlns:stream='http://etherx.jabber.org/streams'><presence/>"));

            CPPUNIT_ASSERT_EQUAL(1, elementsReceived_);
        }

        void testAddLayer_ExistingOnWriteDataSlot() {
            xmppStream_->onWriteData.connect(boost::bind(&StreamStackTest::handleWriteData, this, _1));
            std::unique_ptr<MyStreamLayer> xStream(new MyStreamLayer("X"));
            testling_->addLayer(std::move(xStream));

            xmppStream_->writeData("foo");

            CPPUNIT_ASSERT_EQUAL(1, dataWriteReceived_);
        }

        void handleElement(std::shared_ptr<ToplevelElement>) {
            ++elementsReceived_;
        }

        void handleWriteData(const SafeByteArray&) {
            ++dataWriteReceived_;
        }

    private:
        class MyStreamLayer : public StreamLayer {
            public:
                MyStreamLayer(const std::string& prepend) : prepend_(prepend) {
                }

                virtual void writeData(const SafeByteArray& data) {
                    writeDataToChildLayer(concat(createSafeByteArray(prepend_), data));
                }

                virtual void handleDataRead(const SafeByteArray& data) {
                    writeDataToParentLayer(concat(createSafeByteArray(prepend_), data));
                }

            private:
                std::string prepend_;
        };

        class TestLowLayer : public LowLayer {
            public:
                TestLowLayer() {
                }

                virtual void writeData(const SafeByteArray& data) {
                    data_.push_back(data);
                }

                void onDataRead(const SafeByteArray& data) {
                    writeDataToParentLayer(data);
                }

                std::vector<SafeByteArray> data_;
        };


    private:
        FullPayloadParserFactoryCollection parserFactories_;
        FullPayloadSerializerCollection serializers_;
        TestLowLayer* physicalStream_;
        PlatformXMLParserFactory xmlParserFactory_;
        XMPPLayer* xmppStream_;
        std::unique_ptr<StreamStack> testling_;
        int elementsReceived_;
        int dataWriteReceived_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(StreamStackTest);
