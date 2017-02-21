/*
 * Copyright (c) 2016-2017 Isode Limited.
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
        State currentState = State::NotCheckedForUpdatesYet;
};

SparkleAutoUpdater::SparkleAutoUpdater(const std::string& appcastFeed) : d(new Private()) {
    d->updater = [SUUpdater sharedUpdater];
    [d->updater retain];

    d->delegate = boost::intrusive_ptr<SparkleAutoUpdaterDelegate>([[SparkleAutoUpdaterDelegate alloc] init], false);
    [d->delegate.get() setOnNewUpdateState: [&](AutoUpdater::State updatedState){
        setCurrentState(updatedState);
    }];
    [d->updater setDelegate: d->delegate.get()];

    [d->updater setAutomaticallyChecksForUpdates: true];
    // Automatically check for an update after a day.
    [d->updater setUpdateCheckInterval: 86400];
    [d->updater setAutomaticallyDownloadsUpdates: true];

    setAppcastFeed(appcastFeed);
}

SparkleAutoUpdater::~SparkleAutoUpdater() {
    [d->updater release];
}

void SparkleAutoUpdater::setAppcastFeed(const std::string& appcastFeed) {
    NSURL* nsurl = [NSURL URLWithString: std2NSString(appcastFeed)];
    [d->updater setFeedURL: nsurl];
}

void SparkleAutoUpdater::checkForUpdates() {
    if (!(getCurrentState() == State::CheckingForUpdate ||
          getCurrentState() == State::DownloadingUpdate ||
          getCurrentState() == State::RestartToInstallUpdate)) {
        setCurrentState(State::CheckingForUpdate);
        [d->updater resetUpdateCycle];
        [d->updater checkForUpdatesInBackground];
    }
}

void SparkleAutoUpdater::setCurrentState(AutoUpdater::State updatedState) {
    d->currentState = updatedState;
    onUpdateStateChanged(d->currentState);
}

AutoUpdater::State SparkleAutoUpdater::getCurrentState() {
    return d->currentState;
}

}
