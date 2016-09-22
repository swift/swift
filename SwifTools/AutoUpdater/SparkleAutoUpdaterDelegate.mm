/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#import "SwifTools/AutoUpdater/SparkleAutoUpdaterDelegate.h"

#include <string>

#include <Swiften/Base/Log.h>

#include <SwifTools/Cocoa/CocoaUtil.h>

using namespace Swift;

@implementation SparkleAutoUpdaterDelegate

@synthesize updateDownloadFinished;

- (void)updater:(SUUpdater *)updater didFinishLoadingAppcast:(SUAppcast *)appcast {
    (void)updater;
    (void)appcast;
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
}

- (void)updater:(SUUpdater *)updater willInstallUpdate:(SUAppcastItem *)update {
    (void)updater;
    (void)update;
}

- (void)updater:(SUUpdater *)updater willInstallUpdateOnQuit:(SUAppcastItem *)item immediateInstallationInvocation:(NSInvocation *)invocation {
    (void)updater;
    (void)item;
    (void)invocation;
    updateDownloadFinished();
}

- (void)updater:(SUUpdater *)updater didAbortWithError:(NSError *)error {
    (void)updater;
    SWIFT_LOG(error) << ns2StdString([error localizedDescription]) << std::endl;
}

@end
