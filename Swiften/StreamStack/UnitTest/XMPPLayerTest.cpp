/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <vector>
#include <boost/bind.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/StreamStack/XMPPLayer.h>
#include <Swiften/StreamStack/LowLayer.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

class XMPPLayerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(XMPPLayerTest);
		CPPUNIT_TEST(testParseData_Error);
		CPPUNIT_TEST(testResetParser);
		CPPUNIT_TEST(testResetParser_FromSlot);
		CPPUNIT_TEST(testWriteHeader);
		CPPUNIT_TEST(testWriteElement);
		CPPUNIT_TEST(testWriteFooter);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			lowLayer_ = new DummyLowLayer();
			testling_ = new XMPPLayerExposed(&parserFactories_, &serializers_, &xmlParserFactory_, ClientStreamType);
			testling_->setChildLayer(lowLayer_);
			elementsReceived_ = 0;
			errorReceived_ = 0;
		}

		void tearDown() {
			delete testling_;
			delete lowLayer_;
		}

		void testParseData_Error() {
			testling_->onError.connect(boost::bind(&XMPPLayerTest::handleError, this));

			testling_->handleDataRead(createSafeByteArray("<iq>"));
			
			CPPUNIT_ASSERT_EQUAL(1, errorReceived_);
		}

		void testResetParser() {
			testling_->onElement.connect(boost::bind(&XMPPLayerTest::handleElement, this, _1));
			testling_->onError.connect(boost::bind(&XMPPLayerTest::handleError, this));

			testling_->handleDataRead(createSafeByteArray("<stream:stream to=\"example.com\" xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" >"));
			testling_->resetParser();
			testling_->handleDataRead(createSafeByteArray("<stream:stream to=\"example.com\" xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" >"));
			testling_->handleDataRead(createSafeByteArray("<presence/>"));

			CPPUNIT_ASSERT_EQUAL(1, elementsReceived_);
			CPPUNIT_ASSERT_EQUAL(0, errorReceived_);
		}

		void testResetParser_FromSlot() {
			testling_->onElement.connect(boost::bind(&XMPPLayerTest::handleElementAndReset, this, _1));
			testling_->handleDataRead(createSafeByteArray("<stream:stream to=\"example.com\" xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" ><presence/>"));
			testling_->handleDataRead(createSafeByteArray("<stream:stream to=\"example.com\" xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" ><presence/>"));

			CPPUNIT_ASSERT_EQUAL(2, elementsReceived_);
			CPPUNIT_ASSERT_EQUAL(0, errorReceived_);
		}

		void testWriteHeader() {
			ProtocolHeader header;
			header.setTo("example.com");
			testling_->writeHeader(header);

			CPPUNIT_ASSERT_EQUAL(std::string("<?xml version=\"1.0\"?><stream:stream xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" to=\"example.com\" version=\"1.0\">"), lowLayer_->writtenData);
		}

		void testWriteElement() {
			testling_->writeElement(boost::make_shared<Presence>());

			CPPUNIT_ASSERT_EQUAL(std::string("<presence/>"), lowLayer_->writtenData);
		}

		void testWriteFooter() {
			testling_->writeFooter();

			CPPUNIT_ASSERT_EQUAL(std::string("</stream:stream>"), lowLayer_->writtenData);
		}

		void handleElement(boost::shared_ptr<Element>) {
			++elementsReceived_;
		}

		void handleElementAndReset(boost::shared_ptr<Element>) {
			++elementsReceived_;
			testling_->resetParser();
		}

		void handleError() {
			++errorReceived_;
		}

	private:
		class XMPPLayerExposed : public XMPPLayer {
			public:
				XMPPLayerExposed(
								PayloadParserFactoryCollection* payloadParserFactories,
								PayloadSerializerCollection* payloadSerializers,
								XMLParserFactory* xmlParserFactory,
								StreamType streamType) : XMPPLayer(payloadParserFactories, payloadSerializers, xmlParserFactory, streamType) {}

				using XMPPLayer::handleDataRead;
				using HighLayer::setChildLayer;
		};

		class DummyLowLayer : public LowLayer {
			public:
				virtual void writeData(const SafeByteArray& data) {
					writtenData += byteArrayToString(ByteArray(data.begin(), data.end()));
				}
				
				std::string writtenData;
		};

		FullPayloadParserFactoryCollection parserFactories_;
		FullPayloadSerializerCollection serializers_;
		DummyLowLayer* lowLayer_;
		XMPPLayerExposed* testling_;
		PlatformXMLParserFactory xmlParserFactory_;
		int elementsReceived_;
		int errorReceived_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(XMPPLayerTest);
