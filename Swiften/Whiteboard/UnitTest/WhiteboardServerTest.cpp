/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/Whiteboard/WhiteboardDeleteOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardEllipseElement.h>
#include <Swiften/Elements/Whiteboard/WhiteboardInsertOperation.h>
#include <Swiften/Elements/Whiteboard/WhiteboardUpdateOperation.h>
#include <Swiften/Whiteboard/WhiteboardServer.h>

using namespace Swift;

class WhiteboardServerTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(WhiteboardServerTest);
    CPPUNIT_TEST(testSimpleOp);
    CPPUNIT_TEST(testSimpleOp1);
    CPPUNIT_TEST(testSimpleOp2);
    CPPUNIT_TEST(testFewSimpleOps);
    CPPUNIT_TEST_SUITE_END();
public:
    void testSimpleOp() {
        WhiteboardServer server;
        WhiteboardInsertOperation::ref firstOp = std::make_shared<WhiteboardInsertOperation>();
        firstOp->setID("0");
        server.handleLocalOperationReceived(firstOp);
        WhiteboardInsertOperation::ref serverOp = std::make_shared<WhiteboardInsertOperation>();
        serverOp->setID("b");
        serverOp->setParentID("0");
        serverOp->setPos(1);
        server.handleLocalOperationReceived(serverOp);
        WhiteboardInsertOperation::ref clientOp = std::make_shared<WhiteboardInsertOperation>();
        WhiteboardEllipseElement::ref clientElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setID("a");
        clientOp->setParentID("0");
        clientOp->setPos(1);
        clientOp->setElement(clientElement);
        WhiteboardInsertOperation::ref op = std::dynamic_pointer_cast<WhiteboardInsertOperation>(server.handleClientOperationReceived(clientOp));
        CPPUNIT_ASSERT_EQUAL(std::string("b"), op->getParentID());
        CPPUNIT_ASSERT_EQUAL(std::string("a"), op->getID());
        CPPUNIT_ASSERT_EQUAL(1, op->getPos());
        CPPUNIT_ASSERT_EQUAL(clientElement, std::dynamic_pointer_cast<WhiteboardEllipseElement>(op->getElement()));
    }

    void testSimpleOp1() {
        WhiteboardServer server;
        WhiteboardInsertOperation::ref firstOp = std::make_shared<WhiteboardInsertOperation>();
        firstOp->setID("0");
        server.handleLocalOperationReceived(firstOp);
        WhiteboardDeleteOperation::ref serverOp = std::make_shared<WhiteboardDeleteOperation>();
        serverOp->setID("b");
        serverOp->setParentID("0");
        serverOp->setPos(1);
        server.handleLocalOperationReceived(serverOp);
        WhiteboardUpdateOperation::ref clientOp = std::make_shared<WhiteboardUpdateOperation>();
        WhiteboardEllipseElement::ref clientElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setID("a");
        clientOp->setParentID("0");
        clientOp->setPos(1);
        clientOp->setElement(clientElement);
        WhiteboardDeleteOperation::ref op = std::dynamic_pointer_cast<WhiteboardDeleteOperation>(server.handleClientOperationReceived(clientOp));
        CPPUNIT_ASSERT_EQUAL(std::string("b"), op->getParentID());
        CPPUNIT_ASSERT_EQUAL(std::string("a"), op->getID());
        CPPUNIT_ASSERT_EQUAL(-1, op->getPos());
    }

    void testSimpleOp2() {
        WhiteboardServer server;
        WhiteboardInsertOperation::ref firstOp = std::make_shared<WhiteboardInsertOperation>();
        firstOp->setID("0");
        server.handleLocalOperationReceived(firstOp);
        WhiteboardUpdateOperation::ref serverOp = std::make_shared<WhiteboardUpdateOperation>();
        serverOp->setID("b");
        serverOp->setParentID("0");
        serverOp->setPos(1);
        server.handleLocalOperationReceived(serverOp);
        WhiteboardDeleteOperation::ref clientOp = std::make_shared<WhiteboardDeleteOperation>();
        clientOp->setID("a");
        clientOp->setParentID("0");
        clientOp->setPos(1);
        WhiteboardDeleteOperation::ref op = std::dynamic_pointer_cast<WhiteboardDeleteOperation>(server.handleClientOperationReceived(clientOp));
        CPPUNIT_ASSERT_EQUAL(std::string("b"), op->getParentID());
        CPPUNIT_ASSERT_EQUAL(std::string("a"), op->getID());
        CPPUNIT_ASSERT_EQUAL(1, op->getPos());
    }


    void testFewSimpleOps() {
        WhiteboardServer server;
        WhiteboardInsertOperation::ref firstOp = std::make_shared<WhiteboardInsertOperation>();
        firstOp->setID("0");
        server.handleLocalOperationReceived(firstOp);
        WhiteboardInsertOperation::ref serverOp = std::make_shared<WhiteboardInsertOperation>();
        serverOp->setID("a");
        serverOp->setParentID("0");
        serverOp->setPos(1);
        server.handleLocalOperationReceived(serverOp);
        serverOp = std::make_shared<WhiteboardInsertOperation>();
        serverOp->setID("b");
        serverOp->setParentID("a");
        serverOp->setPos(2);
        server.handleLocalOperationReceived(serverOp);
        serverOp = std::make_shared<WhiteboardInsertOperation>();
        serverOp->setID("c");
        serverOp->setParentID("b");
        serverOp->setPos(3);
        server.handleLocalOperationReceived(serverOp);
        WhiteboardInsertOperation::ref clientOp = std::make_shared<WhiteboardInsertOperation>();
        WhiteboardEllipseElement::ref clientElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setID("d");
        clientOp->setParentID("0");
        clientOp->setPos(1);
        clientOp->setElement(clientElement);
        WhiteboardInsertOperation::ref op = std::dynamic_pointer_cast<WhiteboardInsertOperation>(server.handleClientOperationReceived(clientOp));
        CPPUNIT_ASSERT_EQUAL(std::string("c"), op->getParentID());
        CPPUNIT_ASSERT_EQUAL(std::string("d"), op->getID());
        CPPUNIT_ASSERT_EQUAL(1, op->getPos());
        CPPUNIT_ASSERT_EQUAL(clientElement, std::dynamic_pointer_cast<WhiteboardEllipseElement>(op->getElement()));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WhiteboardServerTest);
