/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QWidget>

#include <Swiften/TLS/Certificate.h>

namespace Swift {

class CocoaUIHelpers {
public:
    static void displayCertificateChainAsSheet(QWidget* parent, const std::vector<Certificate::ref>& chain);
    static void sendCocoaApplicationWillTerminateNotification();
};

}

