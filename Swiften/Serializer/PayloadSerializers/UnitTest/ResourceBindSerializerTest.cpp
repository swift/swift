/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/ResourceBindSerializer.h>

using namespace Swift;

class ResourceBindSerializerTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(ResourceBindSerializerTest);
        CPPUNIT_TEST(testSerialize_JID);
        CPPUNIT_TEST(testSerialize_Resource);
        CPPUNIT_TEST(testSerialize_Empty);
        CPPUNIT_TEST_SUITE_END();

    public:
        ResourceBindSerializerTest() {}

        void testSerialize_JID() {
            ResourceBindSerializer testling;
            std::shared_ptr<ResourceBind> resourceBind(new ResourceBind());
            resourceBind->setJID(JID("somenode@example.com/someresource"));

            CPPUNIT_ASSERT_EQUAL(std::string(
                "<bind xmlns=\"urn:ietf:params:xml:ns:xmpp-bind\">"
                    "<jid>somenode@example.com/someresource</jid>"
                "</bind>"), testling.serialize(resourceBind));
        }

        void testSerialize_Resource() {
            ResourceBindSerializer testling;
            std::shared_ptr<ResourceBind> resourceBind(new ResourceBind());
            resourceBind->setResource("someresource");

            CPPUNIT_ASSERT_EQUAL(std::string(
                "<bind xmlns=\"urn:ietf:params:xml:ns:xmpp-bind\">"
                    "<resource>someresource</resource>"
                "</bind>"), testling.serialize(resourceBind));
        }

        void testSerialize_Empty() {
            ResourceBindSerializer testling;
            std::shared_ptr<ResourceBind> resourceBind(new ResourceBind());

            CPPUNIT_ASSERT_EQUAL(std::string("<bind xmlns=\"urn:ietf:params:xml:ns:xmpp-bind\"/>"), testling.serialize(resourceBind));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ResourceBindSerializerTest);
