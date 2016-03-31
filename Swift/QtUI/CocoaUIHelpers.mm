/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "CocoaUIHelpers.h"

#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>

#include <Cocoa/Cocoa.h>

#include <Security/Security.h>
#include <SecurityInterface/SFCertificatePanel.h>

#include <Swiften/Base/foreach.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

namespace Swift {

void CocoaUIHelpers::displayCertificateChainAsSheet(QWidget* parent, const std::vector<Certificate::ref>& chain) {
    NSWindow* parentWindow = [((NSView*)parent->winId()) window];
    NSMutableArray* certificates = [[NSMutableArray alloc] init];
    foreach(Certificate::ref cert, chain) {
        // convert chain to SecCertificateRef
        ByteArray certAsDER = cert->toDER();
        boost::shared_ptr<boost::remove_pointer<CFDataRef>::type> certData(CFDataCreate(NULL, certAsDER.data(), certAsDER.size()), CFRelease);
        boost::shared_ptr<OpaqueSecCertificateRef> macCert(SecCertificateCreateWithData(NULL, certData.get()), CFRelease);

        // add to NSMutable array
        [certificates addObject: (id)macCert.get()];
    }


    SFCertificatePanel* panel = [[SFCertificatePanel alloc] init];
    //[panel setPolicies:(id)policies.get()];
    [panel beginSheetForWindow:parentWindow modalDelegate:nil didEndSelector:NULL contextInfo:NULL certificates:certificates showGroup:YES];
    [certificates release];
}

}
