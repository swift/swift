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

#include "CocoaUIHelpers.h"

#include <memory>

#include <boost/type_traits.hpp>

#include <Cocoa/Cocoa.h>

#include <Security/Security.h>
#include <SecurityInterface/SFCertificatePanel.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

void CocoaUIHelpers::displayCertificateChainAsSheet(QWidget* parent, const std::vector<Certificate::ref>& chain) {
    NSWindow* parentWindow = [((NSView*)parent->winId()) window];
    NSMutableArray* certificates = [[NSMutableArray alloc] init];
    for (auto&& cert : chain) {
        // convert chain to SecCertificateRef
        ByteArray certAsDER = cert->toDER();
        std::shared_ptr<boost::remove_pointer<CFDataRef>::type> certData(CFDataCreate(nullptr, certAsDER.data(), certAsDER.size()), CFRelease);
        std::shared_ptr<OpaqueSecCertificateRef> macCert(SecCertificateCreateWithData(nullptr, certData.get()), CFRelease);

        // add to NSMutable array
        [certificates addObject: (id)macCert.get()];
    }

    SFCertificatePanel* panel = [[SFCertificatePanel alloc] init];
    //[panel setPolicies:(id)policies.get()];
    [panel beginSheetForWindow:parentWindow modalDelegate:nil didEndSelector:nullptr contextInfo:nullptr certificates:certificates showGroup:YES];
    [certificates release];
}

void CocoaUIHelpers::sendCocoaApplicationWillTerminateNotification() {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"NSApplicationWillTerminateNotification" object:nil];
}

}
