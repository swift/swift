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
#include <Swiften/Whiteboard/WhiteboardClient.h>

using namespace Swift;

class WhiteboardClientTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(WhiteboardClientTest);
    CPPUNIT_TEST(testSynchronize_simplestSync);
    CPPUNIT_TEST(testSynchronize_withoutTranslation);
    CPPUNIT_TEST(testSynchronize_nonInterrupted);
    CPPUNIT_TEST(testSynchronize_clientInterruption);
    CPPUNIT_TEST(testSynchronize_serverInterruption);
    CPPUNIT_TEST(testSynchronize_nonInterruptedMixOperations);
    CPPUNIT_TEST(testSynchronize_nonInterruptedMixOperations2);
    CPPUNIT_TEST_SUITE_END();
public:

    /*!
     *  /\
     *  \/
     *   \
     */
    void testSynchronize_simplestSync() {
        WhiteboardClient client;
        WhiteboardInsertOperation::ref serverOp;
        serverOp = createInsertOperation("0", "", 0);
        WhiteboardClient::Result pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(serverOp, std::dynamic_pointer_cast<WhiteboardInsertOperation>(pairResult.client));
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives first local operation, because it's parented off "0" which exists
        //in server history and client doesn't wait for any operation ack from server,
        //so this operation could be send
        WhiteboardInsertOperation::ref clientOp;
        clientOp = createInsertOperation("a", "0", 1);
        WhiteboardEllipseElement::ref aElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(aElement);
        WhiteboardInsertOperation::ref result;
        checkOperation(client.handleLocalOperationReceived(clientOp), "a", "0", 1, aElement);

        //Client receives server operation parented off "0", which isn't last client operation
        //so it have to be transformed against local operations and then transformed value can
        //be returned to draw
        serverOp = createInsertOperation("b", "0", 1);
        WhiteboardEllipseElement::ref bElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverOp->setElement(bElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.client, "b", "a", 2, bElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives confirmation from the server about processed "a" operation, it had to
        //be transformed against "b" on the server side to receive operation parented off "b".
        //There aren't any waiting operations to send to server, this operation should return
        //nothing
        serverOp = createInsertOperation("a", "b", 1);
        pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives local operation, it doesn't have to be transformed against anything
        //but operation returned to send to the server should be parented off last server
        //operation, which is "b"
        clientOp = createInsertOperation("c", "b", 3);
        WhiteboardEllipseElement::ref cElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(cElement);
        checkOperation(client.handleLocalOperationReceived(clientOp), "c", "a", 3, cElement);

        //Client receives confirmation from the server about processed "a" operation, it
        //should be the same operation as it was sent because server didn't have to
        //transform it
        clientOp = createInsertOperation("c", "a", 3);
        clientOp->setElement(cElement);
        pairResult = client.handleServerOperationReceived(clientOp);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Results:
        //Client operations:
        //ID    pos
        //0        0
        //a        1
        //b        2
        //c        3
        //
        //Server operations:
        //ID    pos
        //0        0
        //b        1
        //a        1
        //c        3
        //
        //what gives 0abc on both sides
    }

    /*!
     *    /
     *   /
     *   \
     */
    void testSynchronize_withoutTranslation() {
        WhiteboardClient client;
        WhiteboardInsertOperation::ref serverOp;
        serverOp = createInsertOperation("0", "", 0);
        WhiteboardClient::Result pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(serverOp, std::dynamic_pointer_cast<WhiteboardInsertOperation>(pairResult.client));
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives first local operation, because it's parented off "0" which exists
        //in server history and client doesn't wait for any operation ack from server,
        //so this operation could be send
        WhiteboardInsertOperation::ref clientOp = createInsertOperation("c", "0", 1);
        WhiteboardEllipseElement::ref cElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(cElement);
        checkOperation(client.handleLocalOperationReceived(clientOp), "c", "0", 1, cElement);

        //Client receives second local operation, client didn't receive ack about previous
        //operation from the server so it can't be send.
        clientOp = createInsertOperation("d", "c", 2);
        WhiteboardEllipseElement::ref dElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(dElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), client.handleLocalOperationReceived(clientOp));

        //Client receives confirmation about processing "c" operation, it should be the
        //same as sent operation because it wasn't transformed, client could send now
        //operation "d"
        clientOp = createInsertOperation("c", "0", 1);
        clientOp->setElement(cElement);
        pairResult = client.handleServerOperationReceived(clientOp);
        checkOperation(pairResult.server, "d", "c", 2, dElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);

        //Client receives confirmation about processing "d", it should be the same as
        //sent operation. There aren't any operations in queue to send.
        clientOp = createInsertOperation("d", "c", 2);
        clientOp->setElement(dElement);
        pairResult = client.handleServerOperationReceived(clientOp);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives new operation from server, it's parented off "d" which is at
        //the end of local history so it doesn't have to be transformed, so operation
        //to pass to window should be the same
        serverOp = createInsertOperation("e", "d", 3);
        WhiteboardEllipseElement::ref eElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverOp->setElement(eElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        WhiteboardInsertOperation::ref result = std::dynamic_pointer_cast<WhiteboardInsertOperation>(pairResult.client);
        CPPUNIT_ASSERT_EQUAL(serverOp, std::dynamic_pointer_cast<WhiteboardInsertOperation>(pairResult.client));
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);


        //Client operations:
        //ID    pos
        //0        0
        //c        1
        //d        2
        //e        3
        //
        //Server operations:
        //ID    pos
        //0        0
        //c        1
        //d        2
        //e        3
    }

    /*!
     *     /\
     *    /  \
     *    \  /
     *     \/
     */
    void testSynchronize_nonInterrupted() {
        WhiteboardClient client;
        WhiteboardInsertOperation::ref serverOp;
        serverOp = createInsertOperation("0", "", 0);
        WhiteboardClient::Result pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(serverOp, std::dynamic_pointer_cast<WhiteboardInsertOperation>(pairResult.client));
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives first local operation, because it's parented off "0" which exists
        //in server history and client doesn't wait for any operation ack from server,
        //so this operation could be send
        WhiteboardInsertOperation::ref clientOp;
        clientOp = createInsertOperation("a", "0", 1);
        WhiteboardEllipseElement::ref aElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(aElement);
        checkOperation(client.handleLocalOperationReceived(clientOp), "a", "0", 1, aElement);

        //Client receives second local operation, client didn't receive ack about previous
        //operation from the server so it can't be send.
        clientOp = createInsertOperation("b", "a", 2);
        WhiteboardEllipseElement::ref bElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(bElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), client.handleLocalOperationReceived(clientOp));

        //Client receives new operation from server, it should be transformed against
        //"a" and "b" before adding to local operations history because it's parented off "0".
        //Because client is waiting for ack of "a", there is no operation to send to server
        serverOp = createInsertOperation("c", "0", 1);
        WhiteboardEllipseElement::ref cElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverOp->setElement(cElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.client, "c", "b", 3, cElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives new operation from server, it should be transformed against
        //results of previous transformations, returned operation should be parented off
        //"c" existing in local history.
        //Because client is waiting for ack of "a", there is no operation to send to server
        serverOp = createInsertOperation("d", "c", 2);
        WhiteboardEllipseElement::ref dElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverOp->setElement(dElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.client, "d", "c", 4, dElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives confirmation about processing "a", it should send next operation
        //to server which is "b", but it should be version parented of transformed "a"
        serverOp = createInsertOperation("a", "d", 1);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.server, "b", "a", 2, bElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);


        //Client receives confirmation about processing "b", there aren't any operations
        //waiting so it should return nothing.
        serverOp = createInsertOperation("b", "a", 2);
        pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client operations:
        //ID    pos
        //0     0
        //a     1
        //b     2
        //c     3
        //d     4
        //
        //Server operations:
        //ID    pos
        //0     0
        //c     1
        //d     2
        //a     1
        //b     2
        //
        //what gives 0abcd on both sides.
    }

    /*!
     *     /\
     *    /  \
     *    \  /
     *    / /
     *    \/
     */
    void testSynchronize_clientInterruption() {
        WhiteboardClient client;
        WhiteboardInsertOperation::ref serverOp;
        serverOp = createInsertOperation("0", "", 0);
        WhiteboardClient::Result pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(serverOp, std::dynamic_pointer_cast<WhiteboardInsertOperation>(pairResult.client));
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives first local operation, because it's parented off "0" which exists
        //in server history and client doesn't wait for any operation ack from server,
        //so this operation could be send
        WhiteboardInsertOperation::ref clientOp;
        clientOp = createInsertOperation("a", "0", 1);
        WhiteboardEllipseElement::ref aElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(aElement);
        checkOperation(client.handleLocalOperationReceived(clientOp), "a", "0", 1, aElement);

        //Client receives second local operation, client didn't receive ack about previous
        //operation from the server so it can't be send.
        clientOp = createInsertOperation("b", "a", 2);
        WhiteboardEllipseElement::ref bElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(bElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), client.handleLocalOperationReceived(clientOp));

        //Client receives new operation from server, it should be transformed against
        //"a" and "b" before adding to local operations history because it's parented off "0".
        //Because client is waiting for ack of "a", there is no operation to send to server
        serverOp = createInsertOperation("c", "0", 1);
        WhiteboardEllipseElement::ref cElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverOp->setElement(cElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.client, "c", "b", 3, cElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives new local operation, client is still waiting for ack so, it
        //should return nothing
        clientOp = createInsertOperation("e", "a", 4);
        WhiteboardEllipseElement::ref eElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(eElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), client.handleLocalOperationReceived(clientOp));

        //Client receives new server operation, to add it to local history it should be transformed
        //against result of previous transformations and operation "e", returned operation should
        //be parented off "e", which was last local operation
        serverOp = createInsertOperation("d", "c", 2);
        WhiteboardEllipseElement::ref dElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverOp->setElement(dElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.client, "d", "e", 5, dElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives confirmation about processing "a", it had to be transformed against
        //"c" and "d" and it is now parented off "d", returned value should be next operation
        //which have to be send to server("b" parented off server version of "a").
        serverOp = createInsertOperation("a", "d", 1);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.server, "b", "a", 2, bElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);

        //Client receives confirmation about processing "b", it is the same operation as sent because
        //it didn't have to be transformed, returned value should be next operation
        //which have to be send to server("e" parented off server version of "b").
        serverOp = createInsertOperation("b", "a", 2);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.server, "e", "b", 4, eElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);

        //Client receives confirmation about processing "b", it is the same operation as sent because
        //it didn't have to be transformed, there aren't any operations to send so this function returns
        //nothing
        serverOp = createInsertOperation("e", "b", 4);
        pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Result:
        //Client operations:
        //ID    pos
        //0     0
        //a     1
        //b     2
        //c     3
        //e     4
        //d     5
        //
        //Server operations:
        //0     0
        //c     1
        //d     2
        //a     1
        //b     2
        //e     4
        //what gives 0abced on both sides
    }

    /*!
     *    /\
     *   / /
     *   \ \
     *    \/
     */
    void testSynchronize_serverInterruption() {
        WhiteboardClient client;
        WhiteboardInsertOperation::ref serverOp;
        serverOp = createInsertOperation("0", "", 0);
        WhiteboardClient::Result pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(serverOp, std::dynamic_pointer_cast<WhiteboardInsertOperation>(pairResult.client));
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives first local operation, because it's parented off "0" which exists
        //in server history and client doesn't wait for any operation ack from server,
        //so this operation could be send
        WhiteboardInsertOperation::ref clientOp;
        clientOp = createInsertOperation("a", "0", 1);
        WhiteboardEllipseElement::ref aElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(aElement);
        checkOperation(client.handleLocalOperationReceived(clientOp), "a", "0", 1, aElement);

        //Client receives second local operation, client didn't receive ack about previous
        //operation from the server so it can't be send.
        clientOp = createInsertOperation("b", "a", 2);
        WhiteboardEllipseElement::ref bElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(bElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), client.handleLocalOperationReceived(clientOp));

        //Client receives new operation from server, it should be transformed against
        //"a" and "b" before adding to local operations history because it's parented off "0".
        //Because client is waiting for ack of "a", there is no operation to send to server
        serverOp = createInsertOperation("c", "0", 1);
        WhiteboardEllipseElement::ref cElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverOp->setElement(cElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.client, "c", "b", 3, cElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives confirmation about processing "a", it had to be transformed against
        //"c" and it is now parented off "c", returned value should be next operation
        //which have to be send to server("b" parented off server version of "a").
        serverOp = createInsertOperation("a", "c", 1);
        serverOp->setElement(aElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.server, "b", "a", 2, bElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);

        //Client receives new server operation, to add it to local history it should be transformed
        //against result of previous transformation(but only with transformation of "b"), returned
        //operation should be parented off "c", which was last local operation
        serverOp = createInsertOperation("d", "a", 3);
        WhiteboardEllipseElement::ref dElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverOp->setElement(dElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.client, "d", "c", 4, dElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives confirmation about processing "b", it had to be transformed against
        //"d" because both operations was parented off server version of "a".
        //there aren't any operations to send so this function returns nothing.
        serverOp = createInsertOperation("b", "d", 2);
        serverOp->setElement(bElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client operations:
        //ID    pos
        //0     0
        //a     1
        //b     2
        //c     3
        //d     4
        //
        //Server operations:
        //ID    pos
        //0     0
        //c     1
        //a     1
        //d     3
        //b     2
        //
        //what gives 0abcd on both sides

    }

    /*!
     *     /\
     *    /  \
     *    \  /
     *     \/
     */
    void testSynchronize_nonInterruptedMixOperations() {
        WhiteboardClient client;
        WhiteboardInsertOperation::ref serverOp;
        serverOp = createInsertOperation("0", "", 0);
        WhiteboardClient::Result pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(serverOp, std::dynamic_pointer_cast<WhiteboardInsertOperation>(pairResult.client));
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives first local operation, because it's parented off "0" which exists
        //in server history and client doesn't wait for any operation ack from server,
        //so this operation could be send
        WhiteboardInsertOperation::ref clientOp;
        clientOp = createInsertOperation("a", "0", 1);
        WhiteboardEllipseElement::ref aElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientOp->setElement(aElement);
        checkOperation(client.handleLocalOperationReceived(clientOp), "a", "0", 1, aElement);

        //Client receives second local operation, client didn't receive ack about previous
        //operation from the server so it can't be send.
        WhiteboardUpdateOperation::ref clientUpdateOp = createUpdateOperation("b", "a", 0);
        WhiteboardEllipseElement::ref bElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientUpdateOp->setElement(bElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), client.handleLocalOperationReceived(clientUpdateOp));

        //Client receives new operation from server, it should be transformed against
        //"a" and "b" before adding to local operations history because it's parented off "0".
        //Because client is waiting for ack of "a", there is no operation to send to server
        WhiteboardUpdateOperation::ref serverUpdateOp = createUpdateOperation("c", "0", 0);
        WhiteboardEllipseElement::ref cElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverUpdateOp->setElement(cElement);
        pairResult = client.handleServerOperationReceived(serverUpdateOp);
        checkOperation(pairResult.client, "c", "b", 0, cElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives new operation from server, it should be transformed against
        //results of previous transformations, returned operation should be parented off
        //"c" existing in local history.
        //Because client is waiting for ack of "a", there is no operation to send to server
        serverOp = createInsertOperation("d", "c", 1);
        WhiteboardEllipseElement::ref dElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverOp->setElement(dElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.client, "d", "c", 2, dElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives confirmation about processing "a", it should send next operation
        //to server which is "b", but it should be version parented of transformed "a"
        serverOp = createInsertOperation("a", "d", 1);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.server, "b", "a", 0, cElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);


        //Client receives confirmation about processing "b", there aren't any operations
        //waiting so it should return nothing.
        serverUpdateOp = createUpdateOperation("b", "a", 0);
        pairResult = client.handleServerOperationReceived(serverUpdateOp);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client operations:
        //ID    pos
        //0     0
        //a     1
        //b     2
        //c     3
        //d     4
        //
        //Server operations:
        //ID    pos
        //0     0
        //c     1
        //d     2
        //a     1
        //b     2
        //
        //what gives 0abcd on both sides.
    }

    /*!
     *     /\
     *    /  \
     *    \  /
     *     \/
     */
    void testSynchronize_nonInterruptedMixOperations2() {
        WhiteboardClient client;
        WhiteboardInsertOperation::ref serverOp;
        serverOp = createInsertOperation("0", "", 0);
        WhiteboardClient::Result pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(serverOp, std::dynamic_pointer_cast<WhiteboardInsertOperation>(pairResult.client));
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        serverOp = createInsertOperation("1", "0", 1);
        pairResult = client.handleServerOperationReceived(serverOp);
        CPPUNIT_ASSERT_EQUAL(serverOp, std::dynamic_pointer_cast<WhiteboardInsertOperation>(pairResult.client));
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);
        //Client receives first local operation, because it's parented off "0" which exists
        //in server history and client doesn't wait for any operation ack from server,
        //so this operation could be send
        WhiteboardInsertOperation::ref clientOp;
        WhiteboardUpdateOperation::ref clientUpdateOp;
        WhiteboardDeleteOperation::ref clientDeleteOp;
        clientUpdateOp = createUpdateOperation("a", "1", 0);
        WhiteboardEllipseElement::ref aElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        clientUpdateOp->setElement(aElement);
        checkOperation(client.handleLocalOperationReceived(clientUpdateOp), "a", "1", 0, aElement);

        //Client receives second local operation, client didn't receive ack about previous
        //operation from the server so it can't be send.
        clientDeleteOp = createDeleteOperation("b", "a", 1);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), client.handleLocalOperationReceived(clientDeleteOp));

        //Client receives new operation from server, it should be transformed against
        //"a" and "b" before adding to local operations history because it's parented off "0".
        //Because client is waiting for ack of "a", there is no operation to send to server
        serverOp = createInsertOperation("c", "1", 2);
        WhiteboardEllipseElement::ref cElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverOp->setElement(cElement);
        pairResult = client.handleServerOperationReceived(serverOp);
        checkOperation(pairResult.client, "c", "b", 1, cElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives new operation from server, it should be transformed against
        //results of previous transformations, returned operation should be parented off
        //"c" existing in local history.
        //Because client is waiting for ack of "a", there is no operation to send to server
        WhiteboardUpdateOperation::ref serverUpdateOp = createUpdateOperation("d", "c", 0);
        WhiteboardEllipseElement::ref dElement = std::make_shared<WhiteboardEllipseElement>(0,0,0,0);
        serverUpdateOp->setElement(dElement);
        pairResult = client.handleServerOperationReceived(serverUpdateOp);
        checkOperation(pairResult.client, "d", "c", 0, dElement);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client receives confirmation about processing "a", it should send next operation
        //to server which is "b", but it should be version parented of transformed "a"
        serverUpdateOp = createUpdateOperation("a", "d", 0);
        pairResult = client.handleServerOperationReceived(serverUpdateOp);
        checkOperation(pairResult.server, "b", "a", 1);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);


        //Client receives confirmation about processing "b", there aren't any operations
        //waiting so it should return nothing.
        WhiteboardDeleteOperation::ref serverDeleteOp = createDeleteOperation("b", "a", 0);
        pairResult = client.handleServerOperationReceived(serverDeleteOp);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.client);
        CPPUNIT_ASSERT_EQUAL(WhiteboardOperation::ref(), pairResult.server);

        //Client operations:
        //ID    pos
        //0     0
        //a     1
        //b     2
        //c     3
        //d     4
        //
        //Server operations:
        //ID    pos
        //0     0
        //c     1
        //d     2
        //a     1
        //b     2
        //
        //what gives 0abcd on both sides.
    }


    WhiteboardInsertOperation::ref createInsertOperation(std::string id, std::string parent, int pos) {
        WhiteboardInsertOperation::ref operation = std::make_shared<WhiteboardInsertOperation>();
        operation->setParentID(parent);
        operation->setID(id);
        operation->setPos(pos);
        return operation;
    }

    WhiteboardUpdateOperation::ref createUpdateOperation(std::string id, std::string parent, int pos) {
        WhiteboardUpdateOperation::ref operation = std::make_shared<WhiteboardUpdateOperation>();
        operation->setParentID(parent);
        operation->setID(id);
        operation->setPos(pos);
        return operation;
    }

    WhiteboardDeleteOperation::ref createDeleteOperation(std::string id, std::string parent, int pos) {
        WhiteboardDeleteOperation::ref operation = std::make_shared<WhiteboardDeleteOperation>();
        operation->setParentID(parent);
        operation->setID(id);
        operation->setPos(pos);
        return operation;
    }

    void checkOperation(WhiteboardOperation::ref operation, std::string id, std::string parent, int pos = -1, WhiteboardElement::ref element = WhiteboardElement::ref()) {
        CPPUNIT_ASSERT_EQUAL(id, operation->getID());
        CPPUNIT_ASSERT_EQUAL(parent, operation->getParentID());
        if (pos != -1) {
            CPPUNIT_ASSERT_EQUAL(pos, operation->getPos());
        }

        if (element) {
            WhiteboardInsertOperation::ref insertOp = std::dynamic_pointer_cast<WhiteboardInsertOperation>(operation);
            if (insertOp) {
                CPPUNIT_ASSERT_EQUAL(element, insertOp->getElement());
            }

            WhiteboardUpdateOperation::ref updateOp = std::dynamic_pointer_cast<WhiteboardUpdateOperation>(operation);
            if (updateOp) {
                CPPUNIT_ASSERT_EQUAL(element, updateOp->getElement());
            }
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WhiteboardClientTest);
