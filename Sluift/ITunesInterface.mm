/*
 * Copyright (c) 2014 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ITunesInterface.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfour-char-constants"
#import <Sluift/iTunes.h>
#pragma clang diagnostic pop
#include <ScriptingBridge/ScriptingBridge.h>

#include <boost/smart_ptr/make_shared.hpp>
#include <boost/optional.hpp>
#include <SwifTools/Cocoa/CocoaUtil.h>

using namespace Swift;

struct ITunesInterface::Private {
	Private() : iTunes(nil) {
	}

	iTunesApplication* iTunes;
};

ITunesInterface::ITunesInterface() : p(boost::make_shared<Private>()) {
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
	result.name = NS2STDSTRING(currentTrack.name);
	result.artist = NS2STDSTRING(currentTrack.artist);
	result.album = NS2STDSTRING(currentTrack.album);
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
