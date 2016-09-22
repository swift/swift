/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/AutoUpdater/SparkleAutoUpdater.h>

#include <Cocoa/Cocoa.h>
#include <Sparkle/Sparkle.h>

#include <SwifTools/AutoUpdater/SparkleAutoUpdaterDelegate.h>
#include <SwifTools/Cocoa/CocoaUtil.h>

namespace Swift {

class SparkleAutoUpdater::Private {
    public:
        SUUpdater* updater;
        boost::intrusive_ptr<SparkleAutoUpdaterDelegate> delegate;
        bool restartToUpdate = false;
};

SparkleAutoUpdater::SparkleAutoUpdater(const std::string& url) {
    d = new Private;

    d->updater = [SUUpdater sharedUpdater];
    [d->updater retain];

    d->delegate = boost::intrusive_ptr<SparkleAutoUpdaterDelegate>([[SparkleAutoUpdaterDelegate alloc] init], false);
    [d->delegate.get() setUpdateDownloadFinished: [&](){
        d->restartToUpdate = true;
        onSuggestRestartToUserToUpdate();
    }];
    [d->updater setDelegate: d->delegate.get()];

    [d->updater setAutomaticallyChecksForUpdates: true];
    // Automatically check for an update after a day.
    [d->updater setUpdateCheckInterval: 86400];
    [d->updater setAutomaticallyDownloadsUpdates: true];

    NSURL* nsurl = [NSURL URLWithString: std2NSString(url)];
    [d->updater setFeedURL: nsurl];
}

SparkleAutoUpdater::~SparkleAutoUpdater() {
    [d->updater release];
    delete d;
    SWIFT_LOG(warning) << std::endl;
}

void SparkleAutoUpdater::checkForUpdates() {
    //[d->updater resetUpdateCycle]; // This is useful for testing to force a check ot start.
    [d->updater checkForUpdatesInBackground];
}

bool SparkleAutoUpdater::recommendRestartToUpdate() {
    return d->restartToUpdate;
}

}
