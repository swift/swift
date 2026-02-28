/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ITunesInterface.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfour-char-constants"
#import <Sluift/iTunes.h>
#pragma clang diagnostic pop
#include <ScriptingBridge/ScriptingBridge.h>

#include <memory>
#include <boost/optional.hpp>
#include <SwifTools/Cocoa/CocoaUtil.h>

using namespace Swift;

struct ITunesInterface::Private {
    Private() : iTunes(nil) {
    }

    iTunesApplication* iTunes;
};

ITunesInterface::ITunesInterface() : p(new Private()) {
}

ITunesInterface::~ITunesInterface() {
}

boost::optional<ITunesInterface::Track> ITunesInterface::getCurrentTrack() const {
    if (!haveApplication()) {
        return boost::optional<ITunesInterface::Track>();
    }
    iTunesTrack* currentTrack = p->iTunes.currentTrack;
    if (!currentTrack) {
        return boost::optional<ITunesInterface::Track>();
    }
    ITunesInterface::Track result;
    result.name = ns2StdString(currentTrack.name);
    result.artist = ns2StdString(currentTrack.artist);
    result.album = ns2StdString(currentTrack.album);
    result.trackNumber = currentTrack.trackNumber;
    result.duration = currentTrack.duration;
    result.rating = currentTrack.rating;
    return result;
}


bool ITunesInterface::haveApplication() const {
    if (!p->iTunes) {
        p->iTunes = [SBApplication applicationWithBundleIdentifier:@"com.apple.iTunes"];
    }
    return p->iTunes != nil && [p->iTunes isRunning];
}
