#include <vector>
#include <boost/bind.hpp>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Elements/Presence.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/StreamStack/XMPPLayer.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

using namespace Swift;

class XMPPLayerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(XMPPLayerTest);
		CPPUNIT_TEST(testParseData_Error);
		CPPUNIT_TEST(testResetParser);
		CPPUNIT_TEST(testResetParser_FromSlot);
		CPPUNIT_TEST(testWriteHeader);
		CPPUNIT_TEST(testWriteElement);
		CPPUNIT_TEST(testWriteFooter);
		CPPUNIT_TEST_SUITE_END();

	public:
		XMPPLayerTest() {}

		void setUp() {
			testling_ = new XMPPLayer(&parserFactories_, &serializers_);
			elementsReceived_ = 0;
			dataReceived_ = "";
			errorReceived_ = 0;
		}

		void tearDown() {
			delete testling_;
		}

		void testParseData_Error() {
			testling_->onError.connect(boost::bind(&XMPPLayerTest::handleError, this));

			testling_->parseData("<iq>");
			
			CPPUNIT_ASSERT_EQUAL(1, errorReceived_);
		}

		void testResetParser() {
			testling_->onElement.connect(boost::bind(&XMPPLayerTest::handleElement, this, _1));
			testling_->onError.connect(boost::bind(&XMPPLayerTest::handleError, this));

			testling_->parseData("<stream:stream to=\"example.com\" xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" >");
			testling_->resetParser();
			testling_->parseData("<stream:stream to=\"example.com\" xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" >");
			testling_->parseData("<presence/>");

			CPPUNIT_ASSERT_EQUAL(1, elementsReceived_);
			CPPUNIT_ASSERT_EQUAL(0, errorReceived_);
		}

		void testResetParser_FromSlot() {
			testling_->onElement.connect(boost::bind(&XMPPLayerTest::handleElementAndReset, this, _1));
			testling_->parseData("<stream:stream to=\"example.com\" xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" ><presence/>");
			testling_->parseData("<stream:stream to=\"example.com\" xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" ><presence/>");

			CPPUNIT_ASSERT_EQUAL(2, elementsReceived_);
			CPPUNIT_ASSERT_EQUAL(0, errorReceived_);
		}

		void testWriteHeader() {
			testling_->onWriteData.connect(boost::bind(&XMPPLayerTest::handleWriteData, this, _1));
			testling_->writeHeader("example.com");

			CPPUNIT_ASSERT_EQUAL(String("<?xml version=\"1.0\"?><stream:stream xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" version=\"1.0\" to=\"example.com\">"), dataReceived_);
		}

		void testWriteElement() {
			testling_->onWriteData.connect(boost::bind(&XMPPLayerTest::handleWriteData, this, _1));
			testling_->writeElement(boost::shared_ptr<Presence>(new Presence()));

			CPPUNIT_ASSERT_EQUAL(String("<presence/>"), dataReceived_);
		}

		void testWriteFooter() {
			testling_->onWriteData.connect(boost::bind(&XMPPLayerTest::handleWriteData, this, _1));
			testling_->writeFooter();

			CPPUNIT_ASSERT_EQUAL(String("</stream:stream>"), dataReceived_);
		}

		void handleElement(boost::shared_ptr<Element>) {
			++elementsReceived_;
		}

		void handleElementAndReset(boost::shared_ptr<Element>) {
			++elementsReceived_;
			testling_->resetParser();
		}

		void handleWriteData(ByteArray ba) {
			dataReceived_ += std::string(ba.getData(), ba.getSize());
		}

		void handleError() {
			++errorReceived_;
		}

	private:
		FullPayloadParserFactoryCollection parserFactories_;
		FullPayloadSerializerCollection serializers_;
		XMPPLayer* testling_;
		int elementsReceived_;
		String dataReceived_;
		int errorReceived_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(XMPPLayerTest);
