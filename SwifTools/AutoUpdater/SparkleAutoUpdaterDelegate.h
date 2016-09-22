/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <functional>

#import <Cocoa/Cocoa.h>

#import <Sparkle/Sparkle.h>

namespace Swift {
    class SparkleAutoUpdater;
}

@interface SparkleAutoUpdaterDelegate : NSObject<SUUpdaterDelegate>
@property (atomic)  std::function< void ()> updateDownloadFinished;

- (void)updater:(SUUpdater *)updater didFinishLoadingAppcast:(SUAppcast *)appcast;

- (void)updater:(SUUpdater *)updater didFindValidUpdate:(SUAppcastItem *)update;

- (id <SUVersionComparison>)versionComparatorForUpdater:(SUUpdater *)updater;

- (void)updaterDidNotFindUpdate:(SUUpdater *)update;

- (void)updater:(SUUpdater *)updater willInstallUpdate:(SUAppcastItem *)update;

- (void)updater:(SUUpdater *)updater willInstallUpdateOnQuit:(SUAppcastItem *)item immediateInstallationInvocation:(NSInvocation *)invocation;

- (void)updater:(SUUpdater *)updater didAbortWithError:(NSError *)error;
@end
