/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <functional>

#import <Cocoa/Cocoa.h>

#import <Sparkle/Sparkle.h>

#include <SwifTools/AutoUpdater/AutoUpdater.h>

@interface SparkleAutoUpdaterDelegate : NSObject<SUUpdaterDelegate>
@property (atomic)  std::function< void (Swift::AutoUpdater::State)> onNewUpdateState;

- (void)updater:(SUUpdater *)updater didFinishLoadingAppcast:(SUAppcast *)appcast;

- (void)updater:(SUUpdater *)updater didFindValidUpdate:(SUAppcastItem *)update;

- (id <SUVersionComparison>)versionComparatorForUpdater:(SUUpdater *)updater;

- (void)updaterDidNotFindUpdate:(SUUpdater *)update;

- (void)updater:(SUUpdater *)updater willDownloadUpdate:(SUAppcastItem *)item withRequest:(NSMutableURLRequest *)request;

- (void)updater:(SUUpdater *)updater failedToDownloadUpdate:(SUAppcastItem *)item error:(NSError *)error;

- (void)updater:(SUUpdater *)updater willInstallUpdate:(SUAppcastItem *)update;

- (void)updater:(SUUpdater *)updater willInstallUpdateOnQuit:(SUAppcastItem *)item immediateInstallationInvocation:(NSInvocation *)invocation;

- (void)updater:(SUUpdater *)updater didAbortWithError:(NSError *)error;
@end
