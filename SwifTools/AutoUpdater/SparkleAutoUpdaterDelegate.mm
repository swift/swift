/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#import <SwifTools/AutoUpdater/SparkleAutoUpdaterDelegate.h>

#include <string>

#include <Swiften/Base/Log.h>

#include <SwifTools/Cocoa/CocoaUtil.h>

using namespace Swift;

@implementation SparkleAutoUpdaterDelegate

@synthesize onNewUpdateState;

- (void)updater:(SUUpdater *)updater didFinishLoadingAppcast:(SUAppcast *)appcast {
    (void)updater;
    (void)appcast;
    onNewUpdateState(AutoUpdater::State::DownloadingUpdate);
}

- (void)updater:(SUUpdater *)updater didFindValidUpdate:(SUAppcastItem *)update {
    (void)updater;
    (void)update;
}

- (id <SUVersionComparison>)versionComparatorForUpdater:(SUUpdater *)updater {
    (void)updater;
    return nil;
}

- (void)updaterDidNotFindUpdate:(SUUpdater *)updater {
    (void)updater;
    onNewUpdateState(AutoUpdater::State::NoUpdateAvailable);
}

- (void)updater:(SUUpdater *)updater willDownloadUpdate:(SUAppcastItem *)item withRequest:(NSMutableURLRequest *)request {
    (void)updater;
    (void)item;
    (void)request;
    onNewUpdateState(AutoUpdater::State::DownloadingUpdate);
}

- (void)updater:(SUUpdater *)updater failedToDownloadUpdate:(SUAppcastItem *)item error:(NSError *)error {
    (void)updater;
    (void)item;
    SWIFT_LOG(error) << ns2StdString([error localizedDescription]) << std::endl;
    onNewUpdateState(AutoUpdater::State::ErrorCheckingForUpdate);
}

- (void)updater:(SUUpdater *)updater willInstallUpdate:(SUAppcastItem *)update {
    (void)updater;
    (void)update;
}

- (void)updater:(SUUpdater *)updater willInstallUpdateOnQuit:(SUAppcastItem *)item immediateInstallationInvocation:(NSInvocation *)invocation {
    (void)updater;
    (void)item;
    (void)invocation;
    onNewUpdateState(AutoUpdater::State::RestartToInstallUpdate);
}

- (void)updater:(SUUpdater *)updater didAbortWithError:(NSError *)error {
    (void)updater;
    if ([error code] == SUNoUpdateError) {
        onNewUpdateState(AutoUpdater::State::NoUpdateAvailable);
    }
    else {
        SWIFT_LOG(error) << ns2StdString([error localizedDescription]) << std::endl;
        onNewUpdateState(AutoUpdater::State::ErrorCheckingForUpdate);
    }
}

@end
