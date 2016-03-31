/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/EventLoop/SimpleEventLoop.h>

#include <Slimber/CLI/DummyMenulet.h>
#include <Slimber/MainController.h>

using namespace Swift;

int main() {
    SimpleEventLoop eventLoop;

    DummyMenulet menulet;
    MainController controller(&menulet, &eventLoop);

    eventLoop.run();
    return 0;
}
