/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/ContainerPayload.h>
#include <Swiften/Parser/PayloadParsers/PubSubRetractParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class PubSubRetractParserTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(PubSubRetractParserTest);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST(testParseNotify);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testParse() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse("<pubsub xmlns='http://jabber.org/protocol/pubsub'>"
                                            "<retract node='princely_musings' xmlns='http://jabber.org/protocol/pubsub'>"
                                                "<item id='ae890ac52d0df67ed7cfdf51b644e901' xmlns='http://jabber.org/protocol/pubsub'/>"
                                            "</retract>"
                                        "</pubsub>"));

            auto payload = parser.getPayload<ContainerPayload<PubSubPayload>>();
            std::shared_ptr<PubSubRetract> retract = std::dynamic_pointer_cast<PubSubRetract>(payload->getPayload());
            CPPUNIT_ASSERT(retract);
            CPPUNIT_ASSERT_EQUAL(std::string("princely_musings"), retract->getNode());
            CPPUNIT_ASSERT_EQUAL(false, retract->isNotify().is_initialized());
        }

        void testParseNotify() {
            {
                PayloadsParserTester parser;

                CPPUNIT_ASSERT(parser.parse("<pubsub xmlns='http://jabber.org/protocol/pubsub'>"
                                                "<retract node='princely_musings' notify='true' xmlns='http://jabber.org/protocol/pubsub'>"
                                                    "<item id='ae890ac52d0df67ed7cfdf51b644e901' xmlns='http://jabber.org/protocol/pubsub'/>"
                                                "</retract>"
                                            "</pubsub>"));

                auto payload = parser.getPayload<ContainerPayload<PubSubPayload>>();
                std::shared_ptr<PubSubRetract> retract = std::dynamic_pointer_cast<PubSubRetract>(payload->getPayload());
                CPPUNIT_ASSERT(retract);
                CPPUNIT_ASSERT_EQUAL(std::string("princely_musings"), retract->getNode());
                CPPUNIT_ASSERT_EQUAL(true, retract->isNotify().get());
            }

            {
                PayloadsParserTester parser;
                CPPUNIT_ASSERT(parser.parse("<pubsub xmlns='http://jabber.org/protocol/pubsub'>"
                                                "<retract node='princely_musings' notify='0' xmlns='http://jabber.org/protocol/pubsub'>"
                                                    "<item id='ae890ac52d0df67ed7cfdf51b644e901' xmlns='http://jabber.org/protocol/pubsub'/>"
                                                "</retract>"
                                            "</pubsub>"));

                auto payload = parser.getPayload<ContainerPayload<PubSubPayload>>();
                auto retract = std::dynamic_pointer_cast<PubSubRetract>(payload->getPayload());
                CPPUNIT_ASSERT(retract);
                CPPUNIT_ASSERT_EQUAL(std::string("princely_musings"), retract->getNode());
                CPPUNIT_ASSERT_EQUAL(false, retract->isNotify().get());
            }
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PubSubRetractParserTest);
